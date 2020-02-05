#pragma once

#define VERTEX_SHADER
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
} // namespace shader
enum class ShaderType { FORWARD_SHADER, FLAT_FORWARD_SHADER };
} // namespace render_system

#undef VERTEX_SHADER
