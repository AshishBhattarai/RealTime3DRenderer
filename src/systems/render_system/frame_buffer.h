#pragma once
#include "types.h"
#include <array>
#include <cassert>

namespace render_system {

/** TODO: option Mipmaps for texture attachmnet,
 *	since framebuffer mipmap needs to generated each time we update the
 *  buffer, its better to create a function for mipmap generation
 **/
class FrameBuffer {
public:
  static constexpr uint MAX_COLOR_ATTACHMENTS = 32;
  enum class AttachType { NONE, TEXTURE_BUFFER, RENDER_BUFFER };
  enum class UseType : uint32_t {
    NORMAL = 0x8D40,
    READ = 0x8CA8,
    DRAW = 0x8CA9
  };
  FrameBuffer(int width, int height);
  ~FrameBuffer();

  /**
   * Set framebuffer attachments, cleans any previous buffers.
   *
   * @brief setColorAttachment
   * @param type
   * @param target - ignored for render buffer
   * @param internalFormat
   * @param transferFormat - ignored for render buffer
   * @param transferType - ignored for render buffer
   * @param index - AttachmentIndex
   */
  void setColorAttachment(AttachType type, uint32_t texTarget,
                          uint32_t internalFormat, uint32_t transferFormat,
                          uint32_t transferType, uint index = 0);

  /**
   * @brief setColorAttachment - creates textture buffer attachment
   * @param texTarget
   * @param internalFormat
   * @param transferFormat
   * @param transferType
   * @param index
   */
  void setColorAttachment(uint32_t texTarget, uint32_t internalFormat,
                          uint32_t transferFormat, uint32_t transferType,
                          uint index = 0);

  /**
   * @brief setColorAttachment - creates render buffer attachment
   * @param internalFormat
   * @param index
   */
  void setColorAttachment(uint32_t internalFormat, uint index = 0);
  /**
   * @brief setDepthAttachment
   * @param type
   * @param target - ignored for render buffer default(GL_TEXTURE_2D)
   */
  void setDepthAttachment(AttachType type, uint32_t texTarget = 0x0DE1);
  void setStencilAttachment(AttachType type, uint32_t texTarget = 0x0DE1);
  void setDepthStencilAttachment(AttachType type, uint32_t texTarget = 0x0DE1);

  // utitly
  bool isComplete();
  void use(UseType type = UseType::NORMAL);
  static void useDefault();
  void loadViewPort();
  /**
   * @brief clearBuffer
   * Not delete it calss glClear()
   */
  void clearBuffer();
  void deleteAllColorAttachments();
  void deleteColorAttachment(uint index = 0);
  void deleteDepthAttachment() {
    deleteAttachment(depthAttachType, &depthBuffer, 1);
  }
  void deleteStencilAttachment() {
    deleteAttachment(stencilAttachType, &stencilBuffer, 1);
  }
  void deleteDepthStencilAttachment() {
    deleteAttachment(depthStencilAttachType, &depthBuffer, 1);
  }

  // getters
  uint getId() const { return fbo; }
  /**
   * @brief getColorAttachmentId
   * @param index - AttachmentIndex
   * @return
   */
  uint getColorAttachmentId(uint index = 0) const {
    assert(index < MAX_COLOR_ATTACHMENTS &&
           "Color attachment index out of range.");
    return colorBuffers[index];
  }
  uint getDepthAttachmentId() const { return depthBuffer; }
  uint getStencilAttachmentId() const { return stencilBuffer; }
  uint getDepthStencilAttachmentId() const { return depthBuffer; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  /**
   * @brief getColorAttachType
   * @param index - AttachmentIndex
   * @return
   */
  AttachType getColorAttachType(uint index = 0) const {
    assert(index < MAX_COLOR_ATTACHMENTS &&
           "Color attachment index out of range.");
    return colorAttachTypes.at(index);
  }
  AttachType getDepthAttacType() const { return depthAttachType; }
  AttachType getStencilAttachType() const { return stencilAttachType; }
  AttachType getDepthStencilAttachType() const {
    return depthStencilAttachType;
  }

  /**
   * @brief readPixels
   * @param buffer -> output buffer
   * @param width -> output image width
   * @param height -> output image height
   * @return
   */
  bool readPixels(char *buffer, int &nrChannels, int &width, int &height);

private:
  uint fbo;
  std::array<uint, MAX_COLOR_ATTACHMENTS>
      colorBuffers;   // color attachment render/texture attachment buffer
  uint stencilBuffer; // stencil render/texture attachment buffer
  uint depthBuffer;   // deptStencil incase of GL_DEPTH24_STENCIL8

  int width;
  int height;

  std::array<AttachType, MAX_COLOR_ATTACHMENTS> colorAttachTypes;
  AttachType depthAttachType;
  AttachType stencilAttachType;
  AttachType depthStencilAttachType;

  void deleteAttachment(AttachType &type, uint *buffer, uint num);
  void createTextureBuffer(uint &buffer, uint32_t target,
                           uint32_t internalFormat, uint32_t transferFormat,
                           uint32_t transferType, uint32_t texAttachment);
  void createRenderBuffer(uint &buffer, uint32_t internalFormat,
                          uint32_t texAttachment);
};

} // namespace render_system
