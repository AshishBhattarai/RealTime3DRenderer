#pragma once

#include "construct.h"
#include "display.h"
#include "input.h"
#include "types.h"

namespace ecs {
class Coordinator;
}
namespace render_system {
class Camera;
class RenderSystem;
} // namespace render_system
namespace world_system {
class WorldSystem;
}
namespace app {
/**
 * Represents the renderer application, handels and manages WINDOW(Context), ECS
 * & UI
 */
class App : NonCopyable {
public:
  // lazy init instance
  App(int argc, char **argv);
  ~App();

  void run();

private:
  void processInput(float dt);

private:
  Display display;
  Input input;
  Construct construct;
  ecs::Coordinator &coordinator;
  world_system::WorldSystem *worldSystem;
  render_system::RenderSystem *renderSystem;
  render_system::Camera *camera;
};
} // namespace app
