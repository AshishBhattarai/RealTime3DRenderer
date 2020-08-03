#pragma once
#include "core/image.h"
#include "types.h"
#include <array>
#include <cassert>
#include <unordered_set>

namespace render_system {

/** TODO: option Mipmaps for texture attachmnet,
 *	since framebuffer mipmap needs to generated each time we update the
 *  buffer, its better to create a function for mipmap generation
 *
 * TODO: Store texture paramters?
 **/
class FrameBuffer {
public:
  static constexpr uint MAX_COLOR_ATTACHMENTS = 32;
  enum class AttachType { NONE, TEXTURE_BUFFER, RENDER_BUFFER };
  enum class UseType : u32 { NORMAL = 0x8D40, READ = 0x8CA8, DRAW = 0x8CA9 };
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
  void setColorAttachment(AttachType type, u32 texTarget, u32 internalFormat,
                          u32 transferFormat, u32 transferType, uint index = 0);

  /**
   * @brief setColorAttachment - creates texture buffer attachment
   * @param texTarget
   * @param internalFormat
   * @param transferFormat
   * @param transferType
   * @param index
   */
  void setColorAttachmentTB(u32 texTarget, u32 internalFormat,
                            u32 transferFormat, u32 transferType,
                            bool enableMipMap = false, uint index = 0);

  /**
   * @brief setColorAttachment - creates render buffer attachment
   * @param internalFormat
   * @param index
   */
  void setColorAttachmentRB(u32 internalFormat, uint index = 0);
  /**
   * @brief setDepthAttachment
   * @param type
   * @param target - ignored for render buffer default(GL_TEXTURE_2D)
   */
  void setDepthAttachment(AttachType type, u32 texTarget = 0x0DE1);
  void setStencilAttachment(AttachType type, u32 texTarget = 0x0DE1);
  void setDepthStencilAttachment(AttachType type, u32 texTarget = 0x0DE1);

  /**
   * @brief bindColorCubeMap - This for rendering color buffer into a cubemaop
   * face. This method calls glFramebufferTexture2D with given textureTarget
   * @param texTarget
   * @param index
   */
  void bindColorCubeMap(u32 texTarget, uint mipLevel = 0, uint index = 0);

  /**
   * @brief releaseColorAttachment - This method releases the ownership of color
   * attachment texture
   * @param index
   * @return returns 0 if the color attachment is not texture attachment or
   * invalid index.
   */
  uint releaseColorAttachment(uint index = 0);

  // utitly
  bool isComplete() const;
  void use(UseType type = UseType::NORMAL) const;
  static void useDefault();
  void loadViewPort();
  /**
   * @brief clearBuffer
   * Not delete it calss glClear()
   */
  void clearBuffer();
  void deleteAllColorAttachments();
  void deleteColorAttachment(uint index = 0);
  void deleteDepthAttachment();
  void deleteStencilAttachment();
  void deleteDepthStencilAttachment();
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
   * @param fromColorBuffer - Default GL_COLOR_ATTACHMENT0
   * @return
   */
  Image readPixels(u32 fromColorBuffer = 0x8CE0);

  /**
   * Read pixels from default framebuffer ie window
   *
   * @brief readPixelsDe
   * @param nrChannels
   * @param width
   * @param height
   * @return
   */
  static Image readPixelsWindow(u32 fromColorBuffer = 0x0404);

  /**
   * @brief blit
   * @param toFrameBuffer - Defaul nullptr (blits to window framebuffer)
   * @param toBuffer
   * @param fromBuffer - default GL_COLOR_ATTACHMENT0
   * @return
   */
  void blit(FrameBuffer *toFrameBuffer, u32 toColorBuffer,
            u32 fromColorBuffer = 0x8CE0);

  /**
   * @brief blitWindow - blit from window (default framebuffer)
   * @param toFrameBuffer
   * @param toBuffer
   * @param fromBuffer - default GL_BACK
   */
  static void blitWindow(const FrameBuffer &toFrameBuffer, u32 toColorBuffer,
                         u32 fromColorBuffer = 0x0404);

private:
  uint fbo;
  std::array<uint, MAX_COLOR_ATTACHMENTS>
      colorBuffers;   // color attachment render/texture attachment buffer
  uint stencilBuffer; // stencil render/texture attachment buffer
  uint depthBuffer;   // deptStencil incase of GL_DEPTH24_STENCIL8

  uint bufferFlag; // buffer flags ie COLOR_BUFFER_BIT, DEPTH_BUFFER_BIT ....

  std::unordered_set<uint>
      validColorBuffers; // contains index for valid color buffers

  int width;
  int height;

  std::array<AttachType, MAX_COLOR_ATTACHMENTS> colorAttachTypes;
  AttachType depthAttachType;
  AttachType stencilAttachType;
  AttachType depthStencilAttachType;

  void deleteAttachment(AttachType &type, uint *buffer, uint num);
  void createTextureBuffer(uint &buffer, u32 target, u32 internalFormat,
                           u32 transferFormat, u32 transferType,
                           u32 texAttachment, bool enableMipMap);
  void createRenderBuffer(uint &buffer, u32 internalFormat, u32 texAttachment);
};

} // namespace render_system
