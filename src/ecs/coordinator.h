#pragma once

#include "component_manager.h"
#include "default_events.h"
#include "entity_manager.h"
#include "event_manager.h"
#include "system_manager.h"
#include <bitset>
#include <memory>

namespace ecs {
class Coordinator : NonCopyable {
public:
  EntityManager &entityManager = EntityManager::getInstace();
  ComponentManager &componentManager = ComponentManager::getInstace();
  SystemManager &systemManager = SystemManager::getInstance();
  EventManager &eventManager = EventManager::getInstace();

  static Coordinator &getInstance() {
    static Coordinator instance;
    return instance;
  }

  Entity createEntity();
  void destoryEntity(Entity entity);

  // component
  template <typename T> ComponentFamily registerComponent() {
    return componentManager.registerComponent<T>();
  }

  template <typename T> void addComponent(Entity entity, const T &component) {
    componentManager.addComponent<T>(entity, component);
    Signature sig =
        entityManager.updateSignaure(entity, componentManager.getComponentFamily<T>(), true);
    eventManager.emit<event::EntityChanged>(entity, sig, event::EntityChanged::Status::UPDATED);
    systemManager.entitySignatureChanged(entity, sig);
  }

  template <typename T, typename... Args> void addComponent(Entity entity, Args &&... args) {
    T component(std::forward<Args>(args)...);
    componentManager.addComponent<T>(entity, component);
    Signature sig =
        entityManager.updateSignaure(entity, componentManager.getComponentFamily<T>(), true);
   eventManager.emit<event::EntityChanged>(entity, sig, event::EntityChanged::Status::UPDATED);
    systemManager.entitySignatureChanged(entity, sig);
  }

  template <typename T> void removeComponent(Entity entity) {
    componentManager.removeComponent<T>(entity);
    Signature signature =
        entityManager.updateSignaure(entity, componentManager.getComponentFamily<T>(), false);
    systemManager.entitySignatureChanged(entity, signature);
  }

  template <typename T> bool hasComponent(Entity entity) {
    std::bitset sig = entityManager.getSignature(entity);
    ComponentFamily fmly = getComponentFamily<T>();
    return sig[fmly];
  }

  template <typename T> T &getComponent(Entity entity) {
    return componentManager.getComponent<T>(entity);
  }

  template <typename T> ComponentFamily getComponentFamily() {
    return componentManager.getComponentFamily<T>();
  }

  // system
  template <typename T> void registerSystem(const Signature &signature) {
    systemManager.registerSystem<T>(signature);
  }

  template <typename T> void setSystemSignature(const Signature &signature) {
    systemManager.setSignature<T>(signature);
  }

  // event
  template <typename T> void registerEvent() { eventManager.registerEvent<T>(); }

  template <typename E, typename R> void subscribeToEvent(R &receiver) {
    eventManager.subscribe<E, R>(receiver);
  }

  template <typename E, typename R> void unsubscribeFromEvent(R &reciver) {
    eventManager.unsubscribe<E, R>(reciver);
  }

  template <typename E> void emitEvent(const E &event) { eventManager.emit<E>(event); }
  template <typename E, typename... Args> void emitEvent(Args &&... args) {
    E event(std::forward<Args>(args)...);
    eventManager.emit<E>(event);
  }

private:
  Coordinator() { registerEvent<event::EntityChanged>(); };
};

} // namespace ecs
