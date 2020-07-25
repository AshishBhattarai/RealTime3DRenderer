/**
 * First part of split sum approximation.
 * Pre-filtered environment map generation.
 **/
#version 460 core
#extension GL_GOOGLE_include_directive: require

#define IBL_SPECULAR_CONVOLUTION_FRAGMENT_SHADER
#include "config.h"

#define PI 3.1415926538f
#define SAMPLE_COUNT 1024u

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout(location = VERT_V_DIRECTION_LOC) in vec3 texDir;

layout(location = FRAG_U_ENVMAP_LOC, binding = FRAG_U_ENVMAP_BINDING) uniform samplerCube envMap;
layout(location = FRAG_U_ROUGHNESS_LOC) uniform float roughness;

/**
 * van der corput sequance base 2 (ie: RadicanlInverse(2), Reverse bits)
 * Reverses the buts in a give 32-bit integer
 */
float radicalInverse_Vdc(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

/**
 * Hammersly sequqnce, 2D quasirandom
 */
vec2 hammersley(uint i, uint N) {
    return vec2(i/N, radicalInverse_Vdc(i));
}

/**
 * Inverse transform GGX (NDF from BDRF) sampling.
 * For importance sampling.
 */
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    // inverse transform sampling, spherical (theta - polar, phi - azumith)
    float a = roughness*roughness;
    float phi = 2.0f * PI * Xi.x;
    float cosTheta = sqrt((1.0f - Xi.y) / ((a*a - 1.0f) * Xi.y + 1.0f));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

    // spherical to cartesian
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector (like diffuse ibl)
    vec3 up = abs(N.z) < 0.99f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, N)); // right-X
    vec3 bitangent = cross(N, tangent); // up-Y

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float distrubutionGGX(float NoH, float roughness) {
    float r = roughness * roughness;
    float r2 = r * r;
    float NoH2 = NoH * NoH;
    float d = (NoH2 * (r2 - 1.0f) + 1.0f);
    return r2 / (d * d * PI);
}

void main() {
   vec3 N = normalize(texDir);
   vec3 R = N;
   vec3 V = R;

   float totalWeight = 0.0f;
   vec3 prefilteredColor = vec3(0.0f);
   for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
       vec2 Xi = hammersley(i, SAMPLE_COUNT);
       vec3 H = importanceSampleGGX(Xi, N, roughness);
       // H to L
       vec3 L = normalize(2.0 * dot(V, H) * H - V);
       // check if L affects this pixel
       float NdotL = max(dot(N, L), 0.0f);
       if (NdotL > 0.0f) {
           // reduce aliasing taking mipMap of envMap base on pdf
           float NoH = max(dot(N, H), 0.0f);
           float HoV = max(dot(H, V), 0.0f);
           float D = distrubutionGGX(NoH, roughness);
           float pdf = (D * NoH / (4.0 * HoV)) + 0.0001;
           float resolution = textureSize(envMap, 0).x; // resolution of envMap per face
           float saTexel = 2.0 * PI / (3.0 * resolution * resolution);
           float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
           float mipLevel = roughness == 0.0 ? 0.0 : max(0.5 * log2(saSample / saTexel), 0.0f);

           prefilteredColor	+= textureLod(envMap, L, mipLevel).rgb * NdotL;
           totalWeight += NdotL;
       }
   }
  prefilteredColor /= totalWeight;
  fragColor = vec4(prefilteredColor, 1.0f);
}

