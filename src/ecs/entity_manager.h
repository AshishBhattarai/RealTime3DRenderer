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
  EntityManager();
  Entity createEntity();
  void destoryEntity(Entity entity);
  void setSignature(Entity entity, Signature signature);
  Signature getSignature(Entity entity) const;

private:
  std::queue<Entity> availableEntities{};
  std::array<Signature, MAX_ENTITES> signatures{};
  u32 livingEntityCount = 0;
};
} // namespace ecs
