#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "core/image.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "shaders/general_vs_ubo.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "default_primitives_renderer.h"
#include <limits>

namespace render_system {

Renderer::Renderer(RendererConfig config)
    : meshes(config.meshes), materials(config.materials),
      projectionMatrix(1.0f), camera(config.camera), generalVSUBO(),
      flatForwardMaterial(config.flatForwardShader),
      skyboxCubeMapShader(config.skyboxCubeMapShader),
      brdfIntegrationMap(std::move(config.brdfIntegrationMap)) {

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::loadPointLight(const PointLight &pointLight, uint idx) {
  flatForwardMaterial.bind();
  flatForwardMaterial.loadPointLight(pointLight, idx);
}

void Renderer::loadPointLightCount(size_t count) {
  flatForwardMaterial.loadPointLightSize(count);
}

void Renderer::preRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // TODO: global / gener ubos handled by render_system (data) ??
  generalVSUBO.setViewMatrix(camera->getViewMatrix());
  generalVSUBO.setCameraPos(camera->position);
}

void Renderer::preRenderMesh(const Texture &diffuseIbl,
                             const Texture &specularIbl) {
  flatForwardMaterial.bind();
  flatForwardMaterial.loadIrradianceMap(diffuseIbl);
  flatForwardMaterial.loadBrdfIntegrationMap(brdfIntegrationMap);
  flatForwardMaterial.loadPrefilteredMap(specularIbl);
}

void Renderer::renderMesh(float, const glm::mat4 &transform,
                          const MeshId &meshId,
                          std::map<PrimitiveId, MaterialId> primIdToMatId) {
  // fetch mesh
  const auto &mesh = meshes.at(meshId);
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    // bind primitive
    const Primitive &primitive = mesh.primitives[i];
    glBindVertexArray(primitive.vao);
    // set entity specific data
    const auto &material = materials.at(primIdToMatId[i]);
    if (material->shaderType == ShaderType::FLAT_FORWARD_SHADER) {
      flatForwardMaterial.bind();
      flatForwardMaterial.loadMaterial(
          *static_cast<FlatMaterial *>(material.get()));
      flatForwardMaterial.loadTransformMatrix(transform);
    }
    // draw
    glDrawElements(primitive.mode, primitive.indexCount, primitive.indexType,
                   primitive.indexOffset);
  }
}

void Renderer::renderSkybox(const Texture &texture) {
  // render skybox
  glDepthFunc(GL_LEQUAL);
  skyboxCubeMapShader.bind();
  skyboxCubeMapShader.bindTexture(texture);
  DefaultPrimitivesRenderer::getInstance().drawCube();
  glDepthFunc(GL_LESS);
}

void Renderer::updateProjectionMatrix(float ar, float fov, float near,
                                      float far) {
  projectionMatrix = glm::perspective(glm::radians(fov), ar, near, far);
  generalVSUBO.setProjectionMatrix(projectionMatrix);
}

} // namespace render_system
