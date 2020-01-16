#pragma once

#include "../third_party/simplesignal.h"
#include "common.h"
#include <memory>
#include <set>
#include <vector>

namespace ecs {

class BaseSystem {
public:
  std::set<Entity> getEntitySet() const { return entites; }

protected:
  static SystemFamily familyCount;
  std::set<Entity> entites;

  friend class SystemManager;
};

template <typename T> class System : BaseSystem {
public:
  static SystemFamily getFamily() { return family; }

protected:
  static const std::shared_ptr<Simple::Signal<void(Entity, Signature)>>
      entityAddeSignal;
  static const std::shared_ptr<Simple::Signal<void(Entity)>>
      entityRemovedSignal;

private:
  static SystemFamily family;
  static SystemFamily genFamily() {
    if (!family)
      family = familyCount++;
    return family;
  }
};

class SystemManager {
public:
  template <typename T> std::shared_ptr<T> registerSystem(Signature signature) {
    checkIsDerived<T>();
    assert(!System<T>::family && "Registering system more than once.");
    SystemFamily family = System<T>::genFamily();
    if (family >= systems.size()) {
      // make sure there is enough space for new system
      size_t newSize = signatures.size() + 1;
      signatures.resize(newSize);
      systems.resize(newSize);
      entityAddedSignals.resize(newSize);
      entityRemovedSignals.resize(newSize);
    }
    // create new system
    signatures[family] = signature;
    auto system = std::make_shared<T>();
    auto entityAddedSignal =
        std::make_shared<Simple::Signal<void(Entity, Signature)>>();
    auto entityRemovedSignal = std::make_shared<Simple::Signal<void(Entity)>>();
    entityAddedSignals[family] = entityAddedSignal;
    entityRemovedSignals[family] = entityRemovedSignal;
    systems[family] = system;
    System<T>::entityAddeSignal = entityAddedSignal;
    System<T>::entityRemovedSignal = entityRemovedSignal;
    return system;
  }

  template <typename T> void setSignature(Signature signature) {
    checkIsDerived<T>();
    assert(System<T>::family && "System used before registered.");
    signatures[System<T>::family] = signature;
  }

  void entityDestoryed(Entity entity);
  void entitySignatureChanged(Entity entity, Signature entitySignature);

private:
  template <typename T> constexpr void checkIsDerived() {
    static_assert(std::is_base_of<System<T>, T>::value,
                  "T not derived from System.");
  }
  // System arrays, [index - system id]
  std::vector<Signature> signatures;
  std::vector<std::shared_ptr<BaseSystem>> systems;
  std::vector<std::shared_ptr<Simple::Signal<void(Entity, Signature)>>>
      entityAddedSignals;
  std::vector<std::shared_ptr<Simple::Signal<void(Entity)>>>
      entityRemovedSignals;
};
} // namespace ecs
