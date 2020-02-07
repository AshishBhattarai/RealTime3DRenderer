#include "app.h"

#include "common.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "display.h"
#include "ecs/coordinator.h"
#include "input.h"
#include "systems/render_system/camera.h"
#include "systems/render_system/render_defaults.h"
#include "systems/render_system/render_system.h"
#include <iostream>

using namespace render_system;

namespace app {
App::App(int, char **)
    : display("App", 1024, 768), input(display),
      coordinator(ecs::Coordinator::getInstance()) {
  DEBUG_SLOG("App constructed.");
  /* Render System classes should not be constructed before loading defauls*/
  render_system::RenderDefaults::getInstance("resources/defaults/checker.bmp");

  auto transformFamily =
      coordinator.componentManager.registerComponent<component::Transform>();
  auto meshFamily =
      coordinator.componentManager.registerComponent<component::Mesh>();
  ecs::Signature renderSig;
  renderSig.set(transformFamily, true);
  renderSig.set(meshFamily, true);
  coordinator.systemManager.registerSystem<RenderSystem>(renderSig);
  renderSystem = new RenderSystem();
  camera = new Camera();
}

void App::processInput() {
  input.addKeyCallback(INPUT_KEY_ESCAPE, [](const Input::KeyEvent &event) {
    DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
  });
}

void App::run() {
  DEBUG_SLOG("App running.");
  while (!display.shouldClose()) {
    input.update();
    processInput();
    display.update();
  }
}

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete renderSystem;
  delete camera;
}

} // namespace app
