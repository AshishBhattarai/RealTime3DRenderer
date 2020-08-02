#include "pre_processor.h"
#include "frame_buffer.h"
#include "render_defaults.h"
#include <glm/gtc/matrix_transform.hpp>

namespace render_system {
PreProcessor::PreProcessor(const shader::StageCodeMap &cubemapShader,
                           const shader::StageCodeMap &equirectangularShader)
    : cubemapShader(cubemapShader),
      equirectangularShader(equirectangularShader) {
  cube = RenderDefaults::getInstance().getCubeVao();
}

Texture PreProcessor::renderToCubeMap(int width, int height, uint maxMipLevels,
                                      bool genMipMap,
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
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
                            mipHeight);
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
      glBindVertexArray(cube);
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
  return renderToCubeMap(
      512, 512, 1, true, &equirectangularShader,
      [&shader = equirectangularShader, &texture = equirectangular](uint) {
        /* pre draw call */
        shader.loadTexture(texture);
      });
}

} // namespace render_system
