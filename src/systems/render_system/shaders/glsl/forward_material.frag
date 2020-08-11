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

#ifdef TEXTURE_MATERIAL
struct Material {
  sampler2D albedo;
  sampler2D metallicRoughness;
  sampler2D roughness;
  sampler2D ao;
};
#else
struct Material {
  vec3 albedo;
  float metallic;
  float roughness;
  float ao;
};
#endif

/* Material and light */
layout(location = FRAG_U_MATERIAL_ALBEDO_LOC) uniform Material material;
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

void main() {
  vec3 N = normalize(fs_in.normal);
  vec3 V = normalize(fs_in.camPos - fs_in.worldPos);
  vec3 R = reflect(-V, N); // reflect viewDir on normal

  // sample texture
#ifdef TEXTURE_MATERIAL
  vec3 albedo = texture(material.albedo, fs_in.texCoord).rgb;
  vec3 metallicRoughness = texture(material.metallicRoughness, fs_in.texCoord).rgb;
  float metallic = metallicRoughness.r;
  float roughness = metallicRoughness.g;
  vec3 ao = texture(material.metallicRoughness, fs_in.texCoord).rgb;
#else
  vec3 albedo = material.albedo;
  float metallic = material.metallic;
  float roughness = material.roughness;
  float ao = material.ao;
#endif

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
  vec3 kS = fresnelSchlickRoughness(clamp(dot(V, N), 0.0f, 1.0f), F0, roughness);
  vec3 kD = 1.0f - kS;
  kD *= 1.0f - metallic;
  vec3 irradiance = texture(irradianceMap, N).rgb;
  vec3 diffuse = kD * (irradiance * albedo);
  const float MAX_REFLECTION_LOD = 4.0f; // max mipLevel for prefilteredMap
  vec3 prefilteredColor =
      textureLod(prefilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;
  vec2 envBRDF = texture(brdfIntegrationMap, vec2(NoV, roughness)).rg;
  vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
  vec3 ambient = (diffuse + specular) * ao;

  // combine direct and indirect language
  vec3 color = ambient + Lo;
  fragColor = vec4(color, 1.0f);
}
