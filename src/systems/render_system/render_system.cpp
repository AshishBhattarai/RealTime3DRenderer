#include "render_system.h"
#include "camera.h"
#include "components/light.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "core/image.h"
#include "ecs/coordinator.h"
#include "events/component_cache_invalid.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "scene.h"
#include "utils/slogger.h"

namespace render_system {

/**
 * Sub-system which is used to register lights in RenderSystem
 */
class RenderSystem::LightingSystem : ecs::System<LightingSystem> {
  friend class RenderSystem;
};

class RenderSystem::EventListener
    : public ecs::Receiver<event::ComponentCacheInvalid<component::Transform>> {
private:
  std::function<void()> lightCacheInvalidCallback;

public:
  using TrasformCacheInvalidEvent =
      event::ComponentCacheInvalid<component::Transform>;
  using LightCacheInvalidEvent = event::ComponentCacheInvalid<component::Light>;

  EventListener(const std::function<void()> &lightCacheInvalidCallback)
      : lightCacheInvalidCallback(lightCacheInvalidCallback) {}

  void receive(const LightCacheInvalidEvent &) { lightCacheInvalidCallback(); }
};

void RenderSystem::initSubSystems(ecs::Coordinator &coordinator) {
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
  initSubSystems(coordinator);

  /*
   * Handle Lights(Entities with light component)
   */
  lightingSystem->connectEntityAddedSignal(
      [&coordinator = coordinator, &pointLights = pointLights/*,
       &entityToIndex = entityToIndex*/](const ecs::Entity &entity,
                                       const ecs::Signature &) {
    if (pointLights.size() >= shader::fragment::PointLight::MAX) {
      CSLOG("Error: Maximum point lights reached.");
      return;
    }
    const auto &transfrom =
        coordinator.getComponent<component::Transform>(entity);
    const auto &light = coordinator.getComponent<component::Light>(entity);
    PointLight pointLight(&transfrom.transformMat[3], &light.color,
                          &light.range, &light.intensity, entity);
    pointLights.push_back(pointLight);
      });

  lightingSystem->connectEntityRemovedSignal([](const ecs::Entity &) {
    // TODO
  });

  eventListener = new EventListener([&poinLights = pointLights,
                                     &coordinator = coordinator] {
    // light cache invalid
    for (auto &pointLight : poinLights) {
      const auto &transfrom =
          coordinator.getComponent<component::Transform>(pointLight.entityId);
      const auto &light =
          coordinator.getComponent<component::Light>(pointLight.entityId);
      pointLight.position = &transfrom.transformMat[3];
      pointLight.color = &light.color;
      pointLight.radius = &light.range;
      pointLight.intensity = &light.intensity;
    }
  });
}

RenderSystem::~RenderSystem() {
  delete lightingSystem;
  delete eventListener;
}

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
  renderer.preRender(this->pointLights);
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
