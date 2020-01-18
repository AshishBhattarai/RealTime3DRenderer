#include "component_manager.h"
#include "entity_manager.h"
#include "system_manager.h"
#include "third_party/catch.hpp"
#include <iostream>

namespace system_manager_test {

ecs::SystemManager &systemManager = ecs::SystemManager::getInstance();
ecs::ComponentManager &componentManager = ecs::ComponentManager::getInstace();
ecs::EntityManager &entityManger = ecs::EntityManager::getInstace();

struct Position {
  float x;
  float y;
  float z;

  Position(float x, float y, float z) : x(x), y(y), z(z) {}

  friend inline bool operator==(const Position &lhs, const Position &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }
};

class TestSystem : ecs::System<TestSystem> {
public:
  TestSystem() {
    connectEntityAddedSignal(
        [](const ecs::Entity &entity, const ecs::Signature &signature) {
          std::cout << "TestSyteam: Entity added with id: " << entity
                    << " and signature: " << signature << std::endl;
        });
    connectEntityRemovedSignal([](const ecs::Entity &entity) {
      std::cout << "TestSystem: Entity removed id: " << entity << std::endl;
    });
  }
};

class TestSystem2 : ecs::System<TestSystem2> {
public:
  TestSystem2() {
    connectEntityAddedSignal(
        [](const ecs::Entity &entity, const ecs::Signature &signature) {
          std::cout << "TestSystem2: Entity added with id: " << entity
                    << " and signature: " << signature << std::endl;
        });
    connectEntityRemovedSignal([](const ecs::Entity &entity) {
      std::cout << "TestSystem2: Entit removed id: " << entity << std::endl;
    });
  }
};

TEST_CASE("SystemManager system creation test", "[SYSTEM_MANAGER]") {
  //  TestSystem(); // assert failur
  ecs::ComponentFamily componentFamily =
      componentManager.registerComponent<Position>();
  systemManager.registerSystem<TestSystem>(
      std::bitset<ecs::MAX_COMPONENTS>().set(componentFamily, true));
  systemManager.registerSystem<TestSystem2>(
      std::bitset<ecs::MAX_COMPONENTS>().set(20, true));
  REQUIRE(systemManager.systemCount() == 2);
}

TEST_CASE("SystemManager entity added & removed test with matching signature",
          "[SYSTEM_MANAGER]") {
  TestSystem testSystem;
  ecs::Entity entity = entityManger.createEntity();
  entityManger.setSignature(
      entity, std::bitset<ecs::MAX_COMPONENTS>().set(
                  componentManager.getComponentFamily<Position>(), true));
  systemManager.entitySignatureChanged(entity,
                                       entityManger.getSignature(entity));
  entityManger.destoryEntity(entity);
  systemManager.entityDestoryed(entity);
}

TEST_CASE(
    "SystemManager entity added & removed test with no matching signature",
    "[SYSTEM_MANAGER]") {
  TestSystem2 testSystem2;
  ecs::Entity entity = entityManger.createEntity();
  std::cout << "Entity added: " << entity << std::endl;
  entityManger.setSignature(entity,
                            std::bitset<ecs::MAX_COMPONENTS>().set(16, true));
  systemManager.entitySignatureChanged(entity,
                                       entityManger.getSignature(entity));
  entityManger.destoryEntity(entity);
  systemManager.entityDestoryed(entity);
}
} // namespace system_manager_test
