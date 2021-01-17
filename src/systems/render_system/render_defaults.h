#pragma once

#include "camera.h"
#include "core/image.h"
#include "mesh.h"
#include "types.h"
#include <glad/glad.h>
#include <string_view>

namespace render_system {
/**
 * @brief The RenderDefaults class
 * Lazy init singleton class to load default render data.
 */
class RenderDefaults : NonCopyable {
private:
  static constexpr uchar white[] = {255, 255, 255, 255};
  static constexpr uchar black[] = {0, 0, 0, 0};

  Image gridImage;
  Image checkerImage;
  Image blackImage;
  Image whiteImage;
  Camera camera;
  // 1x1 cube model vao
  Primitive cube;
  // 1x1 plane
  Primitive plane;

  RenderDefaults(const Image *gridImage, const Image *checkerImage);
  ~RenderDefaults();

  Primitive loadPrimitive(const float *vertices, uint verticesCount, uint dim, const uint *indices,
                          uint indicesCount, GLenum mode);
  Primitive loadCube();
  Primitive loadPlane();

public:
  static RenderDefaults &getInstance(const Image *gridImage = nullptr,
                                     const Image *checkerImage = nullptr) {
    static RenderDefaults instance(gridImage, checkerImage);
    return instance;
  }

  Texture createGridTexture() { return Texture(gridImage, toUnderlying(TextureFlags::REPEATE)); }

  /**
   * @brief Creates a new checker texture.
   *
   * @return Texture
   */
  Texture createCheckerTexture() { return Texture(checkerImage); }

  /**
   * @brief Creates a new white texture.
   *
   * @return Texture
   */
  Texture createWhiteTexture() { return Texture(whiteImage); }

  /**
   * @brief Creates a new black texture.
   *
   * @return Texture
   */
  Texture createBlackTexture() { return Texture(blackImage); }

  const Primitive &getCube() const { return cube; }
  const Primitive &getPlane() const { return plane; }
  const Camera &getCamera() const { return camera; }
};

} // namespace render_system
