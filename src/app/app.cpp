#include "app.h"

#include "common.h"
#include "display.h"
#include "ecs/coordinator.h"
#include "input.h"
#include "systems/render_system/camera.h"
#include "systems/render_system/render_defaults.h"
#include <iostream>

namespace app {
App::App(int, char **)
    : display(new Display("App", 1024, 768)), input(new Input(*display)),
      coordinator(ecs::Coordinator::getInstance()) {
  DEBUG_SLOG("App constructed.");
  auto &renderDefaults = render_system::RenderDefaults::getInstance(
      "resources/defaults/checker.bmp");
  Camera();
}

void App::processInput() {
  input->addKeyCallback(INPUT_KEY_ESCAPE, [](const Input::KeyEvent &event) {
    DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
  });
}

void App::run() {
  DEBUG_SLOG("App running.");
  while (!display->shouldClose()) {
    input->update();
    processInput();
    display->update();
  }
}

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete display;
}

} // namespace app
