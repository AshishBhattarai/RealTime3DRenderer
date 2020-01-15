#include "coordinator.h"

namespace ecs {

Coordinator::Coordinator() {
  entityManager = std::make_unique<EntityManager>();
  componentManager = std::make_unique<ComponentManager>();
  systemManager = std::make_unique<SystemManager>();
}

Entity Coordinator::createEntity() { return entityManager->createEntity(); }

void Coordinator::destoryEntity(Entity entity) {
  entityManager->destoryEntity(entity);
  componentManager->entityDestoryed(entity,
                                    entityManager->getSignature(entity));
  systemManager->entityDestoryed(entity);
}

} // namespace ecs
