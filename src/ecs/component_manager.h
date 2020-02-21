#pragma once

#include "common.h"
#include "component_array.h"
#include <array>
#include <memory>
#include <typeinfo>
#include <utility>

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
  static ComponentFamily genFamily() {
    if (!family) {
      family = familyCount++;
      assert(family < MAX_COMPONENTS &&
             "Maximum number of components reached.");
    }
    return family;
  }
  friend class ComponentManager;
};
template <typename T>
ComponentFamily Component<T>::family = INVALID_COMPONENT_FAMILY;

/**
 * @brief The ComponentManager class
 * A class to manage components and their arrays.
 */
class ComponentManager : NonCopyable {
public:
  static ComponentManager &getInstace() {
    static ComponentManager instance;
    return instance;
  }

  template <typename T> ComponentFamily registerComponent() {
    assert(!Component<T>::family && "Registering component more than once.");
    ComponentFamily family = Component<T>::genFamily();
    // since valid component family starts from 1 and arrays start from 0
    componentArrays[family - 1] = std::make_unique<ComponentArray<T>>();
    return family;
  }

  template <typename T> ComponentFamily getComponentFamily() {
    return Component<T>::family;
  }

  template <typename T> bool addComponent(Entity entity, const T &component) {
    return getComponentArray<T>()->insertData(entity, component);
  }

  template <typename T, typename... Args>
  void addComponent(Entity entity, Args &&... args) {
    T component(std::forward<Args>(args)...);
    getComponentArray<T>()->insertData(entity, component);
  }

  template <typename T> void removeComponent(Entity entity) {
    getComponentArray<T>()->removeData(entity);
  }

  template <typename T> T &getComponent(Entity entity) {
    return getComponentArray<T>()->getData(entity);
  }

  /**
   * Returns total number of componets of type T
   */
  template <typename T> size_t getComponentCount() {
    assert(Component<T>::family &&
           "Components must be regiestered before use.");
    return static_cast<ComponentArray<T> *>(
               componentArrays[Component<T>::family - 1].get())
        ->getSize();
  }

  void entityDestoryed(Entity entity, Signature entitySignature);

  // get raw pointer to component array of T component
  template <typename T> ComponentArray<T> *getComponentArray() const {
    assert(Component<T>::family && "Components must be registered before use.");
    return static_cast<ComponentArray<T> *>(
        componentArrays[Component<T>::family - 1].get());
  }

private:
  ComponentManager() = default;
  ~ComponentManager() = default;
  std::array<std::unique_ptr<BaseComponentArray>, MAX_COMPONENTS>
      componentArrays{};
};
} // namespace ecs
