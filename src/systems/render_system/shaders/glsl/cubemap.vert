#version 460 core
#extension GL_GOOGLE_include_directive: require

#define CUBEMAP_VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec3 position;
layout(location = VERT_V_DIRECTION_LOC) out vec3 vertDir;

struct Matrices {
    mat4 projection;
    mat4 view;
};

layout(location = VERT_U_PROJECTION_MAT_LOC) uniform Matrices matrices;

void main(void)
{
    vertDir = position; // direction on vertex
    vec4 pos = matrices.projection * mat4(mat3(matrices.view)) * vec4(position.xyz, 1.0f);
    gl_Position = pos.xyww;
}
