#pragma once

#include "types.h"

namespace render_system {
struct Primitive;

/**
 * @brief The DefaultPrimitiveRenderer class
 *
 * Singleton
 * We need to render primitives like a screen quad(plane), cube for PreProcessor & PostProcessor.
 * This class provides functionality for rendering those primitve meshes.
 *
 * I can just re-use renderer.renderMesh() for this.
 * Creating a new singleton for it in an attempt to keep everything simple.
 * Might change in the future
 */
class DefaultPrimitivesRenderer : NonCopyable {
private:
  const Primitive &cube;
  const Primitive &plane;
  DefaultPrimitivesRenderer(const Primitive &cube, const Primitive &plane);

public:
  static DefaultPrimitivesRenderer &getInstance(const Primitive *const cube = nullptr,
                                                const Primitive *const plane = nullptr) {
    static DefaultPrimitivesRenderer instance(*cube, *plane);
    return instance;
  }

  void drawCube();
  void drawPlane();
};
} // namespace render_system
