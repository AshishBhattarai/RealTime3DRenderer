#pragma once

#include "ecs/system_manager.h"

namespace render_system {
class RenderSystem : ecs::System<RenderSystem> {
public:
  RenderSystem();
};

} // namespace render_system
