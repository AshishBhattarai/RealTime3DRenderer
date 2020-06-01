#pragma once
#include "types.h"
#include <glad/glad.h>

// Image/Texture medium converts to or from opengl texture

class Image;
namespace render_system {
class Texture {
private:
  friend class RenderDefaults;
  friend class RenderSystem;
  GLuint id;

  int moveId() {
    GLuint temp = id;
    id = 0;
    return temp;
  }

  void loadTexture(const uchar *buffer, int width, int height, int numChannels);

public:
  Texture(const Image &image);
  void loadTexture(const Image &image);
};

} // namespace render_system
