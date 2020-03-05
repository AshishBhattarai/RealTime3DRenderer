#pragma once

#include "ecs/common.h"
#include "ecs/system_manager.h"
#include "frame_buffer.h"
#include "point_light.h"
#include "renderer.h"

namespace tinygltf {
struct Model;
}
namespace ecs {
class Coordinator;
}
class Image;
class Buffer;
namespace render_system {
class Camera;
class Model;

struct RenderSystemConfig {
  const Image &checkerImage; // can be removed after RenderSystem construction
  const shader::StageCodeMap &flatForwardShader;
  float ar;
  int width;
  int height;

  RenderSystemConfig(const Image &checkerImage,
                     const shader::StageCodeMap &flatForwardShader, float width,
                     float height)
      : checkerImage(checkerImage), flatForwardShader(flatForwardShader),
        ar(width / (float)height), width(width), height(height) {}
};

class RenderSystem : ecs::System<RenderSystem> {
private:
  class LightingSystem;
  class EventListener;
  static constexpr float DEFAULT_FOV = 75.0f;
  static constexpr float DEFAULT_NEAR = 0.1f;
  static constexpr float DEFAULT_FAR = 1000.0f;

  Renderer renderer;
  FrameBuffer frameBuffer;
  std::vector<Mesh> meshes;
  std::vector<PointLight> pointLights;
  RenderableMap renderables;
  std::unordered_map<ecs::Entity, size_t> entityToIndex;
  std::unordered_map<MeshId, size_t> meshToIndex;

  LightingSystem *lightingSystem;
  EventListener *eventListener;

  void initSubSystems(ecs::Coordinator &coordinator);

public:
  RenderSystem(const RenderSystemConfig &config);
  ~RenderSystem();

  /**
   * Moves all the meshes in give model to the render system.
   *
   * retunrs valid id(no-zero) on success.
   * 				 zero on failure.
   */
  uint registerMesh(Mesh &&mesh);
  // returns map of mesh name to id
  std::map<std::string, uint> registerMeshes(tinygltf::Model &modelData);
  /** Temp (TODO: Probably remove this) */
  void relaceAllMaterial(uint meshId, const BaseMaterial *material);

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

  Image &&update(float dt);
};
} // namespace render_system
