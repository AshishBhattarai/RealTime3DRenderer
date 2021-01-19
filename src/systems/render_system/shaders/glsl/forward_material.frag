#version 460 core
#extension GL_GOOGLE_include_directive : require
#define FORWARD_FRAGMENT_SHADER
#include "brdf.h"
#include "config.h"
#include "math_constants.h"

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;

layout(location = VERT_INTERFACE_BLOCK_LOC) in VertexData {
#ifdef TEXTURE_MATERIAL
  vec2 texCoord;
#endif
  vec3 worldPos;
  vec3 normal;
  vec3 camPos;
}
fs_in;

struct PointLight {
  vec3 position;
  vec3 color;
  float radius;
  float intensity; // lumen
};

#ifndef TEXTURE_MATERIAL
struct Material {
  vec3 albedo;
  float metallic;
  float roughness;
  float ao;
};
#endif

/* Material and light */
#ifdef TEXTURE_MATERIAL
// Opaque types such as sampler cannot be inside struct
layout(binding = FRAG_U_MATERIAL_ALBEDO_BND) uniform sampler2D material_albedo;
layout(binding = FRAG_U_MATERIAL_METALLIC_ROUGHNESS_BND) uniform sampler2D
    material_metallicRoughness;
layout(binding = FRAG_U_MATERIAL_AO_BND) uniform sampler2D material_ao;
layout(binding = FRAG_U_MATERIAL_NORMAL_BND) uniform sampler2D material_normal;
layout(binding = FRAG_U_MATERIAL_EMISSION_BND) uniform sampler2D material_emission;
#else
layout(location = FRAG_U_MATERIAL_ALBEDO_LOC) uniform Material material;
#endif
layout(location = FRAG_U_POINT_LIGHT0_POS) uniform PointLight pointLights[MAX_POINT_LIGHTS];
layout(location = FRAG_U_POINT_LIGHT_SIZE) uniform int pointLightSize;

/* IBL maps */
layout(binding = FRAG_U_IRRADIANCE_MAP_BND) uniform samplerCube irradianceMap;
layout(binding = FRAG_U_PREFILTERED_MAP_BND) uniform samplerCube prefilteredMap;
layout(binding = FRAG_U_BRDF_INTEGRATION_MAP_BND) uniform sampler2D brdfIntegrationMap;

// light attenuation
float invSqureAttenuation(float distance, float radius) {
  return pow(clamp(1.0f - pow((distance / radius), 4.0f), 0.0f, 1.0f), 2.0f) /
         (distance * distance + 1.0f);
}

#ifdef TEXTURE_MATERIAL
// tangent-normals hax, bad performance
vec3 getNormalFromMap() {
  vec3 tangentNormal = texture(material_normal, fs_in.texCoord).xyz * 2.0 - 1.0;

  vec3 Q1 = dFdx(fs_in.worldPos);
  vec3 Q2 = dFdy(fs_in.worldPos);
  vec2 st1 = dFdx(fs_in.texCoord);
  vec2 st2 = dFdy(fs_in.texCoord);

  vec3 N = normalize(fs_in.normal);
  vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
  vec3 B = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}
#endif

void main() {
  // sample texture
#ifdef TEXTURE_MATERIAL
  vec3 albedo = texture(material_albedo, fs_in.texCoord).rgb;
  vec3 metallicRoughness = texture(material_metallicRoughness, fs_in.texCoord).rgb;
  vec3 emission = texture(material_emission, fs_in.texCoord).rgb;
  /* https://github.com/KhronosGroup/glTF-Sample-Models/issues/54 */
  float metallic = metallicRoughness.b;
  float roughness = metallicRoughness.g;
  // https://github.com/KhronosGroup/glTF/issues/857#issuecomment-290530762
  float ao = texture(material_ao, fs_in.texCoord).r;
  vec3 N = getNormalFromMap();
#else
  vec3 albedo = material.albedo;
  float metallic = material.metallic;
  float roughness = material.roughness;
  float ao = material.ao;
  const vec3 emission = vec3(0, 0, 0);
  vec3 N = normalize(fs_in.normal);
#endif

  vec3 V = normalize(fs_in.camPos - fs_in.worldPos);
  vec3 R = reflect(-V, N); // reflect viewDir on normal

  // calculate surface reflection at zero incidence
  vec3 F0 = vec3(0.04f);
  F0 = mix(F0, albedo, metallic);
  float NoV = max(dot(N, V), 0.0f);

  /**
   * Direct lighting
   *
   * calculate total reflected irradiance by current fragment using
   * reflectance equation.
   * Where the light sources are our point lights.
   */
  vec3 Lo = vec3(0.0f);
  for (int i = 0; i < pointLightSize; ++i) {
    // fragment radiance per pixel
    vec3 lwSub = pointLights[i].position - fs_in.worldPos;
    vec3 lightDir = normalize(lwSub);
    vec3 halfway = normalize(lightDir + V);
    float distance = length(lwSub);
    float attenuation = invSqureAttenuation(distance, pointLights[i].radius);
    vec3 radiance = pointLights[i].color * pointLights[i].intensity * attenuation;

    // apply cook-torrance brdf (D*F*G)/(4(Wo.n)*(Wi*n)
    float NoL = max(dot(N, lightDir), 0.0f);

    float roughness = max(roughness, 0.05f);
    float NDF = distributionGGX(max(dot(N, halfway), 0.0f), roughness);
    float G = geometrySmith(NoV, NoL, roughness);
    vec3 F = fresnelSchlick(clamp(dot(V, halfway), 0.0f, 1.0f), F0);

    vec3 num = NDF * G * F;
    float denom = 4.0f * NoV * NoL;
    vec3 specular = num / max(denom, 0.001f); // to prevent denom from being zero

    vec3 kD = vec3(1.0f) - F; // F is specular ratio

    /**
     * Metallic material dont have diffuse.
     * Linear blend if partly metal (pure metals have no diffuse light).
     */
    kD *= 1.0f - metallic;

    // add to total outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NoL;
  }
  /*
   * Indirect lighting with IBL.
   * Using an EnvMap has a source of surrounding light (diffuse + specular).
   */
  vec3 F = fresnelSchlickRoughness(NoV, F0, roughness);
  vec3 kD = 1.0f - F;
  kD *= 1.0f - metallic;
  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = kD * irradiance * albedo;
  const float MAX_REFLECTION_LOD = 4.0f; // max mipLevel for prefilteredMap
  vec3 prefilteredColor = textureLod(prefilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;
  vec2 envBRDF = texture(brdfIntegrationMap, vec2(NoV, roughness)).rg;
  vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
  vec3 ambient = (diffuse + specular) * ao;
  // combine direct and indirect language
  vec3 color = ambient + Lo + emission;
  fragColor = vec4(color, 1.0f);
}
