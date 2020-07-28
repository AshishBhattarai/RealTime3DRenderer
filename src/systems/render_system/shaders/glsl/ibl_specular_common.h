#ifndef IBL_SPECULAR_COMMON_H
#define IBL_SPECULAR_COMMON_H

#define PI 3.1415926538f
#define SAMPLE_COUNT 1024u

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
  return vec2(i / float(N), radicalInverse_Vdc(i));
}

/**
 * Inverse transform GGX (NDF from BDRF) sampling.
 * For importance sampling.
 */
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
  // inverse transform sampling, spherical (theta - polar, phi - azumith)
  float a = roughness * roughness;
  float phi = 2.0f * PI * Xi.x;
  float cosTheta = sqrt((1.0f - Xi.y) / ((a * a - 1.0f) * Xi.y + 1.0f));
  float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

  // spherical to cartesian
  vec3 H;
  H.x = cos(phi) * sinTheta;
  H.y = sin(phi) * sinTheta;
  H.z = cosTheta;

  // from tangent-space vector to world-space sample vector (like diffuse ibl)
  vec3 up = abs(N.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
  vec3 tangent = normalize(cross(up, N)); // right-X
  vec3 bitangent = cross(N, tangent);     // up-Y

  vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sampleVec);
}

#endif // IBL_SPECULAR_COMMON_H
