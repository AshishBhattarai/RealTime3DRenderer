#pragma once

namespace render_system {
namespace shader_config {
// attribute locations
constexpr int POSITION_LOC = 0;
constexpr int NORMAL_LOC = 1;
constexpr int TEXCOORD0_LOC = 2;
} // namespace shader_config

enum class ShaderType { FORWARD_SHADER, FLAT_FORWARD_SHADER };
} // namespace render_system
