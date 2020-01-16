#pragma once

#include "component_manager.h"
#include "entity_manager.h"
#include "event_manager.h"
#include "system_manager.h"
#include <memory>

namespace ecs {
class Coordinator : NonCopyable {
public:
  static Coordinator &getInstance() {
    static Coordinator instance;
    return instance;
  }

  Entity createEntity();
  void destoryEntity(Entity entity);

  // component
  template <typename T> void registerComponent() {
    componentManager->registerComponent<T>();
  }

  template <typename T> void addComponent(Entity entity, T component) {
    componentManager->addComponent<T>(entity, component);
    Signature signature = entityManager->updateSignaure(
        entity, componentManager->getComponentFamily<T>(), true);
    systemManager->entitySignatureChanged(entity, signature);
  }

  template <typename T> void removeComponent(Entity entity) {
    componentManager->removeComponent<T>(entity);
    Signature signature = entityManager->updateSignaure(
        entity, componentManager->getComponentFamily<T>(), false);
    systemManager->entitySignatureChanged(entity, signature);
  }

  template <typename T> T &getComponent(Entity entity) {
    return componentManager->getComponent<T>(
        entity, entityManager->getSignature(entity));
  }

  template <typename T> ComponentFamily getComponentFamily() {
    return componentManager->getComponentFamily<T>();
  }

  // system
  template <typename T> std::shared_ptr<T> registerSystem() {
    return systemManager->registerSystem<T>();
  }

  template <typename T> void setSystemSignature(Signature signature) {
    systemManager->setSignature<T>(signature);
  }

  // event
  template <typename T> void registerEvent() {
    eventManager->registerEvent<T>();
  }

  template <typename E, typename R>
  void subscribeToEvent(Receiver<E> &receiver) {
    eventManager->subscribe<E, R>(receiver);
  }

  template <typename E, typename R>
  void unsubscribeFromEvent(Receiver<E> &reciver) {
    eventManager->unsubscribe<E, R>(reciver);
  }

  template <typename E> void emitEvent(const E &event) {
    eventManager->emit<E>(event);
  }
  template <typename E, typename... Args> void emitEvent(Args &&... args) {
    eventManager->emit<E, Args...>(std::forward(args)...);
  }

private:
  Coordinator();

  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<ComponentManager> componentManager;
  std::unique_ptr<SystemManager> systemManager;
  std::unique_ptr<EventManager> eventManager;
};

} // namespace ecs
