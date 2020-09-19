#version 460 core
#extension GL_GOOGLE_include_directive : require
#define GRID_PLANE_FRAGMENT_SHADER
#include "config.h"

layout(location = COLOR_ATTACHMENT0) out vec4 fragColor;

layout(location = VERT_INTERFACE_BLOCK_LOC) in VS_OUT {
  vec3 worldPos;
  vec3 camPos;
  float scale;
}
fs_in;

layout(location = FRAG_U_PLANE_COLOR_LOC) uniform vec3 planeColor;
layout(location = FRAG_U_GRID_COLOR_LOC) uniform vec3 gridColor;
layout(location = FRAG_U_GRID_MODE_LOC) uniform int gridFlags;
layout(location = FRAG_U_DISTANCE_LIMIT_LOC) uniform int distanceLimit;

void main(void) {
  bool enableDistance = bool(gridFlags & 0x1);
  bool enableDiscard = bool(gridFlags & 0x2);
  float camDistance = distance(fs_in.camPos, fs_in.worldPos);
  vec2 worldCoord = fs_in.worldPos.xz;
  int distx = int(length(vec2(0, worldCoord.x)));
  int disty = int(length(vec2(worldCoord.y, 0)));
  vec2 frac = fract(worldCoord);
  if (enableDistance && (camDistance > distanceLimit))
    discard;
  else if (worldCoord.y >= -0.1f && worldCoord.y <= 0.1f)
    fragColor = vec4(0.7f, 0.0f, 0.0f, 1.0f); // x axis red color
  else if (worldCoord.x >= -0.1f && worldCoord.x <= 0.1f)
    fragColor = vec4(0.0f, 0.7f, 0.0f, 1.0f); // z axis green color
  else {
    float grad =
        (distx % 10 == 0 || disty % 10 == 0) ? 1.0f : (camDistance > distanceLimit) ? 0.0f : 0.1f;
    vec2 mult = smoothstep(0.0, grad, frac) - smoothstep(1.0 - grad, 1.0, frac);
    vec3 col = mix(gridColor, planeColor, mult.x * mult.y);
    fragColor = vec4(col, 1.0f);
  }
  if ((fragColor.x > 0.15 && fragColor.y > 0.15) && enableDiscard) discard;
}
