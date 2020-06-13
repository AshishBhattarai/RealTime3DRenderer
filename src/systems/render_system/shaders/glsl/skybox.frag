#version 460 core
#extension GL_GOOGLE_include_directive: require

#define SKYBOX_FRAGMENT_SHADER
#include "config.h"

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout(location = VERT_V_DIRECTION_LOC) in vec3 texDir;

#ifdef CUBEMAP_SKYBOX
layout(location = FRAG_U_TEXTURE_LOC) uniform samplerCube cubeMap;
#else
layout(location = FRAG_U_TEXTURE_LOC) uniform sampler2D equirectangularMap;
#endif

void main(void)
{
#ifdef CUBEMAP_SKYBOX
    fragColor = texture(cubeMap, texDir);
#else
   // dir to sphere uv
   vec2 uv = vec2(atan(texDir.x, texDir.y), asin(texDir.y));
   uv *= vec2(0.1591, 0.3183);
   uv += 0.5;
   fragColor = texture(equirectangularMap, uv);
#endif
}
