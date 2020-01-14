#include "entity_manager.h"

#include <cassert>
namespace ecs {
EntityManager::EntityManager() {
  for (Entity entity = 1; entity < MAX_ENTITES; ++entity)
    availableEntities.push(entity);
}

Entity EntityManager::createEntity() {
  assert(livingEntityCount < MAX_ENTITES && "Too many entities in existance.");
  Entity id = availableEntities.front();
  availableEntities.pop();
  ++livingEntityCount;
  return id;
}

void EntityManager::destoryEntity(Entity entity) {
  assert(entity < MAX_ENTITES && "Entity out of range" &&
         entity != INVALID_ENTITY && "Invalid entity.");
  signatures[entity].reset();
  availableEntities.push(entity);
  --livingEntityCount;
}

void EntityManager::setSignature(Entity entity, Signature signature) {
  assert(entity < MAX_ENTITES && "Entity out of range" &&
         entity != INVALID_ENTITY && "Invalid entity.");
  signatures[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity) const {
  assert(entity < MAX_ENTITES && "Entity out of range" &&
         entity != INVALID_ENTITY && "Invalid entity.");
  return signatures[entity];
}
} // namespace ecs
