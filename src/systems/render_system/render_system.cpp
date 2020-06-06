#include "render_system.h"
#include "camera.h"
#include "components/light.h"
#include "components/model.h"
#include "components/transform.h"
#include "core/image.h"
#include "ecs/coordinator.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "scene.h"
#include "shaders/config.h"
#include "utils/slogger.h"

namespace render_system {

/**
 * Sub-system which is used to register lights in RenderSystem
 */
class RenderSystem::LightingSystem : ecs::System<LightingSystem> {
  friend class RenderSystem;
};

/**
 * @brief RenderSystem::initSubSystems
 * Init sub-system to the render system.
 * i) LightSystem
 */
void RenderSystem::initSubSystems() {
  ecs::ComponentFamily lightFamily =
      coordinator.getComponentFamily<component::Light>();
  ecs::ComponentFamily transformFamily =
      coordinator.getComponentFamily<component::Transform>();
  ecs::Signature sig;
  sig.set(lightFamily, true);
  sig.set(transformFamily, true);
  coordinator.registerSystem<LightingSystem>(sig);
  sig.reset();
  lightingSystem = new LightingSystem();
}

RenderSystem::RenderSystem(const RenderSystemConfig &config)
    : renderer(config.width, config.height, meshes, materials,
               &RenderDefaults::getInstance(&config.checkerImage).getCamera(),
               config.flatForwardShader),
      sceneLoader(), coordinator(ecs::Coordinator::getInstance()) {
  pointLights.reserve(shader::fragment::PointLight::MAX);
  updateProjectionMatrix(config.ar);
  /* load default materials */
  auto &renderDefaults = RenderDefaults::getInstance();
  materials.emplace(DEFAULT_MATERIAL_ID,
                    std::unique_ptr<Material>(new Material(
                        {{DEFAULT_MATERIAL_ID, ShaderType::FORWARD_SHADER},
                         renderDefaults.getCheckerTexture(),
                         renderDefaults.getBlackTexture(),
                         renderDefaults.getBlackTexture(),
                         renderDefaults.getBlackTexture(),
                         renderDefaults.getBlackTexture()})));
  materials.emplace(DEFAULT_FLAT_MATERIAL_ID,
                    std::unique_ptr<FlatMaterial>(new FlatMaterial(
                        {BaseMaterial{DEFAULT_FLAT_MATERIAL_ID,
                                      ShaderType::FLAT_FORWARD_SHADER},
                         glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
                         glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0, 1.0f})));

  /* Register & init helper systems(sub-systems) */
  initSubSystems();

  /*
   * Handle Lights(Entities with light component)
   */
  lightingSystem->connectEntityRemovedSignal([](const ecs::Entity &) {
    // TODO
  });
}

RenderSystem::~RenderSystem() { delete lightingSystem; }

SceneRegisterReturn
RenderSystem::registerGltfScene(tinygltf::Model &modelData) {
  auto sceneData = sceneLoader.loadScene(modelData);
  std::vector<MeshId> ids;
  std::vector<uint> numPrimitives;
  for (size_t i = 0; i < sceneData.meshes.size(); ++i) {
    std::string name = sceneData.meshNames[i];
    uint meshId = sceneData.meshes[i].id;
    meshes.emplace(meshId, sceneData.meshes[i]);
    numPrimitives.emplace_back(sceneData.meshes[i].primitives.size());
    ids.emplace_back(meshId);
  }
  for (auto &mat : sceneData.materials) {
    materials.emplace(mat->id, std::move(mat));
  }
  return {sceneData.name,           ids,
          sceneData.meshNames,      numPrimitives,
          sceneData.hasTexCoords,   sceneData.primIdToMatId,
          sceneData.matIdToNameList};
}

std::shared_ptr<Image> RenderSystem::update(float dt) {
  // load preRender data
  renderer.preRender();

  // load lights
  uint i = 0;
  for (EntityId entity : lightingSystem->getEntites()) {
    if (i == shader::fragment::PointLight::MAX)
      break;
    const auto &transfrom =
        coordinator.getComponent<component::Transform>(entity);
    const auto &light = coordinator.getComponent<component::Light>(entity);
    PointLight pointLight(&transfrom.transformMat[3], &light.color,
                          &light.range, &light.intensity, entity);
    renderer.loadPointLight(pointLight, i);
    ++i;
  }
  renderer.loadPointLightCount(i);

  // render entites
  for (EntityId entity : this->getEntites()) {
    auto transform =
        coordinator.getComponent<component::Transform>(entity).transformMat;
    auto model = coordinator.getComponent<component::Model>(entity);
    renderer.render(dt, transform, model.meshId, model.primIdToMatId);
  }
  std::shared_ptr<Image> img = renderer.readPixels();
  // Don't blit before reading the pixels.
  //  renderer.blitToWindow();
  return img;
}

} // namespace render_system
