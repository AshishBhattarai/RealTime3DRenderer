#include "app.h"

#include "app_config.h"
#include "command_queues.h"
#include "common.h"
#include "components/light.h"
#include "components/model.h"
#include "components/transform.h"
#include "core/buffer.h"
#include "core/image.h"
#include "core/shared_queue.h"
#include "display.h"
#include "ecs/coordinator.h"
#include "input.h"
#include "loaders.h"
#include "rtsp_client.h"
#include "systems/render_system/camera.h"
#include "systems/render_system/render_system.h"
#include "systems/render_system/scene.h"
#include "systems/world_system/world_system.h"
#include "utils/slogger.h"
#include <asio_noexcept.h>
#include <glm/vec3.hpp>
#include <map>
#include <tinygltf/tiny_gltf.h>

using namespace render_system;

static constexpr int renderWidth = 1440;
static constexpr int renderHeight = 1080;

namespace app {
App::App(int, char **)
    : threadPool(NUM_THREADS), commandServer(8003, 4), display("App"), input(display), gui(input),
      appUi(), coordinator(ecs::Coordinator::getInstance()),
      worldSystem(new world_system::WorldSystem()),
      renderSystem(createRenderSystem(renderWidth, renderHeight)),
      camera(new Camera(glm::vec3(0.0f, 10.0f, 0.0f))), testLight1(nullptr), testLight2(nullptr) {
  DEBUG_SLOG("App constructed.");
  //  input.setCursorStatus(INPUT_CURSOR_DISABLED);

  renderSystem->setCamera(camera);
  renderSystem->updateProjectionMatrix(display.getAspectRatio());

  // load skybox
  Image skybox;
  Loaders::loadImage(skybox, "resources/skybox/indoor.hdr", true);

  renderSystem->setSkyBox(&skybox);

  // load model
  tinygltf::Model model;
  Loaders::loadModel(model, "resources/meshes/sphere.gltf");
  tinygltf::Model helmet;
  Loaders::loadModel(helmet, "resources/meshes/DamagedHelmet.gltf");
  tinygltf::Model lantern;
  Loaders::loadModel(lantern, "resources/meshes/lantern.gltf");
  tinygltf::Model flightHelmet;
  Loaders::loadModel(flightHelmet, "resources/meshes/FlightHelmet.gltf");
  tinygltf::Model gun;
  Loaders::loadModel(gun, "resources/meshes/gun.gltf");
  tinygltf::Model sniper;
  Loaders::loadModel(sniper, "resources/meshes/sniper.gltf");

  ModelRegisterReturn helmetModel = renderSystem->registerGltfModel(helmet);
  ModelRegisterReturn flightHelmetModel = renderSystem->registerGltfModel(flightHelmet);
  ModelRegisterReturn lanternModel = renderSystem->registerGltfModel(lantern);
  ModelRegisterReturn gunModel = renderSystem->registerGltfModel(gun);
  ModelRegisterReturn sniperModel = renderSystem->registerGltfModel(sniper);

  nameToMeshes.emplace("damaged_helmet", appUi.addLoadedMeshes(helmetModel).front());
  nameToMeshes.emplace("lantern_model", appUi.addLoadedMeshes(lanternModel).front());
  nameToMeshes.emplace("flight_helment", appUi.addLoadedMeshes(flightHelmetModel).front());
  nameToMeshes.emplace("gun_model", appUi.addLoadedMeshes(gunModel).front());
  nameToMeshes.emplace("sniper_model", appUi.addLoadedMeshes(sniperModel).front());

  input.addKeyCallback(Input::Key::ESCAPE, [this, state = 0](const Input::KeyEvent &event) mutable {
    if (event.action == Input::Action::PRESS) {
      DEBUG_SLOG("KEY PRESSED: ", toUnderlying<Input::Key>(event.key));
      switch (state) {
      case 0:
        renderSphere();
        ++state;
        break;
      case 1:
        renderHelments();
        ++state;
        break;
      case 2:
        renderLantern();
        ++state;
        break;
      default:
        worldSystem->clearWorld();
        state = 0;
      }
    }
  });
  input.addKeyCallback(Input::Key::Q, [&input = input](const Input::KeyEvent &event) {
    if (event.action == Input::Action::PRESS) {
      DEBUG_SLOG("KEY PRESSED: ", toUnderlying<Input::Key>(event.key));
      input.toggleCursorMode();
    }
  });

  input.addCursorCallback([&camera = camera, &input = input](const Input::CursorPos &dt) {
    if (input.getCursorMode() == Input::CursorMode::DISABLED) camera->processRotation(dt.x, dt.y);
  });

  GLint size;
  glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &size);
  std::cout << "UBO MB: " << size / 1024 << std::endl;
  glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &size);
  std::cout << "SSBO MB: " << size / 1024 << std::endl;

  std::pair brdfLUT = renderSystem->getBrdfLUT();
  std::pair specularConv = renderSystem->getSpecularConvMap();
  std::pair diffuseConv = renderSystem->getDiffuseConvMap();
  std::pair envMap = renderSystem->getEnvMap();

  appUi.setBrdfLUT(brdfLUT.first, brdfLUT.second);
  appUi.setDiffuseConvMap(diffuseConv.first, diffuseConv.second);
  appUi.setSpecularConvMap(specularConv.first, specularConv.second);
  appUi.setEnvMap(envMap.first, envMap.second);
  // https://stackoverflow.com/questions/38543155/opengl-render-face-of-cube-map-to-a-quad

  auto err = glGetError();
  if (err != GL_NO_ERROR) CSLOG("OpenGL ERROR:", err);
} // namespace app

