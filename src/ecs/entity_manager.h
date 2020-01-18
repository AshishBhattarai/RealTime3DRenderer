#pragma once

#include "common.h"
#include <array>
#include <queue>

namespace ecs {

/**
 * @brief The EntityManager class
 * Used to create entities and track their components
 */
class EntityManager : NonCopyable {
public:
  static EntityManager &getInstace() {
    static EntityManager instace;
    return instace;
  }
  Entity createEntity();
  void destoryEntity(Entity entity);
  void setSignature(Entity entity, const Signature &signature);
  Signature updateSignaure(Entity entity, ComponentFamily family, bool enable);
  Signature getSignature(Entity entity) const;
  Entity getLivingCount() const { return livingEntityCount - 1; }

private:
  EntityManager();
  ~EntityManager() = default;
  std::queue<Entity> availableEntities{};
  std::array<Signature, MAX_ENTITES> signatures{};
  Entity livingEntityCount = 1; // since 0 is invalid entity
};
} // namespace ecs
