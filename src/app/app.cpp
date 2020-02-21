#include "app.h"

#include "app_config.h"
#include "common.h"
#include "components/light.h"
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
#include "utils/buffer.h"
#include "utils/image.h"
#include "utils/slogger.h"
#include <glm/vec3.hpp>
#include <iostream>
#include <map>
#include <third_party/tinygltf/tiny_gltf.h>

using namespace render_system;

namespace app {
App::App(int, char **)
    : display("App", 1024, 700), input(display), construct(),
      coordinator(ecs::Coordinator::getInstance()),
      worldSystem(new world_system::WorldSystem()),
      renderSystem(construct.newRenderSystem(display.getAspectRatio())),
      camera(new Camera(glm::vec3(0.0f, 0.0f, 0.0f))) {
  DEBUG_SLOG("App constructed.");
  //  input.setCursorStatus(INPUT_CURSOR_DISABLED);

  renderSystem->setCamera(camera);
  renderSystem->updateProjectionMatrix(display.getAspectRatio());

  // load model
  tinygltf::Model model;
  Loaders::loadModel(model, "resources/meshes/sphere.gltf");

  input.addKeyCallback(
      INPUT_KEY_ESCAPE, [&display = display](const Input::KeyEvent &event) {
        if (event.action) {
          DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
          display.setShouldClose(true);
        }
      });
  input.addKeyCallback(
      INPUT_KEY_Q, [&input = input](const Input::KeyEvent &event) {
        if (event.action) {
          DEBUG_SLOG("KEY PRESSED: ", event.key, "TIME: ", event.time);
          input.toggleCursorMode();
        }
      });

  input.addCursorCallback([&camera = camera](const Input::CursorPos &dt) {
    camera->processRotation(dt.xPos, dt.yPos);
  });

  int nrRow = 7;
  int nrCOl = 7;
  float spacing = 2.5f;

  // Add world objects
  for (int i = 0; i < nrRow; ++i) {
    float metallic = i / (float)nrRow;
    for (int j = 0; j < nrCOl; ++j) {
      float roughness = j / (float)nrCOl;
      std::map<std::string, uint> ids = renderSystem->registerMeshes(model);
      FlatMaterial newMat;
      newMat.ao = 1.0f;
      newMat.albedo = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
      newMat.emission = glm::vec3(0.0f, 0.0f, 0.0f);
      newMat.metallic = metallic;
      newMat.roughtness = roughness;
      newMat.shaderType = ShaderType::FLAT_FORWARD_SHADER;
      renderSystem->relaceAllMaterial(ids.begin()->second, &newMat);
      component::Mesh mesh(*ids.begin());

      world_system::WorldObject &worldObject =
          worldSystem->createWorldObject(component::Transform(
              glm::vec3((j - (nrCOl / 2.0f)) * spacing,
                        (i - (nrRow / 2.0f)) * spacing, -10.0f)));
      worldObject.addComponent<component::Mesh>(mesh);
    }
  }

  glm::vec3 lightPositions[] = {
      glm::vec3(-10.0f, 10.0f, 10.0f), glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(10.0f, -10.0f, 10.0f)};

  for (uint i = 0; i < render_system::shader::fragment::PointLight::MAX; ++i) {
    glm::vec3 lightPosition = lightPositions[i];
    lightPosition += glm::vec3(sin(display.getTime() * 5.0) * 5.0, 0.0, 0.0);
    world_system::WorldObject &testLight =
        worldSystem->createWorldObject(component::Transform(lightPosition));
    testLight.addComponent<component::Light>(component::Light(
        glm::vec3(1.0f, 1.0f, 1.0f), 300.0f, 100.0f, LightType::POINT_LIGHT));
  }
} // namespace app

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
  float ltf, lt, ct, dt = 0.0f;
  int frameCnt = 0;
  lt = ct = display.getTime(); // time in seconds
  while (!display.shouldClose()) {
    // calculate delta time
    ct = display.getTime();
    dt = ct - lt;
    lt = display.getTime();

    // calculate FPS
    if (ct - ltf >= 1) {
      CSLOG("FPS:", frameCnt);
      frameCnt = 1;
      ltf = ct;
    } else
      frameCnt++;

    processInput(dt);
    worldSystem->update(dt);
    renderSystem->update(dt);
    auto err = glGetError();
    if (err != GL_NO_ERROR)
      CSLOG("OpenGL ERROR:", err);
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
