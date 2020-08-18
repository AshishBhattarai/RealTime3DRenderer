#version 460 core
#extension GL_GOOGLE_include_directive: require

#define FORWARD_VERTEX_SHADER
#include "config.h"

layout(location = VERT_A_POSITION_LOC) in vec3 position;
layout(location = VERT_A_NORMAL_LOC) in vec3 normal;
#ifdef TEXTURE_MATERIAL
layout(location = VERT_A_TEXTCOORD0_LOC) in vec2 texCoord;
#endif 

layout (location = VERT_INTERFACE_BLOCK_LOC) out VertexData {
#ifdef TEXTURE_MATERIAL
   vec2 texCoord;
#endif
   vec3 worldPos;
   vec3 normal;
   vec3 camPos;
} vs_out;


layout(std140, binding = VERT_UB_GENERAL_LOC) uniform GeneralUB {
    mat4 projection;
    mat4 view;
    vec4 cameraPosition;
};

layout(location = VERT_U_TRANSFORMATION_LOC) uniform mat4 transformation;

void main() {
    vec4 worldPos = transformation * vec4(position, 1.0f);
    gl_Position = projection * view * worldPos;
#ifdef TEXTURE_MATERIAL
    vs_out.texCoord = texCoord;
#endif
    vs_out.worldPos = worldPos.xyz;
    vs_out.normal = mat3(transpose(inverse(transformation))) * normal;
    vs_out.camPos = cameraPosition.xyz;
}
