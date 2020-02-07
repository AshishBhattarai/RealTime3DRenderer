#pragma once

#include "ecs/common.h"
#include "ecs/system_manager.h"
#include "mesh.h"
#include "renderable_entity.h"
#include "renderer.h"

namespace tinygltf {
struct Model;
}

namespace render_system {
class Camera;

class RenderSystem : ecs::System<RenderSystem> {
private:
  static constexpr float DEFAULT_FOV = 75.0f;
  static constexpr float DEFAULT_NEAR = 0.1f;
  static constexpr float DEFAULT_FAR = 1000.0f;

  Renderer renderer;
  std::vector<Mesh> meshes;
  RenderableMap renderables;
  std::unordered_map<ecs::Entity, size_t> entityToIndex;

public:
  RenderSystem();

  void registerMesh(Mesh &&mesh) {
    meshes.emplace_back(std::move(mesh));
    renderables.emplace(std::pair(mesh.vao, std::vector<RenderableEntity>{}));
  }

  // TODO: Delete mesh and set all existing entites to default mesh
  bool unregisterMesh(std::string_view name);

  void updateProjectionMatrix(float ar, float fov = DEFAULT_FOV,
                              float near = DEFAULT_NEAR,
                              float far = DEFAULT_FAR) {
    renderer.updateProjectionMatrix(ar, fov, near, far);
  }

  void setCamera(const Camera *camera) {
    assert(camera && "Invalid camera supplied.");
    if (camera)
      renderer.setCamera(camera);
  }

  void update(float dt) { renderer.render(dt); }
};

} // namespace render_system
