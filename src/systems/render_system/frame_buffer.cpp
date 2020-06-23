#include "frame_buffer.h"
#include <cassert>
#include <glad/glad.h>

namespace render_system {

FrameBuffer::FrameBuffer(int width, int height)
    : fbo(0), stencilBuffer(0), depthBuffer(0), bufferFlag(0), width(width),
      height(height), depthAttachType(AttachType::NONE),
      stencilAttachType(AttachType::NONE),
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
bool FrameBuffer::isComplete() const {
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
void FrameBuffer::use(UseType type) const {
  glBindFramebuffer(toUnderlying(type), fbo);
}
void FrameBuffer::useDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::loadViewPort() { glViewport(0, 0, width, height); }
/**
 * @brief clearBuffer
 * Not delete it calss glClear()
 */
void FrameBuffer::clearBuffer() { glClear(bufferFlag); }

void FrameBuffer::deleteAllColorAttachments() {
  for (uint i = 0; i < MAX_COLOR_ATTACHMENTS; ++i) {
    if (colorBuffers[i]) {
      deleteAttachment(colorAttachTypes[i], &colorBuffers[i], 1);
    }
  }
  bufferFlag &= ~GL_COLOR_BUFFER_BIT;
  validColorBuffers.clear();
}

void FrameBuffer::deleteColorAttachment(uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  if (colorBuffers[index]) {
    deleteAttachment(colorAttachTypes[index], &colorBuffers[index], 1);
    validColorBuffers.erase(index);
  }
  if (validColorBuffers.empty())
    bufferFlag &= ~GL_COLOR_BUFFER_BIT;
}

void FrameBuffer::deleteDepthAttachment() {
  deleteAttachment(depthAttachType, &depthBuffer, 1);
  bufferFlag &= ~GL_DEPTH_BUFFER_BIT;
}
void FrameBuffer::deleteStencilAttachment() {
  deleteAttachment(stencilAttachType, &stencilBuffer, 1);
  bufferFlag &= ~GL_STENCIL_BUFFER_BIT;
}
void FrameBuffer::deleteDepthStencilAttachment() {
  deleteAttachment(depthStencilAttachType, &depthBuffer, 1);
  bufferFlag &= ~GL_DEPTH_BUFFER_BIT;
  bufferFlag &= ~GL_STENCIL_BUFFER_BIT;
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

void FrameBuffer::createTextureBuffer(uint &buffer, u32 target,
                                      u32 internalFormat, u32 transferFormat,
                                      u32 transferType, u32 texAttachment) {
  glGenTextures(1, &buffer);
  glBindTexture(target, buffer);
  // Number for images in the buffer
  uint size = 1;
  u32 texTarget = target;
  if (target == GL_TEXTURE_CUBE_MAP) {
    size = 6;
    texTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  }
  for (uint i = 0; i < size; ++i) {
    glTexImage2D(texTarget + i, 0, internalFormat, width, height, 0,
                 transferFormat, transferType, 0);
    if (size == 1) {
      // for cubemap we call this before rendering each face.(bindColorCubeMap).
      glFramebufferTexture2D(GL_FRAMEBUFFER, texAttachment, texTarget + i,
                             buffer, 0);
    }
  }
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (target == GL_TEXTURE_CUBE_MAP)
    glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void FrameBuffer::createRenderBuffer(uint &buffer, u32 internalFormat,
                                     u32 texAttachment) {
  glCreateRenderbuffers(1, &buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, texAttachment, GL_RENDERBUFFER,
                            buffer);
}

void FrameBuffer::setColorAttachment(AttachType type, u32 texTarget,
                                     u32 internalFormat, u32 transferFormat,
                                     u32 transferType, uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  assert(!colorBuffers[index] && "[WARN] - Color attachment already exists.");
  assert(type != AttachType::NONE && "Attacty type cannot be NONE.");
  if (!colorBuffers[index])
    return;
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(colorBuffers[index], internalFormat,
                       GL_COLOR_ATTACHMENT0 + index);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(colorBuffers[index], texTarget, internalFormat,
                        transferFormat, transferType,
                        GL_COLOR_ATTACHMENT0 + index);
  }
  colorAttachTypes[index] = type;
  bufferFlag |= GL_COLOR_BUFFER_BIT;
  validColorBuffers.insert(index);
}

void FrameBuffer::setColorAttachmentTB(u32 texTarget, u32 internalFormat,
                                       u32 transferFormat, u32 transferType,
                                       uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  assert(!colorBuffers[index] && "[WARN] - Color attachment already exists.");
  if (colorBuffers[index])
    return;
  createTextureBuffer(colorBuffers[index], texTarget, internalFormat,
                      transferFormat, transferType,
                      GL_COLOR_ATTACHMENT0 + index);
  colorAttachTypes[index] = AttachType::TEXTURE_BUFFER;
  bufferFlag |= GL_COLOR_BUFFER_BIT;
  validColorBuffers.insert(index);
}

void FrameBuffer::setColorAttachmentRB(u32 internalFormat, uint index) {
  assert(index < MAX_COLOR_ATTACHMENTS &&
         "Color attachment index out of range.");
  assert(!colorBuffers[index] && "[WARN] - Color attachment already exists.");
  if (colorBuffers[index])
    return;
  createRenderBuffer(colorBuffers[index], internalFormat,
                     GL_COLOR_ATTACHMENT0 + index);
  colorAttachTypes[index] = AttachType::RENDER_BUFFER;
  bufferFlag |= GL_COLOR_BUFFER_BIT;
  validColorBuffers.insert(index);
}

void FrameBuffer::setDepthAttachment(AttachType type, u32 texTarget) {
  assert(!depthBuffer && "[WARN] - Depth attachment already exists.");
  assert(type != AttachType::NONE && "Attacty type cannot be NONE.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(depthBuffer, GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(depthBuffer, texTarget, GL_DEPTH_COMPONENT24,
                        GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
  }
  depthAttachType = type;
  bufferFlag |= GL_DEPTH_BUFFER_BIT;
}

void FrameBuffer::setStencilAttachment(AttachType type, u32 texTarget) {
  assert(!stencilBuffer && "[WARN] - Stencil attachment already exists.");
  assert(type != AttachType::NONE && "Attacty type cannot be NONE.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(stencilBuffer, GL_STENCIL_INDEX8, GL_STENCIL_ATTACHMENT);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(stencilBuffer, texTarget, GL_STENCIL_INDEX8,
                        GL_STENCIL_INDEX, GL_UNSIGNED_BYTE,
                        GL_STENCIL_ATTACHMENT);
  }
  depthAttachType = type;
  bufferFlag |= GL_STENCIL_BUFFER_BIT;
}

void FrameBuffer::setDepthStencilAttachment(AttachType type, u32 texTarget) {
  assert(!depthBuffer && !stencilBuffer &&
         "[WARN] - Depth or stencil attachment already exists.");
  assert(type != AttachType::NONE && "Attacty type cannot be NONE.");
  if (type == AttachType::RENDER_BUFFER) {
    createRenderBuffer(depthBuffer, GL_DEPTH24_STENCIL8,
                       GL_DEPTH_STENCIL_ATTACHMENT);
  } else if (type == AttachType::TEXTURE_BUFFER) {
    createTextureBuffer(depthBuffer, texTarget, GL_DEPTH24_STENCIL8,
                        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
                        GL_DEPTH_STENCIL_ATTACHMENT);
  }
  depthAttachType = type;
  bufferFlag |= GL_DEPTH_BUFFER_BIT;
  bufferFlag |= GL_STENCIL_BUFFER_BIT;
}

void FrameBuffer::bindColorCubeMap(u32 texTarget, uint index) {
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                         texTarget, colorBuffers[index], 0);
}

uint FrameBuffer::releaseColorAttachment(uint index) {
  if (colorAttachTypes[index] != AttachType::TEXTURE_BUFFER)
    return 0;
  GLuint textureId = colorBuffers[index];
  colorBuffers[index] = 0;
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                         GL_TEXTURE_2D, 0, 0);
  validColorBuffers.erase(validColorBuffers.find(index));
  return textureId;
}

