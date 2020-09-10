#version 460 core
#extension GL_GOOGLE_include_directive : require
#define GUI_VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec2 position;
layout(location = VERT_A_TEXCOORD0_LOC) in vec2 texCoords;
layout(location = VERT_A_COLOR_LOC) in vec4 color;

layout(location = VERT_INTERFACE_BLOCK_LOC) out VS_OUT {
  vec2 texCoords;
  vec4 color;
}
vs_out;

layout(location = VERT_U_PROJECTION_MAT_LOC) uniform mat4 projection_mat;

void main() {
  gl_Position = projection_mat * vec4(position, 0.0f, 1.0f);
  vs_out.texCoords = texCoords;
  vs_out.color = color;
}
