#include "render_system.h"
#include "camera.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "ecs/coordinator.h"
#include "model.h"
#include "utils/slogger.h"

namespace render_system {
RenderSystem::RenderSystem() : renderer(meshes, renderables, nullptr) {
  auto &coordinator = ecs::Coordinator::getInstance();
  using namespace component;

  /* Handle new entity that matches render system signature */
  connectEntityAddedSignal([&coordinator, &renderables = renderables,
                            &entityToIndex =
                                entityToIndex](const ecs::Entity &entity,
                                               const ecs::Signature &) {
    const auto &transfrom = coordinator.getComponent<Transform>(entity);
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
}

std::map<std::string, uint>
RenderSystem::registerMeshes(tinygltf::Model &modelData) {
  Model model(modelData);
  std::map<std::string, uint> ids;
  for (size_t i = 0; i < model.meshes.size(); ++i) {
    std::string name = model.meshes[i].name;
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
  id = meshes.back().primitives[0].vao + MODEL_ID_OFFSET;
  renderables.emplace(std::pair(id, std::vector<RenderableEntity>{}));
  return id;
}
} // namespace render_system
