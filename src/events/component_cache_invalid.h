#pragma once

#include "ecs/event_manager.h"

namespace event {
template <typename T>
class ComponentCacheInvalid : ecs::Event<ComponentCacheInvalid<T>> {
};
} // namespace event
