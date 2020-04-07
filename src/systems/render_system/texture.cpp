#include "texture.h"
#include "core/image.h"
#include <cassert>

namespace render_system {

Texture::Texture(const Image &image) : id(0) { loadTexture(image); }

void Texture::loadTexture(const Image &image) {
  assert(image.getBuffer() && "Invalid buffer data.");
  if (id || !image.getBuffer())
    return;

  const uchar *data = image.getBuffer()->data();
  int width = image.getWidth();
  int height = image.getHeight();
  int numChannels = image.getNumChannels();

  loadTexture(data, width, height, numChannels);
}

void Texture::loadTexture(const uchar *buffer, int width, int height,
                          int numChannels) {
  GLenum format = GL_RGBA;
  if (numChannels == 1)
    format = GL_RED;
  else if (numChannels == 2)
    format = GL_RG;
  else if (numChannels == 3)
    format = GL_RGB;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format,
               GL_UNSIGNED_BYTE, buffer);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace render_system
