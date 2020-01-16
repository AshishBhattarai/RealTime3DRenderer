#include "entity_manager.h"
#include "third_party/catch.hpp"
#include <array>
#include <random>

TEST_CASE("EntityManager::createEntity test", "[ENTITY_MANAGER]") {
  ecs::EntityManager entityManager;
  auto entity = entityManager.createEntity();
  REQUIRE(entity != ecs::INVALID_ENTITY);
  REQUIRE(entity < ecs::MAX_ENTITES);
}

TEST_CASE("EntityManager::getLivingCount test", "[ENTITY_MANAGER]") {
  ecs::EntityManager entityManager;
  std::array<u32, ecs::MAX_ENTITES - 1> entites;
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    entites[i] = entityManager.createEntity();
    REQUIRE(entites[i] == i + 1);
    REQUIRE(entityManager.getLivingCount() == i + 1);
  }
  REQUIRE(entityManager.getLivingCount() == ecs::MAX_ENTITES - 1);
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    entityManager.destoryEntity(entites[i]);
  }
  REQUIRE(entityManager.getLivingCount() == 0);
}

TEST_CASE("EntityManager::getSignature test", "[ENTITY_MANAGER]") {
  ecs::EntityManager entityManager;
  auto entity = entityManager.createEntity();
  auto sig = entityManager.getSignature(entity);
  REQUIRE(sig == std::bitset<ecs::MAX_COMPONENTS>());
}

TEST_CASE("EntityManager::setSignature test", "[ENTITY_MANAGER]") {
  ecs::EntityManager entityManager;
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    auto e = entityManager.createEntity();
    entityManager.setSignature(
        e, std::bitset<ecs::MAX_COMPONENTS>().set(1 + (i % 31), true));
    REQUIRE(entityManager.getSignature(e) ==
            std::bitset<ecs::MAX_COMPONENTS>().set(1 + (i % 31), true));
  }
}

TEST_CASE("EntityManager::updateSignature test", "[ENTITY_MANAGER]") {
  ecs::EntityManager entityManager;
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    auto e = entityManager.createEntity();
    srand(time(0));
    auto num = rand();
    entityManager.updateSignaure(e, 1 + (num % 31), num % 2);
    entityManager.updateSignaure(e, 1 + (i % 31), num % 2 == 0);
    REQUIRE(entityManager.getSignature(e) ==
            std::bitset<ecs::MAX_COMPONENTS>()
                .set(1 + (num % 31), num % 2)
                .set(1 + (i % 31), num % 2 == 0));
  }
}

TEST_CASE("EntityManager::destoryEntity test", "[ENTITY_MANAGER]") {
  ecs::EntityManager entityManager;
  auto entity = entityManager.createEntity();
  entityManager.updateSignaure(entity, 6, true);
  entityManager.destoryEntity(entity);
  REQUIRE(entityManager.getSignature(entity) ==
          std::bitset<ecs::MAX_COMPONENTS>());
}
