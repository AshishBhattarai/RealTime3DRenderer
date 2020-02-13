#pragma once

#define VERTEX_SHADER
#define FRAGMENT_SHADER
#include "glsl/config.h"

namespace render_system {
namespace shader {
// attribute locations
namespace vertex {
namespace attribute {
constexpr int POSITION_LOC = VERT_A_POSITION_LOC;
constexpr int NORMAL_LOC = VERT_A_NORMAL_LOC;
constexpr int TEXCOORD0_LOC = VERT_A_TEXTCOORD0_LOC;
} // namespace attribute
namespace uniform {
constexpr int PROJECTION_LOC = VERT_U_PROJECTION_LOC;
constexpr int VIEW_LOC = VERT_U_VIEW_LOC;
constexpr int TRANSFORMATION_LOC = VERT_U_TRANSFORMATION_LOC;
} // namespace uniform
} // namespace vertex

namespace fragment {
namespace uniform {
constexpr int PBR_ALBEDO_LOC = FRAG_U_MATERIAL_ALBEDO_LOC;
constexpr int PBR_METALLIC_LOC = FRAG_U_MATERIAL_METALLIC_LOC;
constexpr int PBR_ROUGHNESS_LOC = FRAG_U_MATERIAL_ROUGHNESS_LOC;
constexpr int PBR_AO_LOC = FRAG_U_MATERIAL_AO_LOC;
constexpr int POINT_LIGHTS_LOC[4] = {
    FRAG_U_MATERIAL_POINT_LIGHT0_POS, FRAG_U_MATERIAL_POINT_LIGHT1_POS,
    FRAG_U_MATERIAL_POINT_LIGHT2_POS, FRAG_U_MATERIAL_POINT_LIGHT3_POS};
} // namespace uniform
namespace PointLight {
constexpr int POSITION = 0;
constexpr int COLOR = 1;
constexpr int RADIUS = 2;
} // namespace PointLight
} // namespace fragment
} // namespace shader
enum class ShaderType { FORWARD_SHADER, FLAT_FORWARD_SHADER };
} // namespace render_system

#undef VERTEX_SHADER
#undef FRAGMENT_SHADER
