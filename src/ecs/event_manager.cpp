#include "event_manager.h"

namespace ecs {
EventFamily BaseEvent::familyCount = 1;
template <typename T> EventFamily Event<T>::family = INVALID_EVENT_FAMILY;
} // namespace ecs
