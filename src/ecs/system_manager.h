#pragma once

#include "common.h"
#include "third_party/simplesignal.h"
#include <memory>
#include <set>
#include <vector>

namespace ecs {

class BaseSystem {
public:
  using EntityAddedSignal =
      Simple::Signal<void(const Entity &, const Signature &)>;
  using EntityRemovedSignal = Simple::Signal<void(const Entity &)>;

protected:
  static SystemFamily familyCount;
};

/**
 * All systems must be static or singleton,
 * They exists from start to end of the program.
 *
 * All systems must be registered before adding creating entites.
 */
template <typename T> class System : BaseSystem {
public:
  System() { assert(family && "System must be registered before use."); }
  void connectEntityAddedSignal(
      const std::function<void(const Entity &, const Signature &)> &callback) {
    entityAddedSignal.connect(callback);
  }
  void connectEntityRemovedSignal(
      const std::function<void(const Entity &)> &callback) {
    entityRemovedSignal.connect(callback);
  }
  static std::set<Entity> getEntites() { return entites; }
  static SystemFamily getFamily() { return family; }

private:
  static std::set<Entity> entites;
  static EntityAddedSignal entityAddedSignal;
  static EntityRemovedSignal entityRemovedSignal;
  static SystemFamily family;
  static SystemFamily genFamily() {
    if (!family)
      family = familyCount++;
    return family;
  }
  friend class SystemManager;
};
template <typename T> SystemFamily System<T>::family = INVALID_SYSTEM_FAMILY;
template <typename T>
BaseSystem::EntityAddedSignal System<T>::entityAddedSignal;
template <typename T>
BaseSystem::EntityRemovedSignal System<T>::entityRemovedSignal;
template <typename T> std::set<Entity> System<T>::entites{};

class SystemManager : NonCopyable {
public:
  static SystemManager &getInstance() {
    static SystemManager instance;
    return instance;
  }

  template <typename T> void registerSystem(const Signature &signature) {
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
    // since valid system family starts from 1 and arrays start from 0
    signatures[family - 1] = signature;
    entityAddedSignals[family - 1] = &System<T>::entityAddedSignal;
    entityRemovedSignals[family - 1] = &System<T>::entityRemovedSignal;
    systems[family - 1] = &System<T>::entites;
  }

  template <typename T> void setSignature(const Signature &signature) {
    checkIsDerived<T>();
    assert(System<T>::family && "System used before registered.");
    signatures[System<T>::family - 1] = signature;
  }

  size_t systemCount() const { return systems.size(); }
  void entityDestoryed(Entity entity);
  void entitySignatureChanged(Entity entity, Signature entitySignature);

private:
  SystemManager() = default;
  ~SystemManager() = default;

  template <typename T> constexpr void checkIsDerived() {
    static_assert(std::is_base_of<System<T>, T>::value,
                  "T not derived from System.");
  }
  // System arrays, [index - system id]
  std::vector<Signature> signatures;
  // system is set of entites that match system signature
  std::vector<std::set<Entity> *> systems;
  std::vector<BaseSystem::EntityAddedSignal *> entityAddedSignals;
  std::vector<BaseSystem::EntityRemovedSignal *> entityRemovedSignals;
};
} // namespace ecs