void App::renderSphere() {
  worldSystem->clearWorld();
  testLight1 = nullptr;
  testLight2 = nullptr;

  int nrRow = 7;
  int nrCOl = 7;
  float height = 10.0f;
  float spacing = 2.5f;

  tinygltf::Model model;
  Loaders::loadModel(model, "resources/meshes/sphere.gltf");

  for (int i = 0; i < nrRow; ++i) {
    float metallic = i / (float)nrRow;
    for (int j = 0; j < nrCOl; ++j) {
      float roughness = j / (float)nrCOl;

      MaterialId matId = renderSystem->registerMaterial<FlatMaterial>(
          ShaderType::FLAT_FORWARD_SHADER, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
          glm::vec3(0.0f, 0.0f, 0.0f), metallic, roughness, 1.0f);

      // Add world objects
      ModelRegisterReturn regScene = renderSystem->registerGltfModel(model);
      PrimitiveId primId = regScene.primIdToMatId.front().begin()->first;

      component::Model model = {regScene.meshIds.front(), {{primId, matId}}};

      world_system::WorldObject &worldObject =
          worldSystem->createWorldObject(component::Transform(glm::vec3(
              (j - (nrCOl / 2.0f)) * spacing, ((i - (nrRow / 2.0f)) * spacing) + height, -10.0f)));
      worldObject.addComponent<component::Model>(model);
    }
  }
  glm::vec3 lightPositions[] = {glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 10.0f, 0.0f)};
  testLight1 = &worldSystem->createWorldObject(component::Transform(lightPositions[0]));
  testLight1->addComponent<component::Light>(
      component::Light(glm::vec3(1.0f, 1.0f, 1.0f), 300.0f, 100.0f, LightType::POINT_LIGHT));
  testLight2 = &worldSystem->createWorldObject(component::Transform(lightPositions[0]));
  testLight2->addComponent<component::Light>(
      component::Light(glm::vec3(1.0f, 1.0f, 1.0f), 300.0f, 100.0f, LightType::POINT_LIGHT));
}

