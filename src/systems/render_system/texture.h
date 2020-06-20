#pragma once
#include "types.h"
#include <array>
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
enum class TextureFlags : short { NONE = 0x0000, DISABLE_MIPMAP = 0x0002 };

class Texture : NonCopyable {
private:
  static constexpr uint CUBE_MAP_IMAGES_SIZE = 6;

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
  /**
   * @brief Texture construts a texture object in GPU.
   * @param image source image.
   * @param flags additional image properties.
   */
  Texture(const Image &image, short flags = toUnderlying(TextureFlags::NONE));

  Texture(const std::array<const Image *, 6> images, short flags);

  /**
   * If isDefault is true texture isn't deleted on destructor
   *
   * TODO: Remove this and move to duplicate default.
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
