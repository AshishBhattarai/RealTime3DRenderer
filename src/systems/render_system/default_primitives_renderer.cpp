#include "default_primitives_renderer.h"
#include "mesh.h"

namespace render_system {
DefaultPrimitivesRenderer::DefaultPrimitivesRenderer(const Primitive &cube, const Primitive &plane)
    : cube(cube), plane(plane) {}

void DefaultPrimitivesRenderer::drawCube() {
  glBindVertexArray(cube.vao);
  glDrawElements(cube.mode, cube.indexCount, cube.indexType, cube.indexOffset);
}

void DefaultPrimitivesRenderer::drawPlane() {
  glBindVertexArray(plane.vao);
  /* kinda hacky, not using indices coz with GL_TRIANGLE_STRIP its only 4 vertices. */
  glDrawArrays(plane.mode, 0, plane.indexCount);
}
} // namespace render_system
