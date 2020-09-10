#version 460 core
#extension GL_GOOGLE_include_directive : require
#define GUI_FRAGMENT_SHADER
#include "config.h"

layout(location = COLOR_ATTACHMENT0) out vec4 FragColor;

layout(location = VERT_INTERFACE_BLOCK_LOC) in VS_OUT {
  vec2 texCoords;
  vec4 color;
}
fs_in;

layout(binding = FRAG_U_TEXTURE_BND) uniform sampler2D tex;

void main() {
  FragColor = fs_in.color * texture(tex, fs_in.texCoords);
  // FragColor.rgb = pow(FragColor.rgb, vec3(2.2f));
}
