#version 460 core
#extension GL_GOOGLE_include_directive: require
#define VISUAL_PREP_FRAGMENT_SHADER
#include "config.h"
#include "ibl_specular_common.h"

layout (location = COLOR_ATTACHMENT0) out vec2 fragColor;
layout (location = VERT_V_TEX_COORDS_LOC) in vec2 texCoord;

// Geometry Function for microfacets shadowing & masking
float geometrySchlickGGX(float NoV, float roughness) {
    float r = roughness;
    float k = (r * r) / 2.0f;
    return NoV / (NoV * (1.0f - k) + k);
}

float geometrySmith(float NoV, float NoL, float roughness) {
    float ggxL = geometrySchlickGGX(NoL, roughness);
    float ggxV = geometrySchlickGGX(NoV, roughness);
    return ggxL * ggxV;
}

void main() {
    float NoV = texCoord.x;
    float roughness = texCoord.y;
    // view - (sin(theta), 0.0f, cos(theta)) zx-pane, coz z is normal
    vec3 V;
    V.x = sqrt(1.0f - NoV*NoV); // sin
    V.y = 0.0f;
    V.z = NoV; // cos
    // normal
    vec3 N = vec3(0.0f, 0.0f, 1.0f); // facing z
    vec2 result = vec2(0.0f, 0.0f);
    for(uint i = 0; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NoL = max(L.z, 0.0f);
        float NoH = max(H.z, 0.0f);
        float VoH = max(dot(V, H), 0.0f);

        if(NoL > 0.0) {
            // apply equation
            float G = geometrySmith(max(dot(N, V), 0.0f), NoL, roughness);
            float G_Vis = (G * VoH) / (NoH * NoV); // (G*v.h)/(n.h*v.n)
            float Fc = pow(1.0f - VoH, 5.0f); // a, F without F0
            result.x += (1.0 - Fc) * G_Vis;
            result.y += Fc * G_Vis;
        }
    }
    result.x /= float(SAMPLE_COUNT);
    result.y /= float(SAMPLE_COUNT);
    fragColor = result;
}
