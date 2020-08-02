#version 460 core
#extension GL_GOOGLE_include_directive: require

#define CUBEMAP_FRAGMENT_SHADER
#include "config.h"
#define PI 3.1415926538f

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;
layout(location = VERT_V_DIRECTION_LOC) in vec3 texDir;

#ifdef EQUIRECTANGULAR
layout(binding = FRAG_U_ENVMAP_BND) uniform sampler2D equirectangularMap;
#else
layout(binding = FRAG_U_ENVMAP_BND) uniform samplerCube cubeMap;
#endif

void main(void)
{
   vec3 v = normalize(texDir);
#ifdef EQUIRECTANGULAR
  // dir to sphere uv
   vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
   uv *= vec2(0.159155f, 0.318309f);
   uv += 0.5;
   fragColor = vec4(texture(equirectangularMap, uv).xyz, 1.0f);
#else
   fragColor = vec4(texture(cubeMap, v).xyz, 1.0f);
#endif
}
