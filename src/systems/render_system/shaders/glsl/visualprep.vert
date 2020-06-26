#version 460 core
#extension GL_GOOGLE_include_directive: require

#define VISUAL_PREP_VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec2 aPos;
layout(location = VERT_V_TEX_COORDS_LOC) out vec2 texCoord;

void main(void) {
    gl_Position = vec4(aPos, 0.0f, 1.0f);
    texCoord = aPos.xy*0.5f + 0.5f;  // TODO: CHECK
}
