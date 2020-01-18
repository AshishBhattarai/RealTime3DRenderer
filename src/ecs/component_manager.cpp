#include "component_manager.h"

namespace ecs {

ComponentFamily BaseComponent::familyCount = 1;
void ComponentManager::entityDestoryed(Entity entity,
                                       Signature entitySignature) {
  for (ComponentFamily i = 1; i < MAX_COMPONENTS; ++i) {
    if (entitySignature[i]) // check if entity has the component
      componentArrays[i - 1]->entityDestoryed(entity);
  }
}

} // namespace ecs
