#include "render_system.h"
#include "camera.h"
#include "components/mesh.h"
#include "components/transform.h"
#include "ecs/coordinator.h"
#include "utils/model_loader.h"
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
      it->second.emplace_back(RenderableEntity(transfrom, entity));
      entityToIndex.emplace(std::pair(entity, idx));
    } else {
      SLOG("Error entity with invalid mesh:", modelId, entity);
    }
  });

  connectEntityRemovedSignal([](const ecs::Entity &) {

  });
}
} // namespace render_system
