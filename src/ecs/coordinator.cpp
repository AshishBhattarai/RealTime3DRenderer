#include "coordinator.h"
#include "ecs/default_events.h"

namespace ecs {

Entity Coordinator::createEntity() {
  Entity entity = entityManager.createEntity();
  Signature sig = entityManager.getSignature(entity);
  eventManager.emit<event::EntityChanged>(entity, sig, event::EntityChanged::Status::CREATED);
  return entity;
}
void Coordinator::destoryEntity(Entity entity) {
  Signature sig = entityManager.getSignature(entity);
  eventManager.emit<event::EntityChanged>(entity, sig, event::EntityChanged::Status::DELETED);
  entityManager.destoryEntity(entity);
  componentManager.entityDestoryed(entity, sig);
  systemManager.entityDestoryed(entity);
}

} // namespace ecs
