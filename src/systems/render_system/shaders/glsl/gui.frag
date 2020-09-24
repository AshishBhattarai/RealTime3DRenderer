#version 460 core
#extension GL_GOOGLE_include_directive : require
#define GUI_FRAGMENT_SHADER
#include "config.h"

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;

layout(location = VERT_INTERFACE_BLOCK_LOC) in VS_OUT {
  vec2 texCoords;
  vec4 color;
}
fs_in;

layout(binding = FRAG_U_TEXTURE_BND) uniform sampler2D tex;
layout(binding = FRAG_U_TEXTURE_CUBE_BND) uniform samplerCube texCube;

layout(location = FRAG_U_FACE_LOC) uniform int face;
layout(location = FRAG_U_LOD_LOC) uniform float lod;

void main() {
  if (face == 0) {
    // face == 0 is sampler2D
    fragColor = fs_in.color * texture(tex, fs_in.texCoords);
  } else {
    // project a face of samplerCube to 2D plane
    vec2 uv = 2.0f * fs_in.texCoords - 1.0f; // remap to [-1, 1]
    vec3 uvr = vec3(0);
    /**
     * -ve y for +/- x,z coz images are upsidedown
     *
     * +ve z backward
     * -ve z forward
     *
     * -ve z is upsidedown for +ve y
     * +ve z is upsidedown for -ve y
     */
    switch (face) {
    case 1:
      uvr = vec3(1.0f, -uv.y, uv.x);
      break;
    case 2:
      uvr = vec3(-1.0f, -uv.y, uv.x);
      break;
    case 3:
      uvr = vec3(uv.x, 1.0f, -uv.y);
      break;
    case 4:
      uvr = vec3(uv.x, -1.0f, uv.y);
      break;
    case 5:
      uvr = vec3(uv.x, -uv.y, 1.0f);
      break;
    case 6:
      uvr = vec3(uv.x, -uv.y, -1.0f);
      break;
    }
    fragColor = fs_in.color * textureLod(texCube, uvr, lod);
  }
}
