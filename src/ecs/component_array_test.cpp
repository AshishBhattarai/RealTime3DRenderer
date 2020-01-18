#include "component_array.h"
#include "entity_manager.h"
#include "third_party/catch.hpp"
#include <random>

namespace component_array_test {

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

TEST_CASE("ComponentArray CURD test", "[COMPONENT_ARRAY]") {
  ecs::ComponentArray<Dummy> componentArray;
  ecs::EntityManager &entityManager = ecs::EntityManager::getInstace();
  ecs::Entity entites[ecs::MAX_ENTITES];

  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    srand(time(0));
    auto e = entityManager.createEntity();
    entites[i] = e;
    auto num1 = rand();
    auto num2 = rand() / 20;
    auto num3 = rand() / 200;
    auto num4 = rand() + rand();
    auto dataA = Dummy(num1, num2, num3, num4);
    auto dataB = Dummy(num4, num3, num2, num1);
    // Insert
    componentArray.insertData(e, dataA);
    REQUIRE(dataA == componentArray.getData(e));
    // Update
    componentArray.getData(e) = dataB;
    REQUIRE(dataB == componentArray.getData(e));
    REQUIRE(componentArray.getSize() == i + 1);
  }

  // delete
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    componentArray.removeData(entites[i]);
    REQUIRE(componentArray.getSize() == ecs::MAX_ENTITES - 2 - i);
  }

  // remove entites
  for (u32 i = 0; i < ecs::MAX_ENTITES - 1; ++i) {
    entityManager.destoryEntity(entites[i]);
  }
  REQUIRE(entityManager.getLivingCount() == 0);
}
} // namespace component_array_test
