#pragma once

#include "ecs/common.h"
#include "ecs/system_manager.h"
#include "frame_buffer.h"
#include "point_light.h"
#include "renderer.h"
#include "scene.h"

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

struct SceneRegisterReturn {
  const std::string sceneName;

  /**
   * All these vectors elements belong to a same mesh.
   * IE: meshIds[i] <> meshNames <> ........... <> matIdToNameMap
   */
  const std::vector<MeshId> meshIds;
  const std::vector<std::string> meshNames;
  const std::vector<uint> numPrimitives;
  const std::vector<bool> hasTexCoords;
  const std::vector<std::map<PrimitiveId, MaterialId>> primIdToMatId;
  const std::vector<std::map<MaterialId, std::string>> matIdToNameMap;
};

class RenderSystem : ecs::System<RenderSystem> {
private:
  class LightingSystem;
  class EventListener;
  static constexpr float DEFAULT_FOV = 75.0f;
  static constexpr float DEFAULT_NEAR = 0.1f;
  static constexpr float DEFAULT_FAR = 1000.0f;

  Renderer renderer;
  SceneLoader sceneLoader;

  std::unordered_map<MeshId, Mesh> meshes;
  std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>> materials;
  std::vector<PointLight> pointLights;
  //  std::unordered_map<ecs::Entity, size_t> entityToIndex;
  std::unordered_map<MeshId, size_t> meshIdToIndex;
  std::unordered_map<MeshId, size_t> materialIdToIndex;

  ecs::Coordinator &coordinator;
  LightingSystem *lightingSystem;
  EventListener *eventListener;

  void initSubSystems(ecs::Coordinator &coordinator);

public:
  RenderSystem(const RenderSystemConfig &config);
  ~RenderSystem();

  // returns map of mesh name to id
  SceneRegisterReturn registerGltfScene(tinygltf::Model &modelData);
  template <typename T, typename... Args>
  MaterialId registerMaterial(ShaderType shaderType, Args... args) {
    auto id = sceneLoader.generateMaterialId();
    materials.emplace(id, std::unique_ptr<T>(new T{{id, shaderType}, args...}));
    return id;
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

  std::shared_ptr<Image> update(float dt);
};
} // namespace render_system
