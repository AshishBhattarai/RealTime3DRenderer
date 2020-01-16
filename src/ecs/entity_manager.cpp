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
  entity = INVALID_ENTITY;
}

void EntityManager::setSignature(Entity entity, Signature signature) {
  assert(entity < MAX_ENTITES && "Entity out of range" &&
         entity != INVALID_ENTITY && "Invalid entity.");
  signatures[entity] = signature;
}

Signature EntityManager::updateSignaure(Entity entity, ComponentFamily family,
                                        bool enable) {
  assert(entity < MAX_ENTITES && "Entity out of range" &&
         entity != INVALID_ENTITY && "Invalid entity." &&
         family != INVALID_COMPONENT_FAMILY &&
         "Component family must be valid." && family < MAX_COMPONENTS &&
         "Component family out of range.");
  return signatures[entity].set(family, enable);
}

Signature EntityManager::getSignature(Entity entity) const {
  assert(entity < MAX_ENTITES && "Entity out of range" &&
         entity != INVALID_ENTITY && "Invalid entity.");
  return signatures[entity];
}
} // namespace ecs