void App::renderHelments() {
  worldSystem->clearWorld();
  testLight1 = nullptr;
  testLight2 = nullptr;
  {
    const auto &flightHelmetModel = nameToMeshes["flight_helment"];
    world_system::WorldObject &helmetObject = worldSystem->createWorldObject(component::Transform(
        glm::vec3(0.0f, 4.0f, -8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(3.0f)));
    helmetObject.addComponent<component::Model>(
        {flightHelmetModel.meshId, flightHelmetModel.primIdToMatId});
  }
  {
    const auto &helmetModel = nameToMeshes["damaged_helmet"];
    world_system::WorldObject &helmetObject = worldSystem->createWorldObject(
        component::Transform(glm::vec3(4.0f, 5.0f, -8.0f), glm::vec3(90.0f, 0.0f, 0.0f)));
    helmetObject.addComponent<component::Model>({helmetModel.meshId, helmetModel.primIdToMatId});
  }
  {
    const auto &gunModel = nameToMeshes["gun_model"];
    world_system::WorldObject &gunObject = worldSystem->createWorldObject(
        component::Transform(glm::vec3(-4.0f, 5.0f, -8.0f), glm::vec3(94.0f, -1.0f, -90.07f)));
    gunObject.addComponent<component::Model>({gunModel.meshId, gunModel.primIdToMatId});
  }
  {
    const auto &sniperModel = nameToMeshes["sniper_model"];
    world_system::WorldObject &sniperObject = worldSystem->createWorldObject(
        component::Transform(glm::vec3(12.0f, 5.0f, -8.0f), glm::vec3(0.0f)));
    sniperObject.addComponent<component::Model>({sniperModel.meshId, sniperModel.primIdToMatId});
  }

  glm::vec3 lightPositions[] = {glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f)};
  testLight1 = &worldSystem->createWorldObject(component::Transform(lightPositions[0]));
  testLight1->addComponent<component::Light>(
      component::Light(glm::vec3(1.0f, 1.0f, 1.0f), 300.0f, 100.0f, LightType::POINT_LIGHT));
  testLight2 = &worldSystem->createWorldObject(component::Transform(lightPositions[0]));
  testLight2->addComponent<component::Light>(
      component::Light(glm::vec3(1.0f, 1.0f, 1.0f), 300.0f, 100.0f, LightType::POINT_LIGHT));
}

void App::renderLantern() {
  worldSystem->clearWorld();
  testLight1 = nullptr;
  testLight2 = nullptr;

  const auto &lanternModel = nameToMeshes["lantern_model"];
  world_system::WorldObject &lanternObject = worldSystem->createWorldObject(component::Transform(
      glm::vec3(0.0f, 6.42f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.50f)));
  lanternObject.addComponent<component::Model>({lanternModel.meshId, lanternModel.primIdToMatId});

  auto &lanternLight =
      worldSystem->createWorldObject(component::Transform(glm::vec3(-2.94f, 8.42f, -0.07f)));
  lanternLight.addComponent<component::Light>(
      component::Light(glm::vec3(0.737f, 0.341f, 0.125f), 300.0f, 300.0f, LightType::POINT_LIGHT));
}

void App::processInput(float dt) {
  bool keyW = input.getKey(Input::Key::W);
  bool keyA = input.getKey(Input::Key::A);
  bool keyD = input.getKey(Input::Key::D);
  bool keyS = input.getKey(Input::Key::S);

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
  SLOG("Starting command server.");
  //  commandServer.start();
  //  SLOG("Waiting for clients to connect...");
  //  CommandDto::RTSPConnection connectionDto =
  //  commandServer.popConnectionQueue();
  //  runRenderLoop(connectionDto.toRtspEndpoint());
  //  runRenderLoop("rtsp://0.0.0.0:8554/mystream");
  runRenderLoop("rec.mp4");
}

