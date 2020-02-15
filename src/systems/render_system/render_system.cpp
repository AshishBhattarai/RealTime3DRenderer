#include "render_system.h"
#include "camera.h"
#include "components/light.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "ecs/coordinator.h"
#include "model.h"
#include "render_defaults.h"
#include "utils/image.h"
#include "utils/slogger.h"

namespace render_system {

/**
 * Sub-system which is used to register lights in RenderSystem
 */
class RenderSystem::LightingSystem : ecs::System<LightingSystem> {
  friend class RenderSystem;
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
    : renderer(meshes, renderables,
               &RenderDefaults::getInstance(&config.checkerImage).getCamera(),
               config.flatForwardShader) {
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
      size_t idx = it->second.size();
      it->second.emplace_back(RenderableEntity(transfrom.transformMat, entity));
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
      [&coordinator](const ecs::Entity &entity, const ecs::Signature &) {

      });

  lightingSystem->connectEntityRemovedSignal([](const ecs::Entity &) {
    // TODO
  });
}

RenderSystem::~RenderSystem() { delete lightingSystem; }

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
  meshToIndex.emplace(std::pair(id, meshes.size()));
  renderables.emplace(std::pair(id, std::vector<RenderableEntity>{}));
  return id;
}
} // namespace render_system
