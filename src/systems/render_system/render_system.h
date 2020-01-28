#pragma once

#include "ecs/system_manager.h"
#include "mesh.h"
#include "renderable_entity.h"
#include <unordered_map>

namespace tinygltf {
struct Model;
}

namespace render_system {
class RenderSystem : ecs::System<RenderSystem> {
public:
  RenderSystem();

  void registerMesh(const Mesh &model);
  bool unregisterMesh(std::string_view name);

private:
  std::vector<Mesh> meshes;
  std::unordered_map<std::string, std::vector<RenderableEntity>> renderables;
};

} // namespace render_system
