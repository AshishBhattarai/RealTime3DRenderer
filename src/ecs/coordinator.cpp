#include "coordinator.h"

namespace ecs {

Entity Coordinator::createEntity() { return entityManager->createEntity(); }

void Coordinator::destoryEntity(Entity entity) {
  entityManager->destoryEntity(entity);
  componentManager->entityDestoryed(entity,
                                    entityManager->getSignature(entity));
  systemManager->entityDestoryed(entity);
}

} // namespace ecs
