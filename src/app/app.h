#pragma once

#include "app_ui.h"
#include "command_server.h"
#include "core/shared_queue.h"
#include "display.h"
#include "gui_manager.h"
#include "input.h"
#include "types.h"
#include <asio/thread_pool.hpp>
#include <map>

namespace ecs {
class Coordinator;
}
namespace render_system {
class Camera;
class RenderSystem;
} // namespace render_system
namespace world_system {
class WorldSystem;
class WorldObject;
} // namespace world_system
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

  void renderSphere();
  void renderHelments();
  void renderLantern();
  void renderTank();

private:
  asio::thread_pool threadPool;
  CommandServer commandServer;
  Display display;
  Input input;
  GuiManager gui;
  AppUi appUi;
  ecs::Coordinator &coordinator;
  world_system::WorldSystem *worldSystem;
  render_system::RenderSystem *renderSystem;
  render_system::Camera *camera;
  std::map<std::string, GPUMeshMetaData> nameToMeshes;
  world_system::WorldObject *testLight1;
  world_system::WorldObject *testLight2;

  void processInput(float dt);
  render_system::RenderSystem *createRenderSystem(int width, int height);
};
} // namespace app