Buffer FrameBuffer::readPixels(int &nrChannels, int &width, int &height,
                               u32 fromBuffer) {
  assert(isComplete() && "Framebuffers must be complete before use.");
  height = this->height;
  width = this->width;
  nrChannels = 4;
  // add padding, to create 4-byte alignment
  int stride = Buffer::align(nrChannels * width, 4);
  int bufferSize = stride * height;
  Buffer buffer(bufferSize, 4);
  glNamedFramebufferReadBuffer(fbo, fromBuffer);
  glPixelStorei(GL_PACK_ALIGNMENT, 4); // alignment for each pixel in memory
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
  return buffer;
}

Buffer FrameBuffer::readPixelsWindow(int &nrChannels, int &width, int &height,
                                     u32 fromBuffer) {
  int viewPort[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewPort);
  width = viewPort[2];
  height = viewPort[3];
  nrChannels = 4;
  int stride = Buffer::align(nrChannels * width, 4);
  int bufferSize = stride * height;
  Buffer buffer(bufferSize, 4);
  glNamedFramebufferReadBuffer(0, fromBuffer);
  glPixelStorei(GL_PACK_ALIGNMENT, 4); // alignment for each pixel in memory
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
  return buffer;
}

void FrameBuffer::blit(FrameBuffer *toFrameBuffer, u32 toBuffer,
                       u32 fromBuffer) {
  assert(isComplete() && "Framebuffers must be complete before use.");
  int toWidth = 0;
  int toHeight = 0;
  uint bufferFlag = 0;
  if (toFrameBuffer) {
    assert(toFrameBuffer->isComplete() &&
           "Framebuffers must be complete before use.");
    toWidth = toFrameBuffer->width;
    toHeight = toFrameBuffer->height;
    toFrameBuffer->use(UseType::DRAW);
    bufferFlag = toFrameBuffer->bufferFlag;
  } else {
    int viewPort[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewPort);
    toWidth = viewPort[2];
    toHeight = viewPort[3];
    bufferFlag = GL_COLOR_BUFFER_BIT;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }
  use(UseType::READ);
  glDrawBuffer(toBuffer);
  glReadBuffer(fromBuffer);
  glBlitFramebuffer(0, 0, width, height, 0, 0, toWidth, toHeight, bufferFlag,
                    GL_NEAREST);
}

void FrameBuffer::blitWindow(const FrameBuffer &toFrameBuffer, u32 toBuffer,
                             u32 fromBuffer) {
  assert(toFrameBuffer.isComplete() &&
         "Framebuffers must be complete before use.");
  int toWidth = toFrameBuffer.width;
  int toHeight = toFrameBuffer.height;
  int bufferFlag = toFrameBuffer.bufferFlag;
  int viewPort[4] = {};
  glGetIntegerv(GL_VIEWPORT, viewPort);
  int width = viewPort[2];
  int height = viewPort[3];
  toFrameBuffer.use(UseType::DRAW);
  glNamedFramebufferReadBuffer(0, fromBuffer);
  glDrawBuffer(toBuffer);
  glBlitFramebuffer(0, 0, width, height, 0, 0, toWidth, toHeight, bufferFlag,
                    GL_NEAREST);
}

} // namespace render_system
