#pragma once

class Transform;
namespace render_system {

/**
 * Cached components of entites that are required by the render system.
 */
struct RenderableEntity {
  const Transform &transform;

  RenderableEntity(const Transform &transform) : transform(transform) {}
};
} // namespace render_system
