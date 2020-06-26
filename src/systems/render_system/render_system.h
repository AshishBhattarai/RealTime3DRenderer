#pragma once

#include "ecs/common.h"
#include "ecs/system_manager.h"
#include "frame_buffer.h"
#include "point_light.h"
#include "post_processor.h"
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
  const shader::StageCodeMap &skyboxShader;
  const shader::StageCodeMap &skyboxCubeMapShader;
  const shader::StageCodeMap &visualPrepShader;
  int width;
  int height;
  float ar;
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
  static constexpr float DEFAULT_FOV = 75.0f;
  static constexpr float DEFAULT_NEAR = 0.1f;
  static constexpr float DEFAULT_FAR = 1000.0f;

  Renderer renderer;
  PostProcessor postProcessor;
  FrameBuffer framebuffer;
  SceneLoader sceneLoader;

  std::unordered_map<MeshId, Mesh> meshes;
  std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>> materials;
  //  std::unordered_map<ecs::Entity, size_t> entityToIndex;
  std::unordered_map<MeshId, size_t> meshIdToIndex;
  std::unordered_map<MeshId, size_t> materialIdToIndex;

  ecs::Coordinator &coordinator;
  LightingSystem *lightingSystem;
  std::unique_ptr<Texture> skybox;

  void initSubSystems();

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
  bool setSkyBox(Image *image);
  std::shared_ptr<Image> update(float dt);

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
};
} // namespace render_system
