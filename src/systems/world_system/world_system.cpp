#include "world_system.h"

namespace world_system {

WorldSystem::WorldSystem() { worldObjects.reserve(5000); }

bool WorldSystem::deleteWorldObject(WorldObjectId id) {
  assert(id > 0 && id <= worldObjects.size());
  int objectId = id - 1;
  if (isWorldObject(id)) {
    nullIndices.insert(objectId);
    ecs::Coordinator::getInstance().destoryEntity(worldObjects[objectId]->entityId);
    worldObjects[objectId].reset(nullptr);
    return true;
  }
  return false;
}

void WorldSystem::clearWorld() {
  for (auto &worldObject : worldObjects) {
    if (worldObject != nullptr) deleteWorldObject(worldObject->getId());
  }
}

WorldSystem::~WorldSystem() {
  clearWorld();
  nullIndices.clear();
}
} // namespace world_system
