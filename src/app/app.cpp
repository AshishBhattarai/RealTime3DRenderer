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
#include "systems/render_system/render_defaults.h"
#include "systems/render_system/render_system.h"
#include "utils/image.h"
#include "utils/slogger.h"
#include <iostream>
#include <map>
#include <third_party/tinygltf/tiny_gltf.h>

using namespace render_system;

namespace app {
App::App(int, char **)
    : display("App", 1024, 700), input(display),
      coordinator(ecs::Coordinator::getInstance()) {
  DEBUG_SLOG("App constructed.");
  input.setCursorStatus(INPUT_CURSOR_DISABLED);
  /* Render System classes should not be constructed before loading defauls*/
  Image checkerImage;
  bool status =
      Loaders::loadImage(checkerImage, "resources/defaults/checker.bmp");
  render_system::RenderDefaults::getInstance(&checkerImage);

  auto transformFamily =
      coordinator.componentManager.registerComponent<component::Transform>();
  auto meshFamily =
      coordinator.componentManager.registerComponent<component::Mesh>();
  ecs::Signature renderSig;
  renderSig.set(transformFamily, true);
  renderSig.set(meshFamily, true);
  coordinator.systemManager.registerSystem<RenderSystem>(renderSig);
  renderSystem = new RenderSystem();
  camera = new Camera(glm::vec3(0.0f, 0.0f, -5.0f));

  // load model

  tinygltf::Model model;
  status = Loaders::loadModel(model, "resources/meshes/sphere.gltf");
  std::map<std::string, uint> ids = renderSystem->registerMeshes(model);
  DEBUG_CSLOG("LOADED MESHES: ", ids.size());
  ecs::Entity temp = coordinator.createEntity();
  coordinator.addComponent<component::Transform>(
      temp, component::Transform(glm::vec3(0.0f, 0.0f, -10.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
  component::Mesh mesh;
  mesh.name = model.meshes[0].name;
  mesh.modelId = ids.begin()->second;
  coordinator.addComponent<component::Mesh>(temp, mesh);

  input.addKeyCallback(
      INPUT_KEY_ESCAPE, [&display = display](const Input::KeyEvent &event) {
        DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
        display.setShouldClose(true);
      });

  renderSystem->setCamera(camera);
  renderSystem->updateProjectionMatrix(display.getAspectRatio());

  input.addCursorCallback([&camera = camera](const Input::CursorPos &dt) {
    camera->processRotation(dt.xPos, dt.yPos);
  });
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
    renderSystem->update(dt);
    display.update();
    input.update();
  }
}

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete renderSystem;
  delete camera;
}

} // namespace app
