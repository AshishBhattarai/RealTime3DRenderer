#include "world_object.h"
#include "ecs/common.h"

namespace world_system {

WorldObject::WorldObject()
    : id(INVALID_WORD_OBJECT_ID), entityId(ecs::INVALID_ENTITY),
      transform(nullptr), onUpdate(std::make_unique<OnUpdateSignal>()) {}

WorldObject::~WorldObject() {}

} // namespace world_system
