#include "app.h"

#include "common.h"
#include "display.h"
#include "ecs/coordinator.h"
#include "systems/render_system/render_defaults.h"
#include <iostream>

namespace app {
App::App(int, char **)
    : display(new Display("App", 1024, 768)),
      coordinator(ecs::Coordinator::getInstance()) {
  DEBUG_SLOG("App constructed.");
  auto &renderDefaults = render_system::RenderDefaults::getInstance(
      "resources/defaults/checker.bmp");
}

void App::run() {
  DEBUG_SLOG("App running.");
  while (!display->shouldClose()) {
    display->update();
  }
}

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete display;
}

} // namespace app
