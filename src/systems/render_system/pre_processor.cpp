#include "pre_processor.h"
#include "default_primitives_renderer.h"
#include "frame_buffer.h"
#include "render_defaults.h"
#include <glm/gtc/matrix_transform.hpp>

namespace render_system {
PreProcessor::PreProcessor(const shader::StageCodeMap &cubemapShader,
                           const shader::StageCodeMap &equirectangularShader,
                           const shader::StageCodeMap &iblDiffuseConvolutionShader,
                           const shader::StageCodeMap &iblSpecularConvolutionShader,
                           const shader::StageCodeMap &iblBRDFIntegrationShader)
    : cubemapShader(cubemapShader), equirectangularShader(equirectangularShader),
      iblDiffuseConvolutionShader(iblDiffuseConvolutionShader),
      iblSpecularConvolutionShader(iblSpecularConvolutionShader),
      iblBRDFIntegrationShader(iblBRDFIntegrationShader) {}

Texture PreProcessor::renderToCubeMap(int width, int height, uint maxMipLevels, bool genMipMap,
                                      shader::Cubemap *const shader,
                                      std::function<void(uint)> preDrawCall) {
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
                  glm::vec3(0.0f, -1.0f, 0.0f))};                                  // -Z
  glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f); // cube ar 1:1

  FrameBuffer frambuffer(width, height);
  frambuffer.use();
  frambuffer.setColorAttachmentTB(GL_TEXTURE_CUBE_MAP, GL_RGB16F, GL_RGB, GL_FLOAT,
                                  maxMipLevels > 1 || genMipMap);
  frambuffer.setDepthAttachment(FrameBuffer::AttachType::RENDER_BUFFER);
  // save state
  GLint viewport[] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);

  frambuffer.loadViewPort();
  shader->bind();
  shader->loadProjection(projection);

  for (uint mip = 0; mip < maxMipLevels; ++mip) {
    // TODO: Add support to update buffer attachments in FrameBuffer
    if (mip != 0) {
      // different mipmaplevel sizes
      uint mipWidth = width * std::pow(0.5, mip);
      uint mipHeight = height * std::pow(0.5, mip);

      glBindRenderbuffer(GL_RENDERBUFFER, frambuffer.getDepthAttachmentId());
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
      glViewport(0, 0, mipWidth, mipHeight);
    }
    for (uint i = 0; i < 6; ++i) {
      // bind texture i
      frambuffer.bindColorCubeMap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shader->loadView(caputureViews[i]);
      preDrawCall(mip);
      // render cubeMap
      glDepthFunc(GL_LEQUAL);
      DefaultPrimitivesRenderer::getInstance().drawCube();
      glDepthFunc(GL_LESS);
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
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  return texture;
}

Texture PreProcessor::equirectangularToCubemap(const Texture &equirectangular) {
  return renderToCubeMap(1024, 1024, 1, true, &equirectangularShader,
                         [&shader = equirectangularShader, &texture = equirectangular](uint) {
                           /* pre draw call */
                           shader.loadTexture(texture);
                         });
}

Texture PreProcessor::generateIrradianceMap(const Texture &envmap) {
  return renderToCubeMap(64, 64, 1, false, &iblDiffuseConvolutionShader,
                         [&shader = iblDiffuseConvolutionShader, &texture = envmap](uint) {
                           /* pre draw call */
                           shader.loadTexture(texture);
                         });
}

Texture PreProcessor::generatePreFilteredMap(const Texture &envmap) {
  uint prevMipLevel = std::numeric_limits<uint>::max();
  constexpr uint maxMipLevels = 5;
  return renderToCubeMap(
      256, 256, maxMipLevels, false, &iblSpecularConvolutionShader,
      [&shader = iblSpecularConvolutionShader, &texture = envmap, &prevMipLevel](uint mipLevel) {
        /* pre draw call */
        shader.loadTexture(texture);
        if (mipLevel != prevMipLevel) {
          prevMipLevel = mipLevel;
          /**
           * 5 MipMap Levels
           * 0 - 0/4, 0.25 - 1/4, 0.5 - 2/4, 0.75 - 3/4, 1 -
           * 4/4
           */
          shader.loadRoughness((float)mipLevel / (float)(maxMipLevels - 1));
        }
      });
}

Texture PreProcessor::generateBRDFIntegrationMap() {
  FrameBuffer framebuffer(512, 512);
  framebuffer.use();
  framebuffer.setColorAttachmentTB(GL_TEXTURE_2D, GL_RG16F, GL_RG, GL_FLOAT);

  // TODO: do this inside Framebuffer class ??
  GLint viewport[] = {0, 0, 0, 0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  framebuffer.loadViewPort();

  // generate
  iblBRDFIntegrationShader.bind();
  glClear(GL_COLOR_BUFFER_BIT);
  DefaultPrimitivesRenderer::getInstance().drawPlane();

  //  framebuffer.useDefault();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  return Texture(framebuffer.releaseColorAttachment(), GL_TEXTURE_2D);
}

} // namespace render_system
