#pragma once

#include "common.h"
#include "frame_buffer.h"
#include "shaders/flat_forward_program.h"
#include "shaders/general_vs_ubo.hpp"
#include "shaders/ibl_specular_convolution.h"
#include "shaders/skybox_shader.h"
#include "types.h"
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Buffer;
class Image;
namespace render_system {
struct Mesh;
struct RenderableEntity;
struct PointLight;
class Camera;
class Texture;

struct RendererConfig {
  const int width;
  const int height;
  const std::unordered_map<MeshId, Mesh> &meshes;
  const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
      &materials;
  const Camera *camera;
  const shader::StageCodeMap &forwardShaderCode;
  const shader::StageCodeMap &skyboxEquiShaderCode; // equirectangularMap
  const shader::StageCodeMap &skyboxCubeMapShader;
  /* These will go to pre-processor  */
  const shader::StageCodeMap &iblConvolutionShader;
  const shader::StageCodeMap &iblSpecularConvolutionShader;
  const shader::StageCodeMap &iblBrdfIntegrationShader;
};

class Renderer {
private:
  FrameBuffer frameBuffer;
  const std::unordered_map<MeshId, Mesh> &meshes;
  const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
      &materials;
  glm::mat4 projectionMatrix;
  const Camera *camera;

  shader::GeneralVSUBO generalVSUBO;
  shader::FlatForwardProgram flatForwardShader;
  // TODO: Rename SkyboxShader to CubeMap/EnvMap shader.
  shader::SkyboxShader skyboxCubeMapShader;
  shader::SkyboxShader iblConvolutionShader;
  shader::IBLSpecularConvolution iblSpecularConvolutionShader;
  shader::Program iblBrdfIntegrationShader;

  // shape vaos
  const GLuint cube;
  const GLuint plane;

  Texture brdfIntegrationMap;

public:
  Renderer(int width, int height,
           const std::unordered_map<MeshId, Mesh> &meshes,
           const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
               &materials,
           const Camera *camera, const shader::StageCodeMap &flatForwardShader,
           const shader::StageCodeMap &skyboxCubeMapShader,
           const shader::StageCodeMap &iblConvolutionShader,
           const shader::StageCodeMap &iblSpecularConvolutionShader,
           const shader::StageCodeMap &iblBrdfIntegrationShader);

  void loadPointLight(const PointLight &pointLight, uint idx);
  void loadPointLightCount(size_t count);
  void preRender();
  void preRenderMesh(const Texture &diffuseIbl, const Texture &specularIbl);
  void renderMesh(float dt, const glm::mat4 &transform, const MeshId &meshId,
                  std::map<PrimitiveId, MaterialId> primIdToMatId);
  void renderSkybox(const Texture &texture);

  Texture renderToCubeMap(int width, int height, uint maxMipLevels,
                          bool genMipMap,
                          std::function<void(uint mipLevel)> drawCall);
  Texture equiTriangularToCubeMap(const Texture &equiTriangular);
  /**
   * @brief convoluteCubeMap for diffuse IBL
   * @param cubeMap
   * @param diffuse - true for diffuse, false for specular
   * @return
   */
  Texture convoluteCubeMap(const Texture &cubeMap, bool diffuse);

  /**
   * @brief generateBDRFIntegrationMap - Generates a brdf integration map.
   * @return map in 2D texture
   */
  Texture generateBRDFIntegrationMap();

  void blitToWindow();
  std::shared_ptr<Image> readPixels();
  void updateProjectionMatrix(float ar, float fov, float near, float far);
  void setCamera(const Camera *camera) { this->camera = camera; }
};
} // namespace render_system