void App::runRenderLoop(std::string_view renderOutput) {
  display.showWindow();
  //  FrameQueue frameQueue;
  // create & start rtspClient
  //  RtspClient rtspClient(display.getWidth(), display.getHeight(), frameQueue,
  //                        renderOutput, true);
  //  assert(rtspClient.start());

  int envMap = 0;
  input.addKeyCallback(Input::Key::H, [&envMap, &renderSystem = renderSystem,
                                       &appUi = appUi](const Input::KeyEvent &keyEvent) {
    // change env
    if (keyEvent.action == Input::Action::PRESS) {
      Image skybox;
      switch (envMap) {
      case 0:
        Loaders::loadImage(skybox, "resources/skybox/outdoor.hdr", true);
        break;
      case 1:
        Loaders::loadImage(skybox, "resources/skybox/night.hdr", true);
        break;

      case 2:
        Loaders::loadImage(skybox, "resources/skybox/urban.hdr", true);
        break;

      default:
        Loaders::loadImage(skybox, "resources/skybox/indoor.hdr", true);
        envMap = -1;
      }
      envMap++;
      renderSystem->setSkyBox(&skybox);
      std::pair specularConv = renderSystem->getSpecularConvMap();
      std::pair diffuseConv = renderSystem->getDiffuseConvMap();
      std::pair envMap = renderSystem->getEnvMap();
      appUi.setDiffuseConvMap(diffuseConv.first, diffuseConv.second);
      appUi.setSpecularConvMap(specularConv.first, specularConv.second);
      appUi.setEnvMap(envMap.first, envMap.second);
    }
  });

  float ltf, lt, ct, dt = 0.0f;
  int frameCnt = 0;
  ltf = lt = ct = display.getTime(); // time in seconds
  while (!display.shouldClose()) {
    // calculate delta time
    ct = display.getTime();
    dt = ct - lt;
    lt = display.getTime();

    gui.newFrame(dt, input, display);
    appUi.show();

    // rotate light

    if (testLight1 != nullptr) {
      glm::vec3 lightPosition =
          glm::vec3(10 * cos(display.getTime()), 10, 10 * sin(display.getTime()));
      testLight1->getTransform().position(lightPosition);
    }

    if (testLight2 != nullptr) {
      glm::vec3 lightPosition =
          glm::vec3(16 * sin(display.getTime()), 10, 16 * cos(display.getTime()) - 10);
      testLight2->getTransform().position(lightPosition);
    }

    // calculate FPS
    if (ct - ltf >= 1) {
      appUi.addFps(frameCnt);
      frameCnt = 1;
      ltf = ct;
    } else {
      frameCnt++;
    }

    processInput(dt);
    worldSystem->update(dt);
    auto img = renderSystem->update(dt);

    // ui state update
    AppUi::EditorState editorState = appUi.getEditorState();
    renderSystem->setGridPlaneConfig(editorState.gridPlaneState.scale,
                                     editorState.gridPlaneState.showPlane);
    appUi.setCoordinateSpaceState({camera->getViewMatrix(), renderSystem->getProjectionMatrix(),
                                   glm::vec4(0.0f, 0.0f, renderWidth, renderHeight),
                                   camera->position});

    auto err = glGetError();
    if (err != GL_NO_ERROR) CSLOG("OpenGL ERROR:", err);
    display.update();
    input.update();
    if (appUi.getShouldClose()) display.setShouldClose(true);
  }
  CSLOG("Closing renderer..");
} // namespace app

App::~App() {
  DEBUG_SLOG("App destroyed.");
  delete renderSystem;
  delete camera;
  delete worldSystem;
}

