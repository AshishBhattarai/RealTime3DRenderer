#pragma once
#include "types.h"
#include <glad/glad.h>
#include <vector>

// Image/Texture medium converts to or from opengl texture

class Image;
namespace render_system {
/**
 * @brief The TextureFlags enum
 * Specifies texture properties
 *
 * NONE indicates LDR RGB texture
 */
enum class TextureFlags : short {
  NONE = 0x0000,
  HDR = 0x0001,
  DISABLE_MIPMAP = 0x0002,
  SRGB = 0x0008 // convert to SRGB
};

class Texture : NonCopyable {
private:
  GLuint id;
  GLenum target;
  bool isDefault;

  /**
   * @brief loadTexture
   * @param buffer
   * @param width
   * @param height
   * @param numChannels
   * @param flags
   * @param cmi - CubeMap Index, Only used for (flags|CUBE_MAP) = true
   * @return
   */
  void loadTexture(const std::vector<const Image *> &images,
                   short flags = toUnderlying(TextureFlags::NONE));

public:
  Texture(const Image &image, short flags = toUnderlying(TextureFlags::NONE));
  /**
   * If isDefault is true texture isn't deleted on destructor
   **/
  Texture(GLuint id, bool isDefault = false) : id(id), isDefault(isDefault) {}
  ~Texture();

  /**
   * @brief release
   * @return releases the ownership of current id.
   */
  GLuint release();
  void bind() const { glBindTexture(target, id); }
  GLuint getId() const { return id; }
  GLenum getTarget() const { return target; }
}; // namespace Texture
} // namespace render_system
