#include "system_manager.h"

namespace ecs {

SystemFamily BaseSystem::familyCount = 1;
void SystemManager::entityDestoryed(Entity entity) {
  for (size_t i = 0; i < systems.size(); ++i) {
    size_t ret = systems[i]->erase(entity);
    if (ret)
      entityRemovedSignals[i]->emit(entity);
  }
}

void SystemManager::entitySignatureChanged(Entity entity,
                                           Signature entitySignature) {
  for (size_t i = 0; i < systems.size(); ++i) {
    auto const &systemSignature = signatures[i];
    auto const &system = systems[i];
    if ((entitySignature & systemSignature) == systemSignature) {
      system->insert(entity);
      entityAddedSignals[i]->emit(entity, entitySignature);
    } else {
      size_t ret = system->erase(entity);
      if (ret)
        entityRemovedSignals[i]->emit(entity);
    }
  }
}

} // namespace ecs
