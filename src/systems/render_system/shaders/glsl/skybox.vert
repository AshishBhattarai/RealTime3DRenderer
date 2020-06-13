#version 460 core
#extension GL_GOOGLE_include_directive: require

#define SKYBOX_VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec3 position;
layout(location = VERT_V_DIRECTION_LOC) out vec3 vertDir;

struct Matrices {
    mat4 projection;
    mat4 view;
};

layout(std140, binding = VERT_UB_GENERAL_LOC) uniform GeneralUB {
    mat4 projection;
    mat4 view;
};

void main(void)
{
    vertDir = normalize(position); // direction on vertex
    vec4 pos = projection * mat4(mat3(view)) * vec4(position.xyz, 1.0f);
    gl_Position = pos.xyww;
}
