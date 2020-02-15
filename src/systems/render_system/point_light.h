#pragma once
#include "types.h"
#include <glm/glm.hpp>

namespace render_system {

struct PointLight {
  EntityId entityId;
  const glm::vec4 &position;
  const glm::vec3 &color;
  const float &radius;

  PointLight(const glm::vec4 &position, const glm::vec3 &color,
             const float &radius, const EntityId &entityId)
      : entityId(entityId), position(position), color(color), radius(radius) {}
};

} // namespace render_system
