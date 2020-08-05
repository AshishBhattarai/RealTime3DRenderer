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
private:
  // 4x4 chekerboard texture
  GLuint checkerTexture;
  // 1x1 black & white textures;
  GLuint blackTexture;
  GLuint whiteTexture;
  Camera camera;
  // 1x1 cube model vao
  Primitive cube;
  // 1x1 plane
  Primitive plane;

  RenderDefaults(const Image *checkerImage);
  ~RenderDefaults();

  Primitive loadPrimitive(const float *vertices, uint verticesCount, uint dim, const uint *indices,
                          uint indicesCount, GLenum mode);
  Primitive loadCube();
  Primitive loadPlane();

public:
  static RenderDefaults &getInstance(const Image *checkerImage = nullptr) {
    static RenderDefaults instance(checkerImage);
    return instance;
  }
  GLuint getCheckerTexture() const { return checkerTexture; }
  GLuint getWhiteTexture() const { return whiteTexture; }
  GLuint getBlackTexture() const { return blackTexture; }
  const Primitive& getCube() const { return cube; }
  const Primitive& getPlane() const { return plane; }
  const Camera &getCamera() const { return camera; }
};

} // namespace render_system
