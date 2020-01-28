#include "render_defaults.h"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/tinygltf/stb_image.h"
#include "types.h"
#include <string>

namespace render_system {
RenderDefaults::RenderDefaults(std::string_view checkerTexture)
    : checkerTexture(0), blackTexture(0), whiteTexture(0) {
  int width = 0;
  int height = 0;
  int numChannels = 0;

  uchar *data = stbi_load(std::string(checkerTexture).c_str(), &width, &height,
                          &numChannels, 0);
  assert(data && "failed to load the checker texture.");
  if (data) {
    GLenum format = GL_RGBA;
    if (numChannels == 1)
      format = GL_RED;
    else if (numChannels == 2)
      format = GL_RG;
    else if (numChannels == 3)
      format = GL_RGB;
    this->checkerTexture = loadTexture(data, width, height, format);
  }
  this->blackTexture = loadTexture((const uchar[]){0, 0, 0, 0}, 1, 1, GL_RGBA);
  this->whiteTexture =
      loadTexture((const uchar[]){255, 255, 255, 255}, 1, 1, GL_RGBA);
}

GLuint RenderDefaults::loadTexture(const uchar *data, int width, int height,
                                   GLenum format) {
  GLuint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texId;
}

RenderDefaults::~RenderDefaults() {}
} // namespace render_system
