#version 460 core
#extension GL_GOOGLE_include_directive: require
#define VISUAL_PREP_FRAGMENT_SHADER
#include "config.h"

layout (location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout (location = VERT_V_TEX_COORDS_LOC) in vec2 texCoord;

layout (location = FRAG_U_EXPOSURE_LOC) uniform float exposure;
layout (location = FRAG_U_GAMMA_LOC) uniform float gamma;
layout (location = FRAG_U_FRAME_TEXTURE_LOC) uniform sampler2D frameTexture;

void main(void) {
   vec3 color = texture(frameTexture, texCoord).xyz;
   // tonemapping(simpleReinhardToneMapping) & gamma correction
   // https://www.shadertoy.com/view/lslGzl
   color *= exposure/(1.0f + color / exposure);
   color = pow(color, vec3(1.0f / gamma));
   fragColor = vec4(color.xyz, 1.0f);
}
