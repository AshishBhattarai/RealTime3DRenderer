#pragma once

#include "components/transform.h"
#include "ecs/coordinator.h"
#include "types.h"
#include <cstdlib>
#include <third_party/simplesignal.h>

namespace ecs {
class Coordinator;
}
namespace world_system {
typedef size_t WorldObjectId;
static constexpr WorldObjectId INVALID_WORD_OBJECT_ID = 0;

/**
 * @brief The WorldObject class
 *
 * This class represents an object in the world space
 */
class WorldObject {
private:
  using OnUpdateSignal = Simple::Signal<void(float dt)>;
  friend class WorldSystem;

  WorldObjectId id;
  EntityId entityId;
  //  std::vector<WorldObject*> childerns;

public:
  std::unique_ptr<OnUpdateSignal> onUpdate;

  WorldObject();
  virtual ~WorldObject();

  component::Transform &getTransform() {
    return getComponent<component::Transform>();
  }
  WorldObjectId getId() const { return id; }
  EntityId getEntityId() const { return entityId; }

  template <typename T> void addComponent(const T &component) {
    assert(id && entityId &&
           "Must be constructed with WorldSystem before use.");
    ecs::Coordinator::getInstance().addComponent<T>(entityId, component);
  }

  template <typename T, typename... Args> void addComponent(Args &&... args) {
    assert(id && entityId &&
           "Must be constructed with WorldSystem before use.");
    ecs::Coordinator::getInstance().addComponent<T>(
        entityId, std::forward<Args>(args)...);
  }

  // Might not need this
  //  template <typename T> void removeComponent() {
  //    coordinator.removeComponent<T>(entityId);
  //  }

  template <typename T> T &getComponent() {
    assert(id && entityId &&
           "Must be constructed with WorldSystem before use.");
    return ecs::Coordinator::getInstance().getComponent<T>(entityId);
  }
};

} // namespace world_system
