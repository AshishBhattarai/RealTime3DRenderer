#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "core/image.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include <glm/gtc/matrix_transform.hpp>

namespace render_system {

Renderer::Renderer(
    int width, int height, const std::unordered_map<MeshId, Mesh> &meshes,
    const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
        &materials,
    const RenderableMap &renderables,
    const std::vector<PointLight> &pointLights, const Camera *camera,
    const shader::StageCodeMap &flatForwardShader)
    : frameBuffer(width, height), meshes(meshes), materials(materials),
      renderables(renderables), pointLights(pointLights),
      projectionMatrix(1.0f), camera(camera),
      flatForwardShader(flatForwardShader) {

  // Setup framebuffer
  frameBuffer.use();
  frameBuffer.setColorAttachmentRB(GL_RGB);
  frameBuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Renderer::render(float) {
  //  frameBuffer.use();
  FrameBuffer::useDefault();
  frameBuffer.clearBuffer();
  flatForwardShader.bind();
  flatForwardShader.loadViewMatrix(camera->getViewMatrix());
  flatForwardShader.loadCameraPosition(camera->position);
  for (uint i = 0; i < pointLights.size(); ++i) {
    flatForwardShader.loadPointLight(pointLights[i], i);
  }
  flatForwardShader.loadPointLightSize(pointLights.size());
  for (auto renderable : renderables) {
    // fetch entites and mesh
    const auto &entites = renderable.second;
    const auto &mesh = meshes.at(renderable.first);
    for (size_t i = 0; i < mesh.primitives.size(); ++i) {
      // bind primitive
      const Primitive &primitive = mesh.primitives[i];
      glBindVertexArray(primitive.vao);
      for (const RenderableEntity &entity : entites) {
        // set entity specific data
        const auto &material = materials.at(entity.primIdToMatId.at(i));
        if (material->shaderType == ShaderType::FLAT_FORWARD_SHADER)
          flatForwardShader.loadMaterial(
              *static_cast<FlatMaterial *>(material.get()));
        flatForwardShader.loadTransformMatrix(*entity.transform);
        // draw
        glDrawElements(primitive.mode, primitive.indexCount,
                       primitive.indexType, primitive.indexOffset);
      }
    }
  }
}

void Renderer::blitToWindow() { frameBuffer.blit(nullptr, GL_BACK); }

std::shared_ptr<Image> Renderer::readPixels() {
  int width = 0;
  int height = 0;
  int numChannels = 0;
  Buffer buffer = FrameBuffer::readPixelsWindow(numChannels, width, height);
  return std::make_shared<Image>(std::move(buffer), width, height, numChannels);
}

void Renderer::updateProjectionMatrix(float ar, float fov, float near,
                                      float far) {
  projectionMatrix = glm::perspective(glm::radians(fov), ar, near, far);
  flatForwardShader.bind();
  flatForwardShader.loadProjectionMatrix(projectionMatrix);
  flatForwardShader.unBind();
}

} // namespace render_system
