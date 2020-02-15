#version 460 core
#extension GL_GOOGLE_include_directive: require

#define VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec3 position;
layout(location = VERT_A_NORMAL_LOC) in vec3 normal;
layout(location = VERT_A_TEXTCOORD0_LOC) in vec2 texCoord;

layout (location = VERT_INTERFACE_BLOCK_LOC) out VertexData {
   vec2 texCoord;
   vec3 worldPos;
   vec3 normal;
} vs_out;

struct Matrices {
    mat4 projection;
    mat4 transformation;
    mat4 view;
};

/**
 * 0 - proj, 1 - transform, 2 - view
 */
layout(location = VERT_U_PROJECTION_LOC) uniform Matrices matrices;

void main() {
    vec4 worldPos = matrices.transformation * vec4(position, 1.0f);
    gl_Position = matrices.projection * matrices.view * worldPos;
    vs_out.texCoord = texCoord;
    vs_out.worldPos = worldPos.xyz;
    vs_out.normal = mat3(matrices.transformation) * normal;
}
