#pragma once

#define FRAGMENT_SHADER
#define VERTEX_SHADER
#include "glsl/config.h"
#include "types.h"

namespace render_system {
namespace shader {
/**
 * Common vertex & fragment shader stuff
 */
namespace vertex {
constexpr uint generalUBOBinding = VERT_UB_GENERAL_LOC;
// attribute locations
namespace attribute {
constexpr int POSITION_LOC = VERT_A_POSITION_LOC;
constexpr int NORMAL_LOC = VERT_A_NORMAL_LOC;
constexpr int TEXCOORD0_LOC = VERT_A_TEXTCOORD0_LOC;
} // namespace attribute
} // namespace vertex
namespace fragment {}

#undef FRAGMENT_SHADER
#undef VERTEX_SHADER
#undef GLSL_CONFIG_H

/**
 * Renderer shaders
 */
#define FORWARD_VERTEX_SHADER
#define FORWARD_FRAGMENT_SHADER
#include "glsl/config.h"
// forward shader
namespace forward {
namespace vertex {
namespace uniform {
constexpr int GENERAL_UB_LOC = VERT_UB_GENERAL_LOC;
constexpr int TRANSFORMATION_LOC = VERT_U_TRANSFORMATION_LOC;
} // namespace uniform
} // namespace vertex
namespace fragment {
namespace uniform {
constexpr int PBR_ALBEDO_LOC = FRAG_U_MATERIAL_ALBEDO_LOC;
constexpr int PBR_METALLIC_LOC = FRAG_U_MATERIAL_METALLIC_LOC;
constexpr int PBR_ROUGHNESS_LOC = FRAG_U_MATERIAL_ROUGHNESS_LOC;
constexpr int PBR_AO_LOC = FRAG_U_MATERIAL_AO_LOC;
constexpr int POINT_LIGHT_SIZE_LOC = FRAG_U_POINT_LIGHT_SIZE;
constexpr int POINT_LIGHT_LOC[MAX_POINT_LIGHTS] = {
    FRAG_U_POINT_LIGHT0_POS, FRAG_U_POINT_LIGHT1_POS, FRAG_U_POINT_LIGHT2_POS,
    FRAG_U_POINT_LIGHT3_POS};
constexpr int PBR_IRRADIANCE_MAP_UNIT = FRAG_U_IRRADIANCE_MAP_BND;
constexpr uint PBR_PREFILETERED_MAP_UNIT = FRAG_U_PREFILTERED_MAP_BND;
constexpr uint PBR_BRDF_INTEGRATION_MAP_UNIT = FRAG_U_BRDF_INTEGRATION_MAP_BND;
} // namespace uniform
namespace PointLight {
constexpr uint MAX = MAX_POINT_LIGHTS;
constexpr uint POSITION = 0;
constexpr uint COLOR = 1;
constexpr uint RADIUS = 2;
constexpr uint INTENSITY = 3;
} // namespace PointLight
} // namespace fragment
} // namespace forward
#undef FORWARD_VERTEX_SHADER
#undef FORWARD_FRAGMENT_SHADER

#define SKYBOX_VERTEX_SHADER
#define SKYBOX_FRAGMENT_SHADER
#undef GLSL_CONFIG_H
#include "glsl/config.h"
// skybox shader
namespace skybox {
namespace vertex {}
namespace fragment {
constexpr int TEXTURE_UNIT = FRAG_U_ENVMAP_BND;
}
} // namespace skybox
#undef SKYBOX_VERTEX_SHADER
#undef SKYBOX_FRAGMENT_SHADER

/**
 * Post-processor shaders
 */
#define VISUAL_PREP_VERTEX_SHADER
#define VISUAL_PREP_FRAGMENT_SHADER
#undef GLSL_CONFIG_H
#include "glsl/config.h"
namespace visualprep {
namespace vertex {}
namespace fragment {
constexpr int EXPOSURE_LOC = FRAG_U_EXPOSURE_LOC;
constexpr int GAMMA_LOC = FRAG_U_GAMMA_LOC;
constexpr int TEXTURE_UNIT = FRAG_U_FRAME_TEXTURE_BND;
} // namespace fragment
} // namespace visualprep
#undef VISUAL_PREP_VERTEX_SHADER
#undef VISUAL_PREP_FRAGMENT_SHADER
#undef GLSL_CONFIG_H

/**
 * Pre-processor shaders
 */
#define CUBEMAP_VERTEX_SHADER
#define CUBEMAP_FRAGMENT_SHADER
#include "glsl/config.h"
// cubemap
namespace cubemap {
namespace vertex {
constexpr int PROJECTION_MAT_LOC = VERT_U_PROJECTION_MAT_LOC;
constexpr int VIEW_MAT_LOC = VERT_U_VIEW_MAT_LOC;
} // namespace vertex
namespace fragment {
constexpr int TEXTURE_UNIT = FRAG_U_ENVMAP_BND;
}
} // namespace cubemap
#undef CUBEMAP_FRAGMENT_SHADER
#undef GLSL_CONFIG_H

#define CUBEMAP_VERTEX_SHADER
#define IBL_SPECULAR_CONVOLUTION_FRAGMENT_SHADER
#include "glsl/config.h"
namespace iblSpecularConvolution {
namespace vertex {}
namespace fragment {
constexpr int ENV_MAP_UNIT = FRAG_U_ENVMAP_BND;
constexpr int ROUGHNESS_LOC = FRAG_U_ROUGHNESS_LOC;
} // namespace fragment
} // namespace iblSpecularConvolution
#undef CUBEMAP_VERTEX_SHADER
#undef IBL_SPECULAR_CONVOLUTION_FRAGMENT_SHADER
} // namespace shader
enum class ShaderType { FORWARD_SHADER, FLAT_FORWARD_SHADER };
} // namespace render_system
#undef FRAGMENT_SHADER
#undef VERTEX_SHADER
