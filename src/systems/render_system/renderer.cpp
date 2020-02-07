#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include <glm/gtc/matrix_transform.hpp>

namespace render_system {

Renderer::Renderer(const std::vector<Mesh> &meshes,
                   const RenderableMap &renderables, const Camera *camera)
    : meshes(meshes), renderables(renderables), projectionMatrix(1.0f),
      camera(camera), flatForwardShader() {
  if (!camera) {
    camera = &RenderDefaults::getInstance().getCamera();
  }
}

void Renderer::render(float dt) {
  for (const Mesh &mesh : meshes) {
    const auto &entites = renderables.find(mesh.vao)->second;
    glBindVertexArray(mesh.vao);
    flatForwardShader.bind();
    flatForwardShader.loadViewMatrix(camera->getViewMatrix());
    for (const Primitive &primitive : mesh.primitives) {
      for (const RenderableEntity &entity : entites) {
        const auto &transform = entity.transform;
        // create transformation matrix
        glm::mat4 transformMat =
            glm::translate(glm::mat4(), entity.transform.position);
        transformMat =
            glm::rotate(transformMat, glm::radians(transform.rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
        transformMat =
            glm::rotate(transformMat, glm::radians(transform.rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
        transformMat =
            glm::rotate(transformMat, glm::radians(transform.rotation.z),
                        glm::vec3(1.0f, 0.0f, 1.0f));
        transformMat = glm::scale(transformMat, transform.scale);

        flatForwardShader.loadTransformMatrix(transformMat);
        glDrawElements(primitive.mode, primitive.count, GL_UNSIGNED_INT,
                       primitive.offset);
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
