#pragma once
#include "ecs/coordinator.h"
#include "world_object.h"
#include <memory>
#include <unordered_set>
#include <vector>

namespace world_system {
/**
 * @brief The WorldSystem class
 * This system is not concerned with components. (Hence it doesn't inherit
 * System<T>) It is used to create entites with Transform that exists in the
 * world/scene.
 */
class WorldSystem {
private:
  std::unordered_set<size_t>
      nullIndices; // indices with null data on worldObjects;
  std::vector<std::unique_ptr<WorldObject>> worldObjects;

public:
  WorldSystem();
  ~WorldSystem();

  /**
   * Constructs a new WorldObject&Entity with given transform component.
   *
   * id - 1, since id starts from 1 and array index from 0
   */
  template <typename T = WorldObject, typename... Args>
  T &createWorldObject(const component::Transform &transform, Args &&... args) {
    std::unique_ptr<WorldObject> worldObject =
        std::make_unique<T>(std::forward<Args>(args)...);
    size_t worldId = worldObjects.size();
    if (!nullIndices.empty()) {
      auto it = nullIndices.begin();
      worldId = *it;
      nullIndices.erase(it);
    }
    ecs::Coordinator &coordinator = ecs::Coordinator::getInstance();
    worldObject->id = worldId + 1;
    worldObject->entityId = coordinator.createEntity();
    coordinator.addComponent<component::Transform>(worldObject->entityId,
                                                   transform);
    worldObject->transform =
        &coordinator.getComponent<component::Transform>(worldObject->entityId);
    if (worldId == worldObjects.size()) {
      worldObjects.emplace_back(std::move(worldObject));
    } else {
      worldObjects[worldId] = std::move(worldObject);
    }
    return *worldObjects[worldId];
  }

  /**
   * @brief isWorldObject
   * @param id
   * @return true if id belongs to a valid worldObject.
   */
  bool isWorldObject(WorldObjectId id) const {
    assert(id > 0 && id <= worldObjects.size());
    return nullIndices.find(id - 1) != nullIndices.end();
  }

  /**
   * @brief getWorldObject
   * @param id
   * @return returns reference to the worldObject.
   */
  template <typename T = WorldObject>
  T &getWorldObject(WorldObjectId id) const {
    assert(id > 0 && id <= worldObjects.size());
    return static_cast<T &>(*worldObjects[id - 1]);
  }

  /**
   * @brief deleteWorldObject
   * @param id
   * @return ture if remove worldObject successful.
   */
  bool deleteWorldObject(WorldObjectId id);

  /**
   * @brief update
   * @param dt
   *
   * calls WorldObject.onUpdate(dt)
   */
  void update(float dt) {
    for (const auto &worldObject : worldObjects) {
      if (worldObject)
        worldObject->onUpdate->emit(dt);
    }
  }
};

} // namespace world_system
