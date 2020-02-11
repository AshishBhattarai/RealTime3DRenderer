#include "app.h"

#include "common.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "display.h"
#include "ecs/coordinator.h"
#include "input.h"
#include "loaders.h"
#include "systems/render_system/camera.h"
#include "systems/render_system/model.h"
#include "systems/render_system/render_system.h"
#include "systems/world_system/world_system.h"
#include "utils/image.h"
#include "utils/slogger.h"
#include <iostream>
#include <map>
#include <third_party/tinygltf/tiny_gltf.h>

using namespace render_system;

namespace app {
App::App(int, char **)
    : display("App", 1024, 700), input(display), config(),
      coordinator(ecs::Coordinator::getInstance()),
      worldSystem(new world_system::WorldSystem()),
      camera(new Camera(glm::vec3(0.0f, 0.0f, -5.0f))) {
  DEBUG_SLOG("App constructed.");
  //  input.setCursorStatus(INPUT_CURSOR_DISABLED);

  /* Init RenderSystem */
  Image checkerImage;
  bool status =
      app::Loaders::loadImage(checkerImage, "resources/defaults/checker.bmp");
  renderSystem = new RenderSystem(RenderSystemConfig(&checkerImage));
  renderSystem->setCamera(camera);
  renderSystem->updateProjectionMatrix(display.getAspectRatio());

  // load model
  tinygltf::Model model;
  status = Loaders::loadModel(model, "resources/meshes/sphere.gltf");
  std::map<std::string, uint> ids = renderSystem->registerMeshes(model);
  DEBUG_CSLOG("LOADED MESHES: ", ids.size());
  component::Mesh mesh(*ids.begin());

  input.addKeyCallback(
      INPUT_KEY_ESCAPE, [&display = display](const Input::KeyEvent &event) {
        DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
        display.setShouldClose(true);
      });

  input.addCursorCallback([&camera = camera](const Input::CursorPos &dt) {
    camera->processRotation(dt.xPos, dt.yPos);
  });

  // Add world object
  world_system::WorldObject &worldObject = worldSystem->createWorldObject(
      component::Transform(glm::vec3(0.0f, 0.0f, -10.0f)));
  worldObject.addComponent<component::Mesh>(mesh);
  worldSystem->getWorldObject(worldObject.getId());
}

void App::processInput(float dt) {
  bool keyW = input.getKey(INPUT_KEY_W);
  bool keyA = input.getKey(INPUT_KEY_A);
  bool keyD = input.getKey(INPUT_KEY_D);
  bool keyS = input.getKey(INPUT_KEY_S);

  if (keyW && keyA) {
    camera->processMovement(CameraMovement::STRAFE_LEFT, dt);
  } else if (keyW && keyD) {
    camera->processMovement(CameraMovement::STRAFE_RIGHT, dt);
  } else if (keyW) {
    camera->processMovement(CameraMovement::FORWARD, dt);
  } else if (keyA) {
    camera->processMovement(CameraMovement::LEFT, dt);
  } else if (keyD) {
    camera->processMovement(CameraMovement::RIGHT, dt);
  } else if (keyS) {
    camera->processMovement(CameraMovement::BACKWARD, dt);
  }

  camera->update();
}

void App::run() {
  DEBUG_SLOG("App running.");
  float lt, ct, dt = 0.0f;
  lt = ct = display.getTime();
  while (!display.shouldClose()) {
    // calculate delta time
    ct = display.getTime();
    dt = ct - lt;
    lt = display.getTime();

    processInput(dt);

    worldSystem->update(dt);
    renderSystem->update(dt);
    display.update();
    input.update();
  }
}

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete renderSystem;
  delete camera;
  delete worldSystem;
}

} // namespace app
