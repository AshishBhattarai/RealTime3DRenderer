#version 460 core
#extension GL_GOOGLE_include_directive: require

#define SKYBOX_FRAGMENT_SHADER
#include "config.h"
#define PI 3.1415926538f

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout(location = VERT_V_DIRECTION_LOC) in vec3 texDir;

#ifdef CUBEMAP_SKYBOX
layout(location = FRAG_U_ENVMAP_LOC) uniform samplerCube cubeMap;
#else
layout(location = FRAG_U_ENVMAP_LOC) uniform sampler2D equirectangularMap;
#endif

void main(void)
{
   vec3 v = normalize(texDir);
#ifdef CUBEMAP_SKYBOX
   fragColor = vec4(texture(cubeMap, v).xyz, 1.0f);
#else
   // dir to sphere uv
   vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
   uv *= vec2(0.159155f, 0.318309f);
   uv += 0.5;
   fragColor = vec4(texture(equirectangularMap, uv).xyz, 1.0f);
#endif
}
