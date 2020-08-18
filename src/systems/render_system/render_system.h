#pragma once

#include "ecs/common.h"
#include "ecs/system_manager.h"
#include "frame_buffer.h"
#include "point_light.h"
#include "post_processor.h"
#include "pre_processor.h"
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
  const shader::StageCodeMap &textureForwardShader;
  const shader::StageCodeMap &skyboxShader;
  const shader::StageCodeMap &cubemapShader;
  const shader::StageCodeMap &equirectangularShader;
  const shader::StageCodeMap &visualPrepShader;
  const shader::StageCodeMap &iblConvolutionShader;
  const shader::StageCodeMap &iblSpecularConvolutionShader;
  const shader::StageCodeMap &iblBrdfIntegrationShader;

  int width;
  int height;
  float ar;
};

struct ModelRegisterReturn {
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

/**
 * @brief The RenderSystem class
 * This mainly hold the GPU data(mesh, material) and connects renderer to other
 * systems
 *
 * Contains everything required to load & render a scene.
 * Not responsible for managing secens.
 * Managing multiple scene should be done through the app.
 */
class RenderSystem : NonCopyable, ecs::System<RenderSystem> {
private:
  class LightingSystem;
  static constexpr float DEFAULT_FOV = 75.0f;
  static constexpr float DEFAULT_NEAR = 0.1f;
  static constexpr float DEFAULT_FAR = 1000.0f;
  const bool status;

  PreProcessor preProcessor;
  Renderer renderer;
  PostProcessor postProcessor;
  FrameBuffer framebuffer;
  SceneLoader sceneLoader;

  std::unordered_map<MeshId, Mesh> meshes;
  std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>> materials;
  //  std::unordered_map<ecs::Entity, size_t> entityToIndex;

  ecs::Coordinator &coordinator;
  LightingSystem *lightingSystem;
  std::unique_ptr<Texture> skybox;
  std::unique_ptr<Texture> globalDiffuseIBL;
  std::unique_ptr<Texture> globalSpecularIBL;

  void initSubSystems();
  // init render_system related singletons
  bool initSingletons(const Image &checkerImage);

public:
  RenderSystem(const RenderSystemConfig &config);
  ~RenderSystem();

  // returns map of mesh name to id
  ModelRegisterReturn registerGltfModel(tinygltf::Model &modelData);

  // register a new material of type T
  template <typename T, typename... Args>
  MaterialId registerMaterial(ShaderType shaderType, Args... args) {
    auto id = sceneLoader.generateMaterialId();
    materials.emplace(id, std::unique_ptr<T>(new T{{id, shaderType}, args...}));
    return id;
  }
  // TODO: Delete mesh and set all existing entites to default mesh & material
  bool unregisterMesh(std::string_view name);
  /**
   * @brief setSkyBox, Also sets the image as globalDiffuseIBL
   * @param image
   * @return
   */
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
