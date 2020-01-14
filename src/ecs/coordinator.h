#pragma once

#include "component_manager.h"
#include "entity_manager.h"
#include "system_manager.h"
#include <memory>

namespace ecs {
class Coordinator {
public:
  Coordinator(const Coordinator &) = delete;
  Coordinator &operator=(const Coordinator &) = delete;

  static Coordinator &getInstance() {
    static Coordinator instance;
    return instance;
  }

  Entity createEntity();
  void destoryEntity(Entity entity);

  // component
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

private:
  Coordinator();

  std::unique_ptr<EntityManager> entityManager;
  std::unique_ptr<ComponentManager> componentManager;
  std::unique_ptr<SystemManager> systemManager;
  // TODO: event manager
};

} // namespace ecs
