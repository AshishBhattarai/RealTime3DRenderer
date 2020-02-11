#pragma once

#include "camera.h"
#include "mesh.h"
#include "types.h"
#include <glad/glad.h>
#include <string_view>

class Image;
namespace render_system {
/**
 * @brief The RenderDefaults class
 * Lazy init singleton class to load default render data.
 */
class RenderDefaults : NonCopyable {

public:
  static RenderDefaults &getInstance(const Image *checkerImage = nullptr) {
    static RenderDefaults instance(checkerImage);
    return instance;
  }
  GLuint getCheckerTexture() const { return checkerTexture; }
  GLuint getWhiteTexture() const { return whiteTexture; }
  GLuint getBlackTexture() const { return blackTexture; }
  const Camera &getCamera() const { return camera; }
  const FlatMaterial &getFlatMaterial() const { return flatMaterial; }
  const Material &getMaterial() const { return material; }

private:
  // 4x4 chekerboard texture
  GLuint checkerTexture;
  // 1x1 black & white textures;
  GLuint blackTexture;
  GLuint whiteTexture;

  FlatMaterial flatMaterial;
  Material material;

  Camera camera;

  RenderDefaults(const Image *checkerImage);
  ~RenderDefaults();

  GLuint loadTexture(const uchar *data, int width, int height, GLenum format);
};

} // namespace render_system
