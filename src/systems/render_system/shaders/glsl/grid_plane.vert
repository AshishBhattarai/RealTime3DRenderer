#version 460 core
#extension GL_GOOGLE_include_directive : require

#define GRID_PLANE_VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec2 aPos;

layout(std140, binding = VERT_UB_GENERAL_LOC) uniform GeneralUB {
  mat4 projection;
  mat4 view;
};

layout(location = VERT_INTERFACE_BLOCK_LOC) out VS_OUT {
  vec3 worldPos;
  float scale;
  vec2 uv;
}
vs_out;

layout(location = VERT_U_TRANSFORMATION_LOC) uniform mat4 transform;

void main(void) {
  vec4 pos = transform * vec4(aPos, 0.0f, 1.0f);
  vs_out.uv = aPos * 0.5f + 0.5f;
  vs_out.worldPos = pos.xyz;
  vs_out.scale = transform[0].x;
  gl_Position = projection * view * pos;
}
