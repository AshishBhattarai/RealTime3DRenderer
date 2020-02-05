#version 460 core
#extension GL_GOOGLE_include_directive: require

#define VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec3 position;
layout(location = VERT_A_NORMAL_LOC) in vec3 normal;

struct Matrices {
    mat4 projection;
    mat4 transformation;
    mat4 view;
};

layout(location = VERT_U_PROJECTION_LOC) uniform Matrices matrices; // 0 - proj, 1 - transform, 2 - view

void main() {
    gl_Position = matrices.projection * matrices.view * matrices.transformation * vec4(position, 1.0f);
}
