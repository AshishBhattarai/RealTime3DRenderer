#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "core/image.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "shaders/general_vs_ubo.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace render_system {

Renderer::Renderer(
    int width, int height, const std::unordered_map<MeshId, Mesh> &meshes,
    const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
        &materials,
    const Camera *camera, const shader::StageCodeMap &flatForwardShader,
    const shader::StageCodeMap &skyboxShader,
    const shader::StageCodeMap &skyboxCubeMapShader,
    const shader::StageCodeMap &iblConvolutionShader)
    : frameBuffer(width, height), meshes(meshes), materials(materials),
      projectionMatrix(1.0f), camera(camera), generalVSUBO(),
      flatForwardShader(flatForwardShader), skyboxShader(skyboxShader),
      skyboxCubeMapShader(skyboxCubeMapShader),
      iblConvolutionShader(iblConvolutionShader),
      cube(RenderDefaults::getInstance().getCubeVao()) {

  // Setup framebuffer
  //  frameBuffer.use();
  //  frameBuffer.setColorAttachmentRB(GL_RGB);
  //  frameBuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Renderer::loadPointLight(const PointLight &pointLight, uint idx) {
  flatForwardShader.bind();
  flatForwardShader.loadPointLight(pointLight, idx);
}

void Renderer::loadPointLightCount(size_t count) {
  flatForwardShader.loadPointLightSize(count);
}

void Renderer::preRender(const Texture &diffuseIbl) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  generalVSUBO.setViewMatrix(camera->getViewMatrix());
  generalVSUBO.setCameraPos(camera->position);
  flatForwardShader.bind();
  diffuseIbl.unBind();
  glActiveTexture(GL_TEXTURE0 + shader::Program::TEX_UNIT_DIFF_IBL);
  diffuseIbl.bind();
  glActiveTexture(GL_TEXTURE0);
}

void Renderer::render(float, const glm::mat4 &transform, const MeshId &meshId,
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
      flatForwardShader.bind();
      flatForwardShader.loadMaterial(
          *static_cast<FlatMaterial *>(material.get()));
      flatForwardShader.loadTransformMatrix(transform);
    }
    // draw
    glDrawElements(primitive.mode, primitive.indexCount, primitive.indexType,
                   primitive.indexOffset);
  }
}

void Renderer::renderSkybox(const Texture &texture) {
  // render skybox
  glDepthFunc(GL_LEQUAL);
  glActiveTexture(GL_TEXTURE0 + skyboxShader.textureUnit);
  if (texture.getTarget() == GL_TEXTURE_CUBE_MAP) {
    skyboxCubeMapShader.bind();
    skyboxCubeMapShader.bindTexture(texture);
  } else {
    skyboxShader.bind();
    skyboxShader.bindTexture(texture);
  }
  glBindVertexArray(cube);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glDepthFunc(GL_LESS);
}

Texture Renderer::renderToCubeMap(int width, int height,
                                  std::function<void()> drawCall) {
  // setup data
  /**
   * you can aslo do this by rotation camera for each face too.
   * Note: All the caputreViews are upside down, to create upside down cubemap.
   */
  glm::mat4 caputureViews[] = {
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)), // +X
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)), // -X
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f)), // +Y
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f)), // -Y
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f)), // +Z
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f))}; // -Z
  glm::mat4 projection =
      glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f); // cube ar 1:1

  FrameBuffer frambuffer(width, height);
  frambuffer.use();
  frambuffer.setColorAttachmentTB(GL_TEXTURE_CUBE_MAP, GL_RGB16F, GL_RGB,
                                  GL_FLOAT);
  frambuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);
  // save state
  GLint viewport[] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  // draw
  frambuffer.loadViewPort(); // 512 x 512 cube
  generalVSUBO.setProjectionMatrix(projection);
  generalVSUBO.setCameraPos(glm::vec3(0.0f, 0.0f, 0.0f));
  for (uint i = 0; i < 6; ++i) {
    // bind texture i
    generalVSUBO.setViewMatrix(caputureViews[i]);
    frambuffer.bindColorCubeMap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // render
    drawCall();
  }
  glBindVertexArray(0);
  Texture texture(frambuffer.releaseColorAttachment(), GL_TEXTURE_CUBE_MAP);
  shader::Program::unBind();
  // reset state
  frambuffer.useDefault();
  generalVSUBO.setProjectionMatrix(projectionMatrix);
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  return texture;
}

Texture Renderer::equiTriangularToCubeMap(const Texture &equiTriangular) {
  return renderToCubeMap(
      512, 512,
      std::bind(&Renderer::renderSkybox, this, std::cref(equiTriangular)));
}

Texture Renderer::convoluteCubeMap(const Texture &cubeMap) {
  return renderToCubeMap(
      32, 32, [&cubeMap, cube = cube, &shader = iblConvolutionShader] {
        // render cubeMap
        glDepthFunc(GL_LEQUAL);
        glActiveTexture(GL_TEXTURE0 + shader::SkyboxShader::textureUnit);
        shader.bind();
        cubeMap.bind();
        glBindVertexArray(cube);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthFunc(GL_LESS);
      });
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
  generalVSUBO.setProjectionMatrix(projectionMatrix);
}

} // namespace render_system
