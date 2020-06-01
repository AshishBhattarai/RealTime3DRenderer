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
  std::function<void()> transformCacheInvalidCallback;
  std::function<void()> lightCacheInvalidCallback;

public:
  using TrasformCacheInvalidEvent =
      event::ComponentCacheInvalid<component::Transform>;
  using LightCacheInvalidEvent = event::ComponentCacheInvalid<component::Light>;

  EventListener(const std::function<void()> &transformCacheInvalidCallback,
                const std::function<void()> &lightCacheInvalidCallback)
      : transformCacheInvalidCallback(transformCacheInvalidCallback),
        lightCacheInvalidCallback(lightCacheInvalidCallback) {}

  void receive(const TrasformCacheInvalidEvent &) {
    transformCacheInvalidCallback();
  }

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
    : renderer(config.width, config.height, meshes, materials, renderables,
               pointLights,
               &RenderDefaults::getInstance(&config.checkerImage).getCamera(),
               config.flatForwardShader),
      sceneLoader() {
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
  auto &coordinator = ecs::Coordinator::getInstance();
  initSubSystems(coordinator);

  /* Handle new entity that matches render system signature */
  connectEntityAddedSignal([&coordinator, &renderables = renderables,
                            &meshes = meshes,
                            &materials = materials](const ecs::Entity &entity,
                                                    const ecs::Signature &) {
    // extract transform and model componenet
    const auto &transfrom =
        coordinator.getComponent<component::Transform>(entity);
    const auto &model = coordinator.getComponent<component::Model>(entity);
#ifndef NDEBUG
    // check if model entity is valid
    if (meshes.find(model.meshId) != meshes.end()) { // .contains c++20
      size_t primSize = 1;
      if (primSize != model.primIdToMatId.size()) {
        SLOG("All the primitives must be mapped to materials:", model.meshId,
             entity);
        return;
      }
      for (const auto &primToMat : model.primIdToMatId) {
        if (materials.find(primToMat.second) == materials.end()) {
          SLOG("Invalid materialId:", primToMat.second, entity);
          return;
        }
        if (primToMat.first >= primSize) {
          SLOG("Invalid primitiveId:", primToMat.first, entity);
          return;
        }
      }
    } else {
      SLOG("Invalid meshId:", model.meshId, entity);
      return;
    }
#endif
    // register new renderableEntity
    RenderableEntity renderableEntity = {entity, model.primIdToMatId,
                                         &transfrom.transformMat};
    if (renderables.find(model.meshId) != renderables.end())
      renderables[model.meshId].emplace_back(renderableEntity);
    else
      renderables[model.meshId] =
          std::vector<RenderableEntity>{renderableEntity};
  });

  connectEntityRemovedSignal([](const ecs::Entity &) {
    // TODO
  });

  /*
   * Handle Lights(Entities with light component)
   */
  lightingSystem->connectEntityAddedSignal(
      [&coordinator, &pointLights = pointLights/*,
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

  eventListener = new EventListener(
      // Transform cache invalid
      [&renderables = renderables, &coordinator] {
        for (auto it = renderables.begin(); it != renderables.end(); ++it) {
          for (auto &renderableEntity : it->second) {
            renderableEntity.transform =
                &coordinator
                     .getComponent<component::Transform>(
                         renderableEntity.entityId)
                     .transformMat;
          }
        }
      },
      [&poinLights = pointLights, &coordinator] {
        // light cache invalid
        for (auto &pointLight : poinLights) {
          const auto &transfrom =
              coordinator.getComponent<component::Transform>(
                  pointLight.entityId);
          const auto &light =
              coordinator.getComponent<component::Light>(pointLight.entityId);
          pointLight.position = &transfrom.transformMat[3];
          pointLight.color = &light.color;
          pointLight.radius = &light.range;
          pointLight.intensity = &light.intensity;
        }
      });

  coordinator.subscribeToEvent<EventListener::TrasformCacheInvalidEvent>(
      *eventListener);
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
  renderer.render(dt);
  std::shared_ptr<Image> img = renderer.readPixels();
  // Don't blit before reading the pixels.
  //  renderer.blitToWindow();
  return img;
}

} // namespace render_system
