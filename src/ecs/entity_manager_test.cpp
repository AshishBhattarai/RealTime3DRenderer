#include "entity_manager.h"
#include "third_party/catch.hpp"
#include <array>
#include <random>

namespace entity_manager_test {

ecs::EntityManager &entityManager = ecs::EntityManager::getInstace();

TEST_CASE("EntityManager test with setSignature", "[ENTITY_MANAGER]") {
  ecs::Entity entites[ecs::MAX_ENTITES];
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    auto e = entityManager.createEntity();
    entites[i] = e;
    entityManager.setSignature(e,
                               std::bitset<ecs::MAX_COMPONENTS>().set(
                                   1 + (i % (ecs::MAX_COMPONENTS - 1)), true));
    REQUIRE(entityManager.getSignature(e) ==
            std::bitset<ecs::MAX_COMPONENTS>().set(
                1 + (i % (ecs::MAX_COMPONENTS - 1)), true));
    REQUIRE(entityManager.getLivingCount() == i + 1);
    REQUIRE(entites[i] == i + 1);
  }
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    entityManager.destoryEntity(entites[i]);
  }
  REQUIRE(entityManager.getLivingCount() == 0);
}

TEST_CASE("EntityManager test with update signature", "[ENTITY_MANAGER]") {
  ecs::Entity entites[ecs::MAX_ENTITES];
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    auto e = entityManager.createEntity();
    entites[i] = e;
    srand(time(0));
    auto num = rand();
    entityManager.updateSignaure(e, 1 + (num % (ecs::MAX_COMPONENTS - 1)),
                                 num % 2);
    entityManager.updateSignaure(e, 1 + (i % (ecs::MAX_COMPONENTS - 1)),
                                 num % 2 == 0);
    REQUIRE(entityManager.getSignature(e) ==
            std::bitset<ecs::MAX_COMPONENTS>()
                .set(1 + (num % (ecs::MAX_COMPONENTS - 1)), num % 2)
                .set(1 + (i % (ecs::MAX_COMPONENTS - 1)), num % 2 == 0));
  }
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    entityManager.destoryEntity(entites[i]);
  }
  REQUIRE(entityManager.getLivingCount() == 0);
}

TEST_CASE("Valid entity ids", "[ENTITY_MANAGER") {
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    REQUIRE(entityManager.createEntity() == i + 1);
  }
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    entityManager.destoryEntity(i + 1);
  }
}
} // namespace entity_manager_test
