#pragma once
#include "types.h"
#include <glm/glm.hpp>

namespace render_system {

struct PointLight {
  EntityId entityId;
  const glm::vec4 &position;
  const glm::vec3 &color;
  const float &radius;
  const float &intensity;

  PointLight(const glm::vec4 &position, const glm::vec3 &color,
             const float &radius, const float &intensity,
             const EntityId &entityId)
      : entityId(entityId), position(position), color(color), radius(radius),
        intensity(intensity) {}
};

} // namespace render_system
