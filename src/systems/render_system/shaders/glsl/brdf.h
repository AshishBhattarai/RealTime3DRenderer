#ifndef PBR_COMMON_H
#define PBR_COMMON_H

#include "math_constants.h"

/**
 * BRDF functions used for physically based approximation
 */

// calculates ratio of light that gets reflected
vec3 fresnelSchlick(float VoH, vec3 F0) {
    return F0 + (1.0f - F0) * pow(1.0f - VoH, 5.0f);
}

// fresnelSchlick that takes roughness into account for indirect lighting using irradianceMap
vec3 fresnelSchlickRoughness(float VoH, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - VoH, 5.0f);
}

// NDF for microfactes oriented toward halfway
float distributionGGX(float NoH, float roughness) {
    float r = roughness * roughness;
    float r2 = r * r; // gives better result based on observation by disney and epic games.
    float NoH2 = NoH*NoH;
    float d = (NoH2 * (r2 - 1.0f) + 1.0f);
    return r2 / (d * d * PI);
}

// Geometry Function for microfacets shadowing & masking
float geometrySchlickGGX(float NoV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    return NoV / (NoV * (1.0f - k) + k);
}

float geometrySmith(float NoV, float NoL, float roughness) {
    float ggxL = geometrySchlickGGX(NoL, roughness);
    float ggxV = geometrySchlickGGX(NoV, roughness);
    return ggxL * ggxV;
}

// Gemotry function for ibl
float geometrySchlickGGXIBL(float NoV, float roughness) {
    float r = roughness;
    float k = (r * r) / 2.0f;
    return NoV / (NoV * (1.0f - k) + k);
}

float geometrySmithIBL(float NoV, float NoL, float roughness) {
    float ggxL = geometrySchlickGGXIBL(NoL, roughness);
    float ggxV = geometrySchlickGGXIBL(NoV, roughness);
    return ggxL * ggxV;
}

#endif // PBR_COMMON_H
