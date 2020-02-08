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
    : display("App", 1024, 768), input(display),
      coordinator(ecs::Coordinator::getInstance()) {
  DEBUG_SLOG("App constructed.");
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
  camera = new Camera();
  renderSystem->setCamera(camera);

  // load model

  tinygltf::Model model;
  status = Loaders::loadModel(model, "resources/meshes/sphere_cube.gltf");
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

  input.addKeyCallback(INPUT_KEY_ESCAPE, [](const Input::KeyEvent &event) {
    DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
  });

  renderSystem->updateProjectionMatrix(display.getAspectRatio());

  input.addCursorCallback([&camera = camera](const Input::CursorPos &offset) {
    camera->processRotation(offset.xPos, offset.yPos);
  });
}

void App::processInput() { camera->update(); }

void App::run() {
  DEBUG_SLOG("App running.");
  while (!display.shouldClose()) {
    input.update();
    processInput();
    renderSystem->update(0.0f);
    display.update();
  }
}

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete renderSystem;
  delete camera;
}

} // namespace app
