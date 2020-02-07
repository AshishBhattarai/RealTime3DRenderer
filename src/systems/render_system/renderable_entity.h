#pragma once

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
  const component::Transform &transform;

  RenderableEntity(const component::Transform &transform,
                   const EntityId &entityId)
      : entityId(entityId), transform(transform) {}
};
} // namespace render_system
