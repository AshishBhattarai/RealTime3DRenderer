#include "renderer.h"
#include "camera.h"
#include "components/transform.h"
#include "core/image.h"
#include "mesh.h"
#include "render_defaults.h"
#include "renderable_entity.h"
#include "shaders/general_vs_ubo.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

namespace render_system {

Renderer::Renderer(
    int width, int height, const std::unordered_map<MeshId, Mesh> &meshes,
    const std::unordered_map<MaterialId, std::unique_ptr<BaseMaterial>>
        &materials,
    const Camera *camera, const shader::StageCodeMap &flatForwardShader,
    const shader::StageCodeMap &skyboxCubeMapShader,
    const shader::StageCodeMap &iblConvolutionShader,
    const shader::StageCodeMap &iblSpecularConvolutionShader,
    const shader::StageCodeMap &iblBrdfIntegrationShader)
    : frameBuffer(width, height), meshes(meshes), materials(materials),
      projectionMatrix(1.0f), camera(camera), generalVSUBO(),
      flatForwardShader(flatForwardShader),
      skyboxCubeMapShader(skyboxCubeMapShader),
      iblConvolutionShader(iblConvolutionShader),
      iblSpecularConvolutionShader(iblSpecularConvolutionShader),
      iblBrdfIntegrationShader(iblBrdfIntegrationShader),
      cube(RenderDefaults::getInstance().getCubeVao()),
      plane(RenderDefaults::getInstance().getPlaneVao()),
      brdfIntegrationMap(generateBRDFIntegrationMap()) {

  // Setup framebuffer
  //  frameBuffer.use();
  //  frameBuffer.setColorAttachmentRB(GL_RGB);
  //  frameBuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void Renderer::loadPointLight(const PointLight &pointLight, uint idx) {
  flatForwardShader.bind();
  flatForwardShader.loadPointLight(pointLight, idx);
}

void Renderer::loadPointLightCount(size_t count) {
  flatForwardShader.loadPointLightSize(count);
}

void Renderer::preRender() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // TODO: global / gener ubos handled by render_system (data)
  generalVSUBO.setViewMatrix(camera->getViewMatrix());
  generalVSUBO.setCameraPos(camera->position);
}

void Renderer::preRenderMesh(const Texture &diffuseIbl,
                             const Texture &specularIbl) {
  flatForwardShader.bind();
  flatForwardShader.loadIrradianceMap(diffuseIbl);
  flatForwardShader.loadBrdfIntegrationMap(brdfIntegrationMap);
  flatForwardShader.loadPrefilteredMap(specularIbl);
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
  skyboxCubeMapShader.bind();
  skyboxCubeMapShader.bindTexture(texture);
  glBindVertexArray(cube);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glDepthFunc(GL_LESS);
}

Texture Renderer::renderToCubeMap(int width, int height, uint maxMipLevels,
                                  bool genMipMap,
                                  std::function<void(uint mipLevel)> drawCall) {
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
                                  GL_FLOAT, maxMipLevels > 1 || genMipMap);
  frambuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);
  // save state
  GLint viewport[] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  // draw
  frambuffer.loadViewPort(); // 512 x 512 cube
  generalVSUBO.setProjectionMatrix(projection);
  generalVSUBO.setCameraPos(glm::vec3(0.0f, 0.0f, 0.0f));
  // TODO: find better way do do this mipMap thingy
  // TODO: Make a separate class dedicated for generating & convoluting cubemap
  for (uint mip = 0; mip < maxMipLevels; ++mip) {
    // TODO: Add support to update buffer attachments in FrameBuffer
    if (mip != 0) {
      // different mipmaplevel sizes
      uint mipWidth = width * std::pow(0.5, mip);
      uint mipHeight = height * std::pow(0.5, mip);

      glBindRenderbuffer(GL_RENDERBUFFER, frambuffer.getDepthAttachmentId());
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
                            mipHeight);
      glViewport(0, 0, mipWidth, mipHeight);
    }
    for (uint i = 0; i < 6; ++i) {
      // bind texture i
      generalVSUBO.setViewMatrix(caputureViews[i]);
      frambuffer.bindColorCubeMap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // render
      drawCall(mip);
    }
  }
  if (genMipMap) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, frambuffer.getColorAttachmentId());
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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
      512, 512, 1, true,
      std::bind(&Renderer::renderSkybox, this, std::cref(equiTriangular)));
}

Texture Renderer::convoluteCubeMap(const Texture &cubeMap, bool diffuse) {
  uint prevMipLevel = std::numeric_limits<uint>::max();
  constexpr uint maxMipLevels = 5;
  if (diffuse)
    return renderToCubeMap(
        32, 32, 1, false,
        [&cubeMap, cube = cube, &shader = iblConvolutionShader](uint) {
          // render cubeMap
          glDepthFunc(GL_LEQUAL);
          shader.bind();
          shader.bindTexture(cubeMap);
          glBindVertexArray(cube);
          glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
          glDepthFunc(GL_LESS);
        });
  else
    return renderToCubeMap(
        128, 128, maxMipLevels, false,
        [&cubeMap, cube = cube, &shader = iblSpecularConvolutionShader,
         &prevMipLevel = prevMipLevel](uint mipLevel) {
          shader.bind();
          shader.envMap(cubeMap);
          if (mipLevel != prevMipLevel) {
            prevMipLevel = mipLevel;
            /**
             * 5 MipMap Levels
             * 0 - 0/4, 0.25 - 1/4, 0.5 - 2/4, 0.75 - 3/4, 1 - 4/4
             */
            shader.roughness((float)mipLevel / (float)(maxMipLevels - 1));
          }
          // render cubeMap
          glDepthFunc(GL_LEQUAL);
          glBindVertexArray(cube);
          glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
          glDepthFunc(GL_LESS);
        });
}

Texture Renderer::generateBRDFIntegrationMap() {
  FrameBuffer framebuffer(512, 512);
  framebuffer.use();
  framebuffer.setColorAttachmentTB(GL_TEXTURE_2D, GL_RG16F, GL_RG, GL_FLOAT);

  // TODO: do this inside Framebuffer class ??
  GLint viewport[] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  framebuffer.loadViewPort();

  // generate
  glDisable(GL_DEPTH_TEST);
  iblBrdfIntegrationShader.bind();
  glClear(GL_COLOR_BUFFER_BIT);
  glBindVertexArray(plane);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glEnable(GL_DEPTH_TEST);

  //  framebuffer.useDefault();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  return Texture(framebuffer.releaseColorAttachment(), GL_TEXTURE_2D);
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
