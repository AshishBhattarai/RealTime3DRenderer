#include "frame_buffer.h"
#include <cassert>
#include <glad/glad.h>

namespace render_system {

FrameBuffer::FrameBuffer(int width, int height)
    : fbo(0), stencilBuffer(0), depthBuffer(0), width(width), height(height),
      depthAttachType(AttachType::NONE), stencilAttachType(AttachType::NONE),
      depthStencilAttachType(AttachType::NONE) {
  colorBuffers.fill(0);
  colorAttachTypes.fill(AttachType::NONE);
  glGenFramebuffers(1, &fbo);
}

FrameBuffer::~FrameBuffer() {
  // cleanup
  useDefault();
  deleteAllColorAttachments();
  deleteDepthAttachment();
  deleteStencilAttachment();
  deleteDepthStencilAttachment();

  // delete framebuffer
  glDeleteFramebuffers(1, &fbo);
}

// utitly
bool FrameBuffer::isComplete() {
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
void FrameBuffer::use(UseType type) {
  glBindFramebuffer(toUnderlying(type), fbo);
}
void FrameBuffer::useDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::loadViewPort() { glViewport(0, 0, width, height); }
/**
 * @brief clearBuffer
 * Not delete it calss glClear()
 */
void FrameBuffer::clearBuffer() {
  unsigned int clear = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
  if (stencilAttachType != AttachType::NONE ||
      depthStencilAttachType != AttachType::NONE)
    clear |= GL_STENCIL_BUFFER_BIT;
  glClear(clear);
}

void FrameBuffer::deleteAllColorAttachments() {
  for (uint i = 0; i < MAX_COLOR_ATTACHMENTS; ++i) {
    if (colorBuffers[i]) {
      deleteAttachment(colorAttachTypes[i], &colorBuffers[i], 1);
    }
  }
}

void FrameBuffer::deleteColorAttachment(uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  if (colorBuffers[index]) {
    deleteAttachment(colorAttachTypes[index], &colorBuffers[index], 1);
  }
}

void FrameBuffer::deleteAttachment(AttachType &type, uint *buffer, uint num) {
  if (type == AttachType::TEXTURE_BUFFER) {
    glDeleteTextures(num, buffer);
  } else if (type == AttachType::RENDER_BUFFER) {
    glDeleteRenderbuffers(num, buffer);
  }
  *buffer = 0;
  type = AttachType::NONE;
}

void FrameBuffer::createTextureBuffer(uint &buffer, uint32_t target,
                                      uint32_t internalFormat,
                                      uint32_t transferFormat,
                                      uint32_t transferType,
                                      uint32_t texAttachment) {
  glGenTextures(1, &buffer);
  glBindTexture(target, buffer);
  // Number for images in the buffer
  uint size = 1;
  uint32_t texTarget = target;
  if (target == GL_TEXTURE_CUBE_MAP) {
    size = 6;
    texTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  }
  for (uint i = 0; i < size; ++i) {
    glTexImage2D(texTarget + i, 0, internalFormat, width, height, 0,
                 transferFormat, transferType, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, texAttachment, texTarget + i, buffer,
                           0);
  }
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (target == GL_TEXTURE_CUBE_MAP)
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void FrameBuffer::createRenderBuffer(uint &buffer, uint32_t internalFormat,
                                     uint32_t texAttachment) {
  glCreateRenderbuffers(1, &buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, texAttachment, GL_RENDERBUFFER,
                            buffer);
}

void FrameBuffer::setColorAttachment(AttachType type, uint32_t texTarget,
                                     uint32_t internalFormat,
                                     uint32_t transferFormat,
                                     uint32_t transferType, uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  assert(!colorBuffers[index] && "[WARN] - Color attachment already exists.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(colorBuffers[index], internalFormat,
                       GL_COLOR_ATTACHMENT0 + index);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(colorBuffers[index], texTarget, internalFormat,
                        transferFormat, transferType,
                        GL_COLOR_ATTACHMENT0 + index);
  }
  colorAttachTypes[index] = type;
}

void FrameBuffer::setColorAttachment(uint32_t texTarget,
                                     uint32_t internalFormat,
                                     uint32_t transferFormat,
                                     uint32_t transferType, uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  assert(!colorBuffers[index] && "[WARN] - Color attachment already exists.");
  createTextureBuffer(colorBuffers[index], texTarget, internalFormat,
                      transferFormat, transferType,
                      GL_COLOR_ATTACHMENT0 + index);
  colorAttachTypes[index] = AttachType::TEXTURE_BUFFER;
}

void FrameBuffer::setColorAttachment(uint32_t internalFormat, uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  assert(!colorBuffers[index] && "[WARN] - Color attachment already exists.");
  createRenderBuffer(colorBuffers[index], internalFormat,
                     GL_COLOR_ATTACHMENT0 + index);
  colorAttachTypes[index] = AttachType::RENDER_BUFFER;
}

void FrameBuffer::setDepthAttachment(AttachType type, uint32_t texTarget) {
  assert(!depthBuffer && "[WARN] - Depth attachment already exists.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(depthBuffer, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(depthBuffer, texTarget, GL_DEPTH_COMPONENT,
                        GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
  }
  depthAttachType = type;
}

void FrameBuffer::setStencilAttachment(AttachType type, uint32_t texTarget) {
  assert(!stencilBuffer && "[WARN] - Stencil attachment already exists.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(stencilBuffer, GL_STENCIL_INDEX, GL_STENCIL_ATTACHMENT);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(stencilBuffer, texTarget, GL_STENCIL_INDEX,
                        GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
  }
  depthAttachType = type;
}

void FrameBuffer::setDepthStencilAttachment(AttachType type,
                                            uint32_t texTarget) {
  assert(!depthBuffer && !stencilBuffer &&
         "[WARN] - Depth or stencil attachment already exists.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(depthBuffer, GL_DEPTH32F_STENCIL8,
                       GL_DEPTH_STENCIL_ATTACHMENT);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(depthBuffer, texTarget, GL_DEPTH24_STENCIL8,
                        GL_DEPTH32F_STENCIL8, GL_UNSIGNED_INT_24_8,
                        GL_DEPTH_STENCIL_ATTACHMENT);
  }
  depthAttachType = type;
}
} // namespace render_system
