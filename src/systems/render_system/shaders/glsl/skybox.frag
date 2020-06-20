#version 460 core
#extension GL_GOOGLE_include_directive: require

#define SKYBOX_FRAGMENT_SHADER
#include "config.h"
#define PI 3.1415926538f

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
   vec3 v = normalize(texDir);
   vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
   uv *= vec2(0.159155f, 0.31830989f);
   uv += 0.5;
   vec3 color = texture(equirectangularMap, uv).xyz;
   // apply gamma correction + tonemapping
//   color = color / (color + vec3(1.0f));
//   color = pow(color, vec3(1.0f / 2.2f));
   fragColor = vec4(color, 1.0f);
#endif
}
