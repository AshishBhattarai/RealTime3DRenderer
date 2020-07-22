#version 460 core
#extension GL_GOOGLE_include_directive: require

#define SKYBOX_FRAGMENT_SHADER
#include "config.h"
#define PI 3.1415926538f
#define SAMPLE_DELTA 0.025f

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout(location = VERT_V_DIRECTION_LOC) in vec3 texDir;

layout(location = FRAG_U_ENVMAP_LOC) uniform samplerCube envMap;

void main(void) {
    vec3 normal = normalize(texDir);
    vec3 irradiance = vec3(0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = cross(up, normal);
    up = cross(normal, right);
    int nrSamples = 0;
    // (phi, theta) coords for unit sphere
    for(float phi = 0.0; phi < 2.0 * PI; phi += SAMPLE_DELTA) { // azimuth
        for(float theta = 0.0f; theta < 0.5f * PI; theta += SAMPLE_DELTA) { // zenith
            // spherical to cartesian coords (in tangent space)
            vec3 cartesian = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // add current frag normal orientation (tangent space to world)
            vec3 sampleVec = cartesian.x * right + cartesian.y * up + cartesian.z * normal;
            // take sample (phi, theta), formula based on render equation
            irradiance += texture(envMap, sampleVec).rgb * cos(theta) * sin(theta);
            // cost(theta) - lambert
            // sin(theta) - solid angle
            ++nrSamples;
        }
    }
    irradiance = PI * irradiance * ( 1.0f / float(nrSamples));
    fragColor = vec4(irradiance, 1.0f);
}
