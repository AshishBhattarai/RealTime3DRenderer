#pragma once

#include "common.h"
#include "frame_buffer.h"
#include "shaders/flat_forward_program.h"
#include "shaders/general_vs_ubo.hpp"
#include "shaders/skybox_shader.h"
#include "types.h"
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
  shader::SkyboxShader skyboxShader;
  const GLuint cube;

public:
  Renderer(int width, int height,
           const std::unordered_map<MeshId, Mesh> &meshes,
           const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
               &materials,
           const Camera *camera, const shader::StageCodeMap &flatForwardShader,
           const shader::StageCodeMap &skyboxShader);

  void loadPointLight(const PointLight &pointLight, uint idx);
  void loadPointLightCount(size_t count);
  void preRender();
  void render(float dt, const glm::mat4 &transform, const MeshId &meshId,
              std::map<PrimitiveId, MaterialId> primIdToMatId);
  void renderSkybox(const Texture &texture);
  void blitToWindow();
  std::shared_ptr<Image> readPixels();
  void updateProjectionMatrix(float ar, float fov, float near, float far);

  void setCamera(const Camera *camera) { this->camera = camera; }
};
} // namespace render_system
