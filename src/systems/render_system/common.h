#pragma once

#include "types.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace render_system {
using MeshId = uint;

constexpr MeshId MESH_ID_OFFSET = 1000;
constexpr MeshId INVALID_MESH_ID = 0;

} // namespace render_system
