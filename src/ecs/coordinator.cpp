#include "coordinator.h"

namespace ecs {

Entity Coordinator::createEntity() { return entityManager.createEntity(); }
void Coordinator::destoryEntity(Entity entity) {
  Signature entitySignature = entityManager.getSignature(entity);
  entityManager.destoryEntity(entity);
  componentManager.entityDestoryed(entity, entitySignature);
  systemManager.entityDestoryed(entity);
}

} // namespace ecs