render_system::RenderSystem *App::createRenderSystem(int width, int height) {
  /* Init RenderSystem */
  Image checkerImage, gridImage;
  bool status = Loaders::loadImage(checkerImage, "resources/defaults/checker.bmp");
  status = Loaders::loadImage(gridImage, "resources/defaults/grid.png");
  Buffer flatForwardVertex, flatForwardFragment, textureForwardVertex, textureForwardFragment,
      skyboxVertex, cubemapVertex, cubemapFragment, equirectangularFragment, visualPrepVertex,
      visualPrepFragment, iblConvolutionFragment, iblSpecularConvolutionFragment,
      iblBrdfIntegrationFragment, guiVertex, guiFragment, gridPlaneVertex, gridPlaneFragment;
  status = Loaders::loadBinaryFile(flatForwardVertex, "shaders/flat_forward_material_vert.spv");
  status = Loaders::loadBinaryFile(flatForwardFragment, "shaders/flat_forward_material_frag.spv");
  status =
      Loaders::loadBinaryFile(textureForwardVertex, "shaders/texture_forward_material_vert.spv");
  status =
      Loaders::loadBinaryFile(textureForwardFragment, "shaders/texture_forward_material_frag.spv");
  status = Loaders::loadBinaryFile(skyboxVertex, "shaders/skybox_vert.spv");
  status = Loaders::loadBinaryFile(cubemapVertex, "shaders/cubemap_vert.spv");
  status = Loaders::loadBinaryFile(cubemapFragment, "shaders/cubemap_frag.spv");
  status = Loaders::loadBinaryFile(equirectangularFragment, "shaders/equirectangular_frag.spv");
  status = Loaders::loadBinaryFile(visualPrepVertex, "shaders/visualprep_vert.spv");
  status = Loaders::loadBinaryFile(visualPrepFragment, "shaders/visualprep_frag.spv");
  status =
      Loaders::loadBinaryFile(iblConvolutionFragment, "shaders/ibl_diffuse_convolution_frag.spv");
  status = Loaders::loadBinaryFile(iblSpecularConvolutionFragment,
                                   "shaders/ibl_specular_convolution_frag.spv");
  status =
      Loaders::loadBinaryFile(iblBrdfIntegrationFragment, "shaders/ibl_brdf_integration_map.spv");
  status = Loaders::loadBinaryFile(guiVertex, "shaders/gui_vert.spv");
  status = Loaders::loadBinaryFile(guiFragment, "shaders/gui_frag.spv");
  status = Loaders::loadBinaryFile(guiVertex, "shaders/gui_vert.spv");
  status = Loaders::loadBinaryFile(guiFragment, "shaders/gui_frag.spv");
  status = Loaders::loadBinaryFile(gridPlaneVertex, "shaders/grid_plane_vert.spv");
  status = Loaders::loadBinaryFile(gridPlaneFragment, "shaders/grid_plane_frag.spv");

  return new RenderSystem(
      {gridImage, checkerImage,
       /**
        * const shader::StageCodeMap &flatForwardShader;
        * const shader::StageCodeMap &textureForwardShader; const shader::StageCodeMap
        * &skyboxShader; const shader::StageCodeMap &cubemapShader; const shader::StageCodeMap
        * &equirectangularShader; const shader::StageCodeMap &visualPrepShader; const
        * shader::StageCodeMap &iblConvolutionShader; const shader::StageCodeMap
        * &iblSpecularConvolutionShader; const shader::StageCodeMap &iblBrdfIntegrationShader;
        * guiVertex;
        * guiFragment
        */
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, flatForwardVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, flatForwardFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, textureForwardVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, textureForwardFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, skyboxVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, cubemapFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, cubemapFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, equirectangularFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, visualPrepVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, visualPrepFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, iblConvolutionFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, cubemapVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, iblSpecularConvolutionFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, visualPrepVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, iblBrdfIntegrationFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, guiVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, guiFragment}},
       shader::StageCodeMap{{shader::ShaderStage::VERTEX_SHADER, gridPlaneVertex},
                            {shader::ShaderStage::FRAGMENT_SHADER, gridPlaneFragment}},
       [&appUi = appUi](uint textureId, int width, int height) {
         appUi.showFrame(textureId, width, height);
       },
       width, height, width / (float)height} // namespace app
  );
}

} // namespace app
