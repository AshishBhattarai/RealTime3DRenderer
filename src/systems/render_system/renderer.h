#pragma once

#include "common.h"
#include "shaders/flat_forward_program.h"
#include "types.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace render_system {
struct Mesh;
struct RenderableEntity;
class Camera;

using RenderableMap = std::unordered_map<GLuint, std::vector<RenderableEntity>>;

class Renderer {
private:
  const std::vector<Mesh> &meshes;
  const RenderableMap &renderables;
  std::unordered_set<EntityId> removeables;
  glm::mat4 projectionMatrix;

  const Camera *camera;

  shader::FlatForwardProgram flatForwardShader;

public:
  Renderer(const std::vector<Mesh> &meshes, const RenderableMap &renderables,
           const Camera *camera);

  void render(float dt);
  void updateProjectionMatrix(float ar, float fov, float near, float far);

  void setCamera(const Camera *camera) { this->camera = camera; }
};
} // namespace render_system
