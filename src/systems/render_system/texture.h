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
 * NONE indicates LDR RGB texture?? re do this
 */
enum class TextureFlags : short { NONE = 0x0000, DISABLE_MIPMAP = 0x0002, REPEATE = 0x0004 };

class Texture : NonCopyable {
private:
  static constexpr uint CUBE_MAP_IMAGES_SIZE = 6;
  GLuint id;
  GLenum target;

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
   * @brief Texture construts a 2D texture object in GPU.
   * @param image source image.
   * @param flags additional image properties.
   */
  Texture(const Image &image, short flags = toUnderlying(TextureFlags::NONE));

  /**
   * @brief Texture construts a cubemap texture object in GPU.
   * @param images array of cubemap source iamges.
   * @param flags additional image properties.
   */
  Texture(const std::array<const Image *, 6> images, short flags);

  Texture(GLuint id, GLenum target) : id(id), target(target) {}
  /**
   * @brief Texture move constructor.
   */
  Texture(Texture &&texture);

  /**
   * @brief operator = Texture move assignment
   * @return reference to new texture object.
   */
  Texture &operator=(Texture &&);

  ~Texture();

  /**
   * @brief release
   * @return releases the ownership of current id.
   */
  GLuint release();
  void bind() const { glBindTexture(target, id); }
  void unBind() const { glBindTexture(target, 0); }
  GLuint getId() const { return id; }
  GLenum getTarget() const { return target; }
}; // namespace Texture
} // namespace render_system
