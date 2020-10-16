#version 460 core
#extension GL_GOOGLE_include_directive : require
#define GRID_PLANE_FRAGMENT_SHADER
#include "config.h"

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;

layout(location = VERT_INTERFACE_BLOCK_LOC) in VS_OUT {
  vec3 worldPos;
  float scale;
  vec2 uv;
}
fs_in;

layout(binding = FRAG_U_GRID_TEXTURE_BND) uniform sampler2D gridTexture;

void main(void) {
  vec2 worldCoord = fs_in.worldPos.xz;
  fragColor = texture(gridTexture, fs_in.uv * fs_in.scale * 0.5f);
  if (worldCoord.y >= -0.1f && worldCoord.y <= 0.1f)
    fragColor += vec4(0.7f, 0.0f, 0.0f, 1.0f); // x axis red color
  else if (worldCoord.x >= -0.1f && worldCoord.x <= 0.1f)
    fragColor += vec4(0.0f, 0.7f, 0.0f, 1.0f); // z axis green color
}
