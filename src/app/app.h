#pragma once

#include "command_server.h"
#include "construct.h"
#include "core/shared_queue.h"
#include "display.h"
#include "input.h"
#include "types.h"
#include <asio/thread_pool.hpp>

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
  static constexpr uint NUM_THREADS = 2;
  // lazy init instance
  App(int argc, char **argv);
  ~App();

  void run();
  void runRenderLoop(std::string_view renderOutpu);

private:
  asio::thread_pool threadPool;
  CommandServer commandServer;
  Display display;
  Input input;
  Construct construct;
  ecs::Coordinator &coordinator;
  world_system::WorldSystem *worldSystem;
  render_system::RenderSystem *renderSystem;
  render_system::Camera *camera;

  void processInput(float dt);
};
} // namespace app
