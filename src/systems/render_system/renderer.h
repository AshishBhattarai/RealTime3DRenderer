#pragma once

#include "common.h"
#include "frame_buffer.h"
#include "shaders/flat_forward_program.h"
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

using RenderableMap = std::unordered_map<MeshId, std::vector<RenderableEntity>>;

class Renderer {
private:
  FrameBuffer frameBuffer;
  const std::unordered_map<MeshId, Mesh> &meshes;
  const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
      &materials;
  const RenderableMap &renderables;
  const std::vector<PointLight> &pointLights;
  //  std::unordered_set<EntityId> removeables;
  glm::mat4 projectionMatrix;

  const Camera *camera;

  shader::FlatForwardProgram flatForwardShader;

public:
  Renderer(int width, int height,
           const std::unordered_map<MeshId, Mesh> &meshes,
           const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
               &materials,
           const RenderableMap &renderables,
           const std::vector<PointLight> &pointLights, const Camera *camera,
           const shader::StageCodeMap &flatForwardShader);

  void render(float dt);
  void blitToWindow();
  std::shared_ptr<Image> readPixels();
  void updateProjectionMatrix(float ar, float fov, float near, float far);

  void setCamera(const Camera *camera) { this->camera = camera; }
};
} // namespace render_system
