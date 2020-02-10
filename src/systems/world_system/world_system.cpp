#include "world_system.h"

namespace world_system {

WorldSystem::WorldSystem() { worldObjects.reserve(1000); }

bool WorldSystem::deleteWorldObject(WorldObjectId id) {
  assert(id > 0 && id <= worldObjects.size());
  int objectId = id - 1;
  if (isWorldObject(objectId)) {
    nullIndices.insert(objectId);
    ecs::Coordinator::getInstance().destoryEntity(
        worldObjects[objectId]->entityId);
    worldObjects[objectId].reset(nullptr);
    return true;
  }
  return false;
}

WorldSystem::~WorldSystem() {
  worldObjects.clear();
  nullIndices.clear();
}
} // namespace world_system
