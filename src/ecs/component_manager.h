#pragma once

#include "common.h"
#include "component_array.h"
#include <array>
#include <memory>
#include <typeinfo>

namespace ecs {
/**
 * @brief The BaseComponent class
 *  BaseComponent store type count for all components
 */
class BaseComponent {
protected:
  static ComponentFamily familyCount;
};

/**
 * Contains a type id for each component type T
 */
template <typename T> class Component : BaseComponent {
private:
  static ComponentFamily family;
  friend class ComponentManager;

  static ComponentFamily genFamily() {
    if (!family) {
      family = familyCount++;
      assert(family < MAX_COMPONENTS &&
             "Maximum number of components reached.");
    }
    return family;
  }
};

/**
 * @brief The ComponentManager class
 * A class to manage components and their arrays.
 */
class ComponentManager : NonCopyable {
public:
  template <typename T> void registerComponent() {
    asser(!Component<T>::family && "Registering component more than once.");
    ComponentFamily family = Component<T>::genFamily();
    componentArrays[family] = std::make_unique<ComponentArray<T>>();
  }

  template <typename T> ComponentFamily getComponentFamily() {
    return Component<T>::family;
  }

  template <typename T>
  void addComponent(Entity entity, Signature entitySignature, T component) {
    assert(!entitySignature[Component<T>::family] &&
           "Entity must have unique components.");
    getComponentArray<T>()->insertData(entity, component);
  }

  template <typename T>
  void removeComponent(Entity entity, Signature entitySignature) {
    assert(!entitySignature[Component<T>::family] &&
           "Entity doesn't have the component.");
    getComponentArray<T>()->removeData(entity);
  }

  template <typename T>
  T &getComponent(Entity entity, Signature entitySignature) {
    assert(!entitySignature[Component<T>::family] &&
           "Entity doesn't have the component.");
    return getComponentArray<T>()->getData(entity);
  }

  void entityDestoryed(Entity entity, Signature entitySignature);

private:
  std::array<std::unique_ptr<BaseComponentArray>, MAX_COMPONENTS>
      componentArrays{};
  // get raw pointer to component array of T component
  template <typename T> const ComponentArray<T> *getComponentArray() const {
    ComponentFamily family = getComponentFamily<T>()
        assert(!family && "Components must be registered before use.");
    return static_cast<const ComponentArray<T> *>(
        componentArrays[family].get());
  }
};
} // namespace ecs
