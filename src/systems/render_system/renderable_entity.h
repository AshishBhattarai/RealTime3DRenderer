#pragma once

#include "glm/glm.hpp"
#include "types.h"

namespace component {
class Transform;
}
namespace render_system {

/**
 * Cached components of entites that are required by the render system.
 */
struct RenderableEntity {
  const EntityId entityId;
  const glm::mat4 &transform;

  RenderableEntity(const glm::mat4 &transform, const EntityId &entityId)
      : entityId(entityId), transform(transform) {}
};
} // namespace render_system
