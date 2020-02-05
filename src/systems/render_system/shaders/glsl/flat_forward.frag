#version 460 core
#extension GL_GOOGLE_include_directive: require

#define FRAGMENT_SHADER
#include "config.h"

layout(location = COLOR_ATTACHMENT0) out vec4 color;

void main() {
    color = vec4(1.0f);
}
