#pragma once

#define FORWARD_VERTEX_SHADER
#define FORWARD_FRAGMENT_SHADER
#include "glsl/config.h"
#include "types.h"

namespace render_system {
namespace shader {
// attribute locations
namespace vertex {
constexpr uint generalUBOBinding = 0;
namespace attribute {
constexpr int POSITION_LOC = VERT_A_POSITION_LOC;
constexpr int NORMAL_LOC = VERT_A_NORMAL_LOC;
constexpr int TEXCOORD0_LOC = VERT_A_TEXTCOORD0_LOC;
} // namespace attribute
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
} // namespace uniform
namespace PointLight {
constexpr uint MAX = MAX_POINT_LIGHTS;
constexpr uint POSITION = 0;
constexpr uint COLOR = 1;
constexpr uint RADIUS = 2;
constexpr uint INTENSITY = 3;
} // namespace PointLight
} // namespace fragment
#undef FORWARD_VERTEX_SHADER
#undef FORWARD_FRAGMENT_SHADER

#define SKYBOX_VERTEX_SHADER
#define SKYBOX_FRAGMENT_SHADER
#undef GLSL_CONFIG_H
#include "glsl/config.h"
// skybox shader
namespace skybox {
namespace vertex {
constexpr int POSITION_LOC = VERT_A_POSITION_LOC;
}
namespace fragment {
constexpr int TEXTURE_LOC = FRAG_U_TEXTURE_LOC;
}
} // namespace skybox
#undef SKYBOX_VERTEX_SHADER
#undef SKYBOX_FRAGMENT_SHADER
} // namespace shader
enum class ShaderType { FORWARD_SHADER, FLAT_FORWARD_SHADER };
} // namespace render_system
