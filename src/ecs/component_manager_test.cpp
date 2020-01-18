#include "component_manager.h"
#include "entity_manager.h"
#include "third_party/catch.hpp"
#include <string>

namespace component_manager_test {

ecs::ComponentManager &componentManager = ecs::ComponentManager::getInstace();
ecs::EntityManager &entityManager = ecs::EntityManager::getInstace();

struct Dummy {
  int a;
  float b;
  double c;
  u64 d;

  Dummy(int a, float b, double c, u64 d) : a(a), b(b), c(c), d(d) {}

  friend bool operator==(const Dummy &lhs, const Dummy &rhs) {
    return lhs.a == rhs.a && lhs.b == rhs.b && lhs.c == rhs.c && lhs.d == rhs.d;
  }
};

struct Position {
  float x;
  float y;
  float z;

  Position(float x, float y, float z) : x(x), y(y), z(z) {}

  friend inline bool operator==(const Position &lhs, const Position &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }
};

TEST_CASE("ComponentManager register component test", "COMPONENT_MANAGER") {
  componentManager.registerComponent<Dummy>();
  componentManager.registerComponent<Position>();
  componentManager.registerComponent<int>();
  componentManager.registerComponent<float>();
  componentManager.registerComponent<double>();
  componentManager.registerComponent<u8>();
  componentManager.registerComponent<u16>();
  componentManager.registerComponent<u32>();
  componentManager.registerComponent<u64>();
  componentManager.registerComponent<char>();
  componentManager.registerComponent<std::string>();
  REQUIRE(componentManager.getComponentFamily<Dummy>() == 1);
  REQUIRE(componentManager.getComponentFamily<Position>() == 2);
  REQUIRE(componentManager.getComponentFamily<int>() == 3);
  REQUIRE(componentManager.getComponentFamily<float>() == 4);
  REQUIRE(componentManager.getComponentFamily<double>() == 5);
  REQUIRE(componentManager.getComponentFamily<u8>() == 6);
  REQUIRE(componentManager.getComponentFamily<u16>() == 7);
  REQUIRE(componentManager.getComponentFamily<u32>() == 8);
  REQUIRE(componentManager.getComponentFamily<u64>() == 9);
  REQUIRE(componentManager.getComponentFamily<char>() == 10);
  REQUIRE(componentManager.getComponentFamily<std::string>() == 11);
}

TEST_CASE("ComponentManager add component for entity with incorrect signature.",
          "COMPONENT_MANAGER") {
  ecs::Entity entity = entityManager.createEntity();
  Dummy dummy(0, 1, 2, 3);
  componentManager.addComponent<Dummy>(entity, Dummy(0, 1, 2, 3));
  REQUIRE(componentManager.getComponentCount<Dummy>() == 1);
  REQUIRE(dummy == componentManager.getComponent<Dummy>(entity));
  componentManager.removeComponent<Dummy>(entity);
  //  componentManager.getComponent<Dummy>(entity); // assertion error
  componentManager.addComponent<Dummy>(entity, 0, 1, 2, 3);
  REQUIRE(dummy == componentManager.getComponent<Dummy>(entity));
  entityManager.destoryEntity(entity);
  componentManager.entityDestoryed(
      entity, std::bitset<ecs::MAX_COMPONENTS>().set(1, true));
  REQUIRE(componentManager.getComponentCount<Dummy>() == 0);
}

} // namespace component_manager_test
