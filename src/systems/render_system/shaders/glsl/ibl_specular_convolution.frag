/**
 * First part of split sum approximation.
 * Pre-filtered environment map generation.
 **/
#version 460 core
#extension GL_GOOGLE_include_directive: require

#define IBL_SPECULAR_CONVOLUTION_FRAGMENT_SHADER
#include "config.h"
#include "ibl_specular_common.h"

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout(location = VERT_V_DIRECTION_LOC) in vec3 texDir;

layout(binding = FRAG_U_ENVMAP_BND) uniform samplerCube envMap;
layout(location = FRAG_U_ROUGHNESS_LOC) uniform float roughness;

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
           // reduce aliasing taking samples from mipMap of envMap base on pdf
           float NoH = max(dot(N, H), 0.0f);
           float HoV = max(dot(H, V), 0.0f);
           float D = distrubutionGGX(NoH, roughness);
           float pdf = (D * NoH / (4.0 * HoV)) + 0.0001;
           float resolution = textureSize(envMap, 0).x; // resolution of envMap per face
           float saTexel = 2.0 * PI / (3.0 * resolution * resolution);
           float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
           float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

           prefilteredColor	+= textureLod(envMap, L, mipLevel).rgb * NdotL;
           totalWeight += NdotL;
       }
   }
  prefilteredColor /= totalWeight;
  fragColor = vec4(prefilteredColor, 1.0f);
}

