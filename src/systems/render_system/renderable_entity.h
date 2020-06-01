#pragma once

#include "common.h"
#include "glm/glm.hpp"
#include "types.h"
#include <map>

namespace component {
class Transform;
}
namespace render_system {

/**
 * Cached components of entites that are required by the render system.
 */
struct RenderableEntity {
  const EntityId entityId;
  const std::map<PrimitiveId, MaterialId> primIdToMatId;
  const glm::mat4 *transform;
};
} // namespace render_system
