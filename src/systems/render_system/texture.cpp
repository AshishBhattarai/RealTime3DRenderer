#include "texture.h"
#include "core/image.h"
#include "types.h"
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

  short nFlags = flags;
  if (image.getIsHDR())
    nFlags |= toUnderlying(TextureFlags::HDR);
  loadTexture({&image}, nFlags);
}

Texture::~Texture() {
  if (!isDefault) {
    glBindTexture(target, 0);
    glDeleteTextures(1, &id);
  }
}

void Texture::loadTexture(const std::vector<const Image *> &images,
                          short flags) {

  GLenum internalFormat = GL_RGBA;
  GLenum transferType = GL_UNSIGNED_BYTE;
  target = GL_TEXTURE_2D;
  bool disableMipmap = flags & toUnderlying(TextureFlags::DISABLE_MIPMAP);
  if (flags & toUnderlying(TextureFlags::HDR)) {
    internalFormat = GL_RGB16F;
    transferType = GL_FLOAT;
  }

  /**
   * To make texture filter customizable, i can have a RTextureLoader
   * singleton that has a textureFilterType member that is use to set
   * filter type.
   */
  glGenTextures(1, &id);
  glBindTexture(target, id);
  GLenum texTarget = GL_TEXTURE_2D;
  if (target == GL_TEXTURE_CUBE_MAP)
    texTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

  for (uint i = 0; i < images.size(); ++i) {
    const Image &image = *images.at(i);
    auto numChannels = image.getNumChannels();
    auto width = image.getWidth();
    auto height = image.getHeight();
    const void *buffer = image.getBuffer()->data();

    GLenum format = GL_RGBA;
    if (numChannels == 1)
      format = GL_RED;
    else if (numChannels == 2)
      format = GL_RG;
    else if (numChannels == 3)
      format = GL_RGB;

    glTexImage2D(texTarget + i, 0, GL_RGB, width, height, 0, format,
                 transferType, (void *)buffer);
  }
  if (!disableMipmap) {
    // this works for cubemap too
    glGenerateMipmap(target);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  } else {
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(target, 0);
}

GLuint Texture::release() {
  GLuint tId = id;
  id = 0;
  return tId;
}
} // namespace render_system
