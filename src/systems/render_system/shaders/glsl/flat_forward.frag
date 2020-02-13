#version 460 core
#extension GL_GOOGLE_include_directive: require

#define FRAGMENT_SHADER
#include "config.h"
#define PI 3.1415926538f

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;

layout (location = VERT_INTERFACE_BLOCK_LOC) in VertexData {
   vec2 texCoord;
   vec3 worldPos;
   vec3 normal;
   vec3 viewDir;
} fs_in;

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float radius;
};

layout(location = FRAG_U_MATERIAL_ALBEDO_LOC) uniform Material material;
layout(location = FRAG_U_MATERIAL_POINT_LIGHT0_POS) uniform PointLight pointLights[MAX_POINT_LIGHTS];

float invSqureAttenuation(float distance, float radius) {
    return pow(clamp(pow((distance / radius), 4.0f), 0.0f, 1.0f), 2.0f)/(distance* distance + 1.0f);
}

// calculates ratio of light that gets reflected
vec3 fresnelSchlick(float HoV, vec3 F0) {
    return F0 + (1.0f - F0) * pow(1.0f - HoV, 5.0f);
}

// NDF for microfactes oriented toward halfway
float distributionGGX(float NoH, float roughness) {
    float a = roughness * roughness; // gives better result based on observation by disney and epic games.
    float a2 = a * a;
    float NoH2 = NoH*NoH;
    float denom = (NoH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    return a2/denom;
}

// Geometry Function for microfacets shadowing & masking
float geometrySchlickGGX(float NoV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    return NoV / (NoV * (1.0f - k) + k);
}

float geometrySmith(float NoV, float NoL, float roughness) {
    float ggxV = geometrySchlickGGX(NoV, roughness);
    float ggxL = geometrySchlickGGX(NoL, roughness);
    return ggxV * ggxL;
}

void main() {
    vec3 normal = normalize(fs_in.normal);
    vec3 viewDir = normalize(fs_in.viewDir);

    // calculate total reflected irradiance reflectance equation
    vec3 Lo = vec3(0.0f);
    for(int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        // fragment radiance per pixel
        vec3 lwSub = pointLights[i].position - fs_in.worldPos;
        vec3 lightDir = normalize(lwSub);
        vec3 halfway = normalize(lightDir + viewDir);
        float distance = length(lwSub);
        float attenuation = invSqureAttenuation(distance, pointLights[i].radius);
        vec3 radiance = pointLights[i].color * attenuation;

        // apply cook-torrance brdf (D*F*G)/(4(Wo.n)*(Wi*n)
        float NoV = dot(normal, viewDir);
        float NoL = dot(normal, lightDir);
        vec3 F0 = vec3(0.04f); // surface reflection at zero incidence
        F0 = mix(F0, material.albedo, material.metallic);

        vec3 F = fresnelSchlick(dot(halfway, viewDir), F0);
        float NDF = distributionGGX(dot(normal, halfway), material.roughness);
        float G = geometrySmith(NoV, NoL, material.roughness);

        vec3 num = NDF*G*F;
        float denom = 4.0 * max(NoV, 0.0f) * max(NoL, 0.0f);
        vec3 specular = num / max(denom, 0.001f); // to prevent denom from being zero

        vec3 kD = vec3(1.0f) - F; // F is specular ratio
        kD *= 1.0f - material.metallic; // metallic material dont have diffuse

        // add to total irradiance Lo
        Lo += (kD*material.albedo/PI + specular)*radiance*NoL;
    }
    vec3 ambient = vec3(0.03) * material.albedo * material.ao;
    vec3 color = ambient + Lo;

    // gamma correction & tonemapping(Rienhard) (do this on another pass(post-processing) later
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f / 2.2f));
    fragColor = vec4(color, 1.0f);
}
