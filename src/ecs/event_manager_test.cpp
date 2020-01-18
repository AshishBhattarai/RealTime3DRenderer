#include "event_manager.h"
#include "third_party/catch.hpp"
#include <iostream>

namespace event_manager_test {
ecs::EventManager &eventManager = ecs::EventManager::getInstace();

struct TestEvent : public ecs::Event<TestEvent> {
  int stateX = 0;
  int stateY = 0;
  TestEvent(int stateX, int stateY) : stateX(stateX), stateY(stateY) {}
};

class TestSystem : public ecs::Receiver<TestEvent> {
public:
  void receive(const TestEvent &event) {
    std::cout << "event received(Test System) stateX: " << event.stateX
              << " stateY: " << event.stateY << std::endl;
  }
};

class TestSystem2 : public ecs::Receiver<TestEvent> {
public:
  void receive(const TestEvent &event) {
    std::cout << "event received(Test System 2) stateX: " << event.stateX
              << " stateY: " << event.stateY << std::endl;
  }
};

TEST_CASE("EventManager event registration test", "[EVENT_MANAGER]") {
  eventManager.registerEvent<TestEvent>();
  REQUIRE(eventManager.totalEvents() == 1);
}

TEST_CASE("EventManager event subscribe test.", "[EVENT_MANAGER]") {
  TestSystem testSystem;
  eventManager.subscribe<TestEvent, TestSystem>(testSystem);
  REQUIRE(eventManager.totalConnectedReceivers() == 1);
}

TEST_CASE("EventManager event emit test.", "[EVENT_MANAGER]") {
  std::cout << "EventManager event emit test." << std::endl;
  REQUIRE(eventManager.totalConnectedReceivers() == 0);
  TestSystem testSystem;
  eventManager.subscribe<TestEvent, TestSystem>(testSystem);
  eventManager.emit<TestEvent>(TestEvent(0, 1));
  eventManager.emit<TestEvent>(2, 4);
  TestSystem2 testSystem2;
  eventManager.subscribe<TestEvent, TestSystem2>(testSystem2);
  eventManager.emit<TestEvent>(5, 5);
  REQUIRE(eventManager.totalConnectedReceivers() == 2);
}

TEST_CASE("EventManager event unsubscribe test.", "[EVENT_MANAGER]") {
  std::cout << "EventManager event unsubscribe test." << std::endl;
  REQUIRE(eventManager.totalConnectedReceivers() == 0);
  REQUIRE(eventManager.totalEvents() == 1);
  TestSystem testSystem;
  eventManager.subscribe<TestEvent, TestSystem>(testSystem);
  eventManager.emit<TestEvent>(TestEvent(10, 1));
  eventManager.unsubscribe<TestEvent, TestSystem>(testSystem);
  eventManager.emit<TestEvent>(TestEvent(20, 4));
  REQUIRE(eventManager.totalConnectedReceivers() == 0);
}
} // namespace event_manager_test
