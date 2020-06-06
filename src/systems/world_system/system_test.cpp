#define CATCH_CONFIG_MAIN
#include "components/light.h"
#include "components/model.h"
#include "third_party/catch.hpp"
#include "world_system.h"

namespace system_test {

inline void initTest() {
  using namespace ecs;
  Coordinator &coordinator = Coordinator::getInstance();
  coordinator.registerComponent<component::Model>();
  coordinator.registerComponent<component::Transform>();
  coordinator.registerComponent<component::Light>();
}

TEST_CASE("Creating a new world object", "[WORLD_SYSTEM") {
  initTest();
  world_system::WorldSystem system;
  float i = 20.0;
  auto &worldObject = system.createWorldObject(component::Transform(
      glm::vec3(i + 2.0f, i, i), glm::vec3(glm::radians(i)), glm::vec3(i)));
  auto &transform = worldObject.getTransform();
  REQUIRE(transform.position() == glm::vec3(i + 2.0f, i, i));
  //  REQUIRE(transform.rotation() == glm::vec3(glm::radians(i))); Precision
  REQUIRE(glm::ceil(transform.scale()) == glm::vec3(i, i, i));
  REQUIRE(system.numValidWorldObjects() == 1);
  system.deleteWorldObject(worldObject.getId());
  REQUIRE(system.numValidWorldObjects() == 0);
}

TEST_CASE("Creating multiple world objects", "[WORLD_SYSTEM") {
  world_system::WorldSystem system;
  std::vector<world_system::WorldObject *> objects;
  uint size = 2000;
  for (uint i = 0; i < size; ++i) {
    objects.emplace_back(&system.createWorldObject(component::Transform(
        glm::vec3(i + 0.0f, i + 0.0f, i + 0.0f),
        glm::vec3(glm::radians(i + 1.0f), glm::radians(i + 2.0f),
                  glm::radians(i + 3.0f)),
        glm::vec3(i))));
  }
  REQUIRE(system.numValidWorldObjects() == size);
  for (uint i = 0; i < size; ++i) {
    auto &worldObject = objects[i];
    auto transform = worldObject->getTransform();
    auto position =
        worldObject->getComponent<component::Transform>().position();
    auto scale = transform.scale();
    if (glm::vec3(i, i, i) != transform.position()) {
      REQUIRE(false);
    }
    REQUIRE(transform.position() == glm::vec3(i, i, i));
  }
}
} // namespace system_test
