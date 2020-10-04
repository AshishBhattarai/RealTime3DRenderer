#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "core/image.h"
#include "default_primitives_renderer.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "shaders/general_vs_ubo.h"
#include "utils/slogger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

namespace render_system {

Renderer::Renderer(RendererConfig config)
    : meshes(config.meshes), materials(config.materials), projectionMatrix(1.0f),
      camera(config.camera), generalVSUBO(), flatForwardMaterial(config.flatForwardShader),
      textureForwardMaterial(config.textureForwardShader),
      skyboxCubeMapShader(config.skyboxCubeMapShader), gridPlaneShader(config.gridPlaneShape),
      brdfIntegrationMap(std::move(config.brdfIntegrationMap)) {

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::loadPointLight(const PointLight &pointLight, uint idx) {
  // TODO: Light to lit material fs ubo ??
  flatForwardMaterial.bind();
  flatForwardMaterial.loadPointLight(pointLight, idx);
  textureForwardMaterial.bind();
  textureForwardMaterial.loadPointLight(pointLight, idx);
}

void Renderer::loadPointLightCount(size_t count) {
  flatForwardMaterial.bind();
  flatForwardMaterial.loadPointLightSize(count);
  textureForwardMaterial.bind();
  textureForwardMaterial.loadPointLightSize(count);
}

void Renderer::preRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // TODO: global / gener ubos handled by render_system (data) ??
  generalVSUBO.setViewMatrix(camera->getViewMatrix());
  generalVSUBO.setCameraPos(camera->position);
}

void Renderer::preRenderMesh(const Texture &diffuseIbl, const Texture &specularIbl) {
  // TODO: env maps to lit material fs ubo ??
  flatForwardMaterial.bind();
  flatForwardMaterial.loadIrradianceMap(diffuseIbl);
  flatForwardMaterial.loadBrdfIntegrationMap(brdfIntegrationMap);
  flatForwardMaterial.loadPrefilteredMap(specularIbl);
  textureForwardMaterial.bind();
  textureForwardMaterial.loadIrradianceMap(diffuseIbl);
  textureForwardMaterial.loadBrdfIntegrationMap(brdfIntegrationMap);
  textureForwardMaterial.loadPrefilteredMap(specularIbl);
  textureForwardMaterial.unBind();
}

void Renderer::renderMesh(float, const glm::mat4 &transform, const MeshId &meshId,
                          std::map<PrimitiveId, MaterialId> primIdToMatId) {
  // fetch mesh
  const auto &mesh = meshes.at(meshId);
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    // bind primitive
    const Primitive &primitive = mesh.primitives[i];
    glBindVertexArray(primitive.vao);
    // set entity specific data
    const auto &material = materials.at(primIdToMatId[primitive.vao]);
    if (material->shaderType == ShaderType::FLAT_FORWARD_SHADER) {
      flatForwardMaterial.bind();
      flatForwardMaterial.loadMaterial(*static_cast<FlatMaterial *>(material.get()));
      flatForwardMaterial.loadTransformMatrix(transform);
    } else {
      textureForwardMaterial.bind();
      textureForwardMaterial.loadMaterial(*static_cast<TextureMaterial *>(material.get()));
      textureForwardMaterial.loadTransformMatrix(transform);
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

void Renderer::updateProjectionMatrix(float ar, float fov, float near, float far) {
  projectionMatrix = glm::perspective(glm::radians(fov), ar, near, far);
  generalVSUBO.setProjectionMatrix(projectionMatrix);
}

void Renderer::renderGridPlane() {
  gridPlaneShader.bind();
  DefaultPrimitivesRenderer::getInstance().drawPlane();
}

} // namespace render_system
