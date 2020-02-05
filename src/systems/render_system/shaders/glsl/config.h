#ifndef GLSL_CONFIG_H
#define GLSL_CONFIG_H

// configuration file for vertex shaders
#ifdef VERTEX_SHADER
// attribute locations
#define VERT_A_POSITION_LOC 0
#define VERT_A_NORMAL_LOC 1
#define VERT_A_TEXTCOORD0_LOC 2
// uniform locations
#define VERT_U_PROJECTION_LOC 0
#define VERT_U_VIEW_LOC 1
#define VERT_U_TRANSFORMATION_LOC 2
#endif

#ifdef FRAGMENT_SHADER
#define COLOR_ATTACHMENT0 0
#endif

#endif
