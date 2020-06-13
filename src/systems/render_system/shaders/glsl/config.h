#ifndef GLSL_CONFIG_H
#define GLSL_CONFIG_H

// configuration file for vertex shaders
#ifdef FORWARD_VERTEX_SHADER
// attribute locations
#define VERT_A_POSITION_LOC 0
#define VERT_A_NORMAL_LOC 1
#define VERT_A_TEXTCOORD0_LOC 2
// uniform locations
#define VERT_U_TRANSFORMATION_LOC 0
// uniform buffer block indices
#endif

/**
 * Common - 0 start
 *
 * 0 - texCoord, 1 - worldPos, 3 - normal, 4 - camPos
 */
#if defined(FORWARD_VERTEX_SHADER) || defined(FORWARD_FRAGMENT_SHADER)
#define VERT_INTERFACE_BLOCK_LOC 0
#endif

#ifdef FORWARD_FRAGMENT_SHADER
#define COLOR_ATTACHMENT0 0
#define FRAG_U_MATERIAL_ALBEDO_LOC 10
#define FRAG_U_MATERIAL_METALLIC_LOC 11
#define FRAG_U_MATERIAL_ROUGHNESS_LOC 12
#define FRAG_U_MATERIAL_AO_LOC 13

#define MAX_POINT_LIGHTS 4
#define FRAG_U_POINT_LIGHT_SIZE 14
#define FRAG_U_POINT_LIGHT0_POS 15
#define FRAG_U_POINT_LIGHT0_COL 16
#define FRAG_U_POINT_LIGHT0_RAD 17
#define FRAG_U_POINT_LIGHT0_INT 18
#define FRAG_U_POINT_LIGHT1_POS 19
#define FRAG_U_POINT_LIGHT1_COL 20
#define FRAG_U_POINT_LIGHT1_RAD 21
#define FRAG_U_POINT_LIGHT1_INT 22
#define FRAG_U_POINT_LIGHT2_POS 23
#define FRAG_U_POINT_LIGHT2_COL 24
#define FRAG_U_POINT_LIGHT2_RAD 25
#define FRAG_U_POINT_LIGHT2_INT 26
#define FRAG_U_POINT_LIGHT3_POS 27
#define FRAG_U_POINT_LIGHT3_COL 28
#define FRAG_U_POINT_LIGHT3_RAD 29
#define FRAG_U_POINT_LIGHT3_INT 30
#endif

#ifdef SKYBOX_VERTEX_SHADER
// attribute locations
#define VERT_A_POSITION_LOC 0
// uniform locations
#define VERT_U_PROJECTION_LOC 0
#define VERT_U_VIEW_LOC 1
#endif

#ifdef SKYBOX_FRAGMENT_SHADER
#define COLOR_ATTACHMENT0 0
#define FRAG_U_TEXTURE_LOC 0
#endif

#if defined(SKYBOX_VERTEX_SHADER) || defined(SKYBOX_FRAGMENT_SHADER)
#define VERT_V_DIRECTION_LOC 1
#endif

#if defined(FORWARD_VERTEX_SHADER) || defined(SKYBOX_VERTEX_SHADER)
#define VERT_UB_GENERAL_LOC 0
#endif

#endif
