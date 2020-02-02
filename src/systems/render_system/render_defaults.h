#pragma once

#include "types.h"
#include <glad/glad.h>
#include <string_view>

namespace render_system {
/**
 * @brief The RenderDefaults class
 * Lazy init singleton class to load default render data.
 */
class RenderDefaults : NonCopyable {

public:
  static RenderDefaults &getInstance(std::string_view checkerTexture) {
    static RenderDefaults instance(checkerTexture);
    return instance;
  }
  GLuint getCheckerTexture() const { return checkerTexture; }
  //  GLuint getWhiteTexture() const { return whiteTexture; }
  GLuint getBlackTexture() const { return blackTexture; }

private:
  // 4x4 chekerboard texture
  GLuint checkerTexture;
  // 1x1 black & white textures;
  GLuint blackTexture;
  GLuint whiteTexture;

  RenderDefaults(std::string_view checkerTexture);
  ~RenderDefaults();

  GLuint loadTexture(const uchar *data, int width, int height, GLenum format);
};

} // namespace render_system
