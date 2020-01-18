#include "coordinator.h"
#include "third_party/catch.hpp"
#include <iostream>

namespace coordinator_test {

ecs::Coordinator &coordiantor = ecs::Coordinator::getInstance();

struct Position {
  float x;
  float y;
  float z;

  Position(float x, float y, float z) : x(x), y(y), z(z) {}

  friend inline bool operator==(const Position &lhs, const Position &rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }
};

class TestEvent : public ecs::Event<TestEvent> {};

class MovementSystem : public ecs::System<MovementSystem>,
                       public ecs::Receiver<TestEvent> {
public:
  MovementSystem() {
    connectEntityAddedSignal(
        [](const ecs::Entity &entity, const ecs::Signature &signature) {
          std::cout << "MovementSystem: Added entity with id: " << entity
                    << " and sig: " << signature << std::endl;
        });
    connectEntityRemovedSignal([](const ecs::Entity &entity) {
      std::cout << "MovementSystem: Removed entity with id: " << entity
                << std::endl;
    });
  }
  void receive(const TestEvent &) {
    std::cout << "Event received" << std::endl;
  }
};

TEST_CASE("Coordinator system & event test.", "[COORDINATOR]") {
  std::cout << "Coordinator test." << std::endl;
  coordiantor.registerComponent<Position>();
  coordiantor.registerSystem<MovementSystem>(
      std::bitset<ecs::MAX_COMPONENTS>().set(
          coordiantor.getComponentFamily<Position>(), true));
  coordiantor.setSystemSignature<MovementSystem>(
      std::bitset<ecs::MAX_COMPONENTS>().set(
          coordiantor.getComponentFamily<Position>(), true));
  coordiantor.registerEvent<TestEvent>();
  MovementSystem system;
  coordiantor.subscribeToEvent<TestEvent, MovementSystem>(system);
  coordiantor.emitEvent<TestEvent>();
  coordiantor.unsubscribeFromEvent<TestEvent, MovementSystem>(system);
}

TEST_CASE("Coordinator entity & component test.", "[COORDINATOR") {
  ecs::Entity entity = coordiantor.createEntity();
  coordiantor.addComponent<Position>(entity, 1, 2, 3);
  REQUIRE(Position(1, 2, 3) == coordiantor.getComponent<Position>(entity));
  coordiantor.removeComponent<Position>(entity);
  coordiantor.addComponent<Position>(entity, Position(22, 2, 2));
  coordiantor.destoryEntity(entity);
}
} // namespace coordinator_test
