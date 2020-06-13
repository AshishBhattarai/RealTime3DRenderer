#include "texture.h"
#include "core/image.h"
#include "utils/slogger.h"
#include <cassert>

namespace render_system {

Texture::Texture(const Image &image, short flags)
    : id(0), target(GL_TEXTURE_2D), isDefault(false) {
  assert(image.getBuffer() && "Invalid buffer data.");
  if (!image.getBuffer()) {
    CSLOG("LOAD_TEXTURE failed invalid image.");
    return;
  }

  const uchar *data = image.getBuffer()->data();
  int width = image.getWidth();
  int height = image.getHeight();
  int numChannels = image.getNumChannels();

  short nFlags = flags;
  if (image.getIsHDR())
    nFlags |= toUnderlying(TextureFlags::HDR);
  if (nFlags & toUnderlying(TextureFlags::CUBE_MAP))
    ; // TODO: CUBE_MAP
  else
    loadTexture2D(data, width, height, numChannels, nFlags);
}

Texture::Texture(const uchar *buffer, int width, int height, int numChannels,
                 short flags)
    : id(0), target(GL_TEXTURE_2D), isDefault(false) {
  if (flags & toUnderlying(TextureFlags::CUBE_MAP))
    ; // TODO: CUBE_MAP
  else
    loadTexture2D(buffer, width, height, numChannels, flags);
}

Texture::~Texture() {
  if (!isDefault) {
    glBindTexture(target, 0);
    glDeleteTextures(1, &id);
  }
}

void Texture::loadTexture2D(const uchar *buffer, int width, int height,
                            int numChannels, short flags) {
  GLenum format = GL_RGBA;
  if (numChannels == 1)
    format = GL_RED;
  else if (numChannels == 2)
    format = GL_RG;
  else if (numChannels == 3)
    format = GL_RGB;

  GLenum internalFormat = GL_RGBA;
  GLenum transferType = GL_UNSIGNED_BYTE;
  if (flags & toUnderlying(TextureFlags::HDR)) {
    internalFormat = GL_RGBA16F;
    transferType = GL_HALF_FLOAT;
  }

  /**
   * To make texture filter customizable, i can have a RTextureLoader
   * singleton that has a textureFilterType member that is use to set
   * filter type.
   */
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
               transferType, (void *)buffer);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  target = GL_TEXTURE_2D;
}

GLuint Texture::release() {
  GLuint tId = id;
  id = 0;
  return tId;
}
} // namespace render_system
