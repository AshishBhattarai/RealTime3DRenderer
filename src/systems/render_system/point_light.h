#pragma once
#include "types.h"
#include <glm/glm.hpp>

namespace render_system {

struct PointLight {
  const EntityId entityId;
  const glm::vec3 position;
  const glm::vec3 color;
  const float radius;
  const float intensity;
};

} // namespace render_system
