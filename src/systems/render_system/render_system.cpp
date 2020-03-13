#include "render_system.h"
#include "camera.h"
#include "components/light.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "ecs/coordinator.h"
#include "events/component_cache_invalid.h"
#include "model.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "utils/image.h"
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
    : renderer(config.width, config.height, meshes, renderables, pointLights,
               &RenderDefaults::getInstance(&config.checkerImage).getCamera(),
               config.flatForwardShader) {
  pointLights.reserve(shader::fragment::PointLight::MAX);
  updateProjectionMatrix(config.ar);
  auto &coordinator = ecs::Coordinator::getInstance();
  /* Register & init helper systems(sub-systems) */
  initSubSystems(coordinator);

  /* Handle new entity that matches render system signature */
  connectEntityAddedSignal([&coordinator, &renderables = renderables,
                            &entityToIndex =
                                entityToIndex](const ecs::Entity &entity,
                                               const ecs::Signature &) {
    const auto &transfrom =
        coordinator.getComponent<component::Transform>(entity);
    unsigned int modelId =
        coordinator.getComponent<component::Mesh>(entity).modelId;
    auto it = renderables.find(modelId);
    assert(modelId && it != renderables.end() && "Entity with invalid mesh");
    if (it != renderables.end()) {
      size_t idx = it->second.size() - 1;
      it->second.emplace_back(
          RenderableEntity(&transfrom.transformMat, entity));
      entityToIndex.emplace(std::pair(entity, idx));
    } else {
      SLOG("Error entity with invalid mesh:", modelId, entity);
    }
  });

  connectEntityRemovedSignal([](const ecs::Entity &) {
    // TODO
  });

  /*
   * Handle Lights(Entities with light component)
   */
  lightingSystem->connectEntityAddedSignal(
      [&coordinator, &pointLights = pointLights,
       &entityToIndex = entityToIndex](const ecs::Entity &entity,
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
        entityToIndex.emplace(std::pair(entity, pointLights.size()));
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

std::map<std::string, uint>
RenderSystem::registerMeshes(tinygltf::Model &modelData) {
  Model model(modelData);
  std::map<std::string, uint> ids;
  for (size_t i = 0; i < model.meshes.size(); ++i) {
    std::string name = model.names[i];
    uint id = registerMesh(std::move(model.meshes[i]));
    ids.emplace(std::pair(name, id));
  }
  return ids;
}

uint RenderSystem::registerMesh(Mesh &&mesh) {
  if (!mesh.isValid())
    return 0;
  uint id = 0;
  meshes.emplace_back(std::move(mesh));
  id = meshes.back().primitives[0].vao + MESH_ID_OFFSET;
  meshToIndex.emplace(std::pair(id, meshes.size() - 1));
  renderables.emplace(std::pair(id, std::vector<RenderableEntity>{}));
  return id;
}

void RenderSystem::relaceAllMaterial(uint meshId,
                                     const BaseMaterial *material) {
  size_t idx = meshToIndex[meshId];
  for (auto &primitive : meshes[idx].primitives) {
    if (material->shaderType == ShaderType::FLAT_FORWARD_SHADER) {
      auto *mat = static_cast<const FlatMaterial *>(material);
      std::unique_ptr<FlatMaterial> newMaterial =
          std::make_unique<FlatMaterial>();
      newMaterial->shaderType = ShaderType::FLAT_FORWARD_SHADER;
      newMaterial->ao = mat->ao;
      newMaterial->albedo = mat->albedo;
      newMaterial->emission = mat->emission;
      newMaterial->roughtness = mat->roughtness;
      newMaterial->metallic = mat->metallic;
      primitive.material = std::move(newMaterial);
    } else {
      assert(false && "TODO");
    }
  }
}

std::shared_ptr<Image> RenderSystem::update(float dt) {
  renderer.render(dt);
  auto img = renderer.readPixels();
  // Don't blit before reading the pixels.
  //  renderer.blitToWindow();
  return img;
}

} // namespace render_system
