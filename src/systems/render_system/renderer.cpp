#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include <glm/gtc/matrix_transform.hpp>

namespace render_system {

Renderer::Renderer(const std::vector<Mesh> &meshes,
                   const RenderableMap &renderables, const Camera *camera,
                   const shader::StageCodeMap &flatForwardShader)
    : meshes(meshes), renderables(renderables), projectionMatrix(1.0f),
      camera(camera), flatForwardShader(flatForwardShader) {
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
}

void Renderer::render(float dt) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  flatForwardShader.bind();
  flatForwardShader.loadViewMatrix(camera->getViewMatrix());
  for (const Mesh &mesh : meshes) {
    const auto &entites =
        renderables.find(mesh.primitives[0].vao + MODEL_ID_OFFSET)->second;
    for (const Primitive &primitive : mesh.primitives) {
      glBindVertexArray(primitive.vao);
      for (const RenderableEntity &entity : entites) {
        flatForwardShader.loadTransformMatrix(entity.transform);
        glDrawElements(primitive.mode, primitive.indexCount,
                       primitive.indexType, primitive.indexOffset);
      }
    }
  }
}

void Renderer::updateProjectionMatrix(float ar, float fov, float near,
                                      float far) {
  projectionMatrix = glm::perspective(glm::radians(fov), ar, near, far);
  flatForwardShader.bind();
  flatForwardShader.loadProjectionMatrix(projectionMatrix);
  flatForwardShader.unBind();
}

} // namespace render_system
