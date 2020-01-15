#include "component_manager.h"

namespace ecs {

ComponentFamily BaseComponent::familyCount = 1;
template <typename T>
ComponentFamily Component<T>::family = INVALID_COMPONENT_FAMILY;

void ComponentManager::entityDestoryed(Entity entity,
                                       Signature entitySignature) {
  for (ComponentFamily i = 0; i < MAX_COMPONENTS; ++i) {
    if (entitySignature[i]) // check if entity has the component
      componentArrays[i]->entityDestoryed(entity);
  }
}

} // namespace ecs
