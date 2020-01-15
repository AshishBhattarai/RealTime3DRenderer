#include "system_manager.h"

namespace ecs {

SystemFamily BaseSystem::familyCount = 1;
template <typename T> SystemFamily System<T>::family = INVALID_SYSTEM_FAMILY;

void SystemManager::entityDestoryed(Entity entity) {
  for (size_t i = 0; i < systems.size(); ++i) {
    auto removed = systems[i]->entites.erase(entity);
    if (removed)
      entityRemovedSignals[i]->emit(entity);
  }
}

void SystemManager::entitySignatureChanged(Entity entity,
                                           Signature entitySignature) {
  for (size_t i = 0; i < systems.size(); ++i) {
    auto const &systemSignature = signatures[i];
    auto const &system = systems[i];
    if ((entitySignature & systemSignature) == systemSignature) {
      system->entites.insert(entity);
      entityAddedSignals[i]->emit(entity, entitySignature);
    } else {
      system->entites.erase(entity);
      entityRemovedSignals[i]->emit(entity);
    }
  }
}

} // namespace ecs
