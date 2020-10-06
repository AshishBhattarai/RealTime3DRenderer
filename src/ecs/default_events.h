#pragma once

#include "ecs/common.h"
#include "event_manager.h"

namespace event {
/**
 * This event is fired when an entity is added, changed or about to be deleted.
 *
 * DELETE - means the entity is about to be deleted, so its valid entity in event callback.
 */
struct EntityChanged : ecs::Event<EntityChanged> {
  enum class Status { CREATED, UPDATED, DELETED };

  EntityId entity;
  ecs::Signature signature;
  Status status;

  EntityChanged(EntityId entity, ecs::Signature sig, Status status)
      : entity(entity), signature(sig), status(status) {}
};
} // namespace event
