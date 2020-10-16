#include "render_defaults.h"
#include "core/image.h"
#include "texture.h"
#include "types.h"
#include <glad/glad.h>
#include <string>

namespace render_system {
RenderDefaults::RenderDefaults(const Image *gridImage, const Image *checkerImage)
    : gridImage(*gridImage), checkerImage(*checkerImage), blackImage(Buffer(black, 4), 1, 1, 4),
      whiteImage(Buffer(white, 4), 1, 1, 4), camera(glm::vec3(0, 0, -10.0f)), cube(loadCube()),
      plane(loadPlane()) {

  assert(gridImage && checkerImage && "Invalid checker or grid image received.");
} // namespace render_system

Primitive RenderDefaults::loadCube() {
  float cubeVertices[] = {
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f, // (front)
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f, // (right)
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, // (top)
      -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, // (left)
      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
      1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, // (bottom)
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f // (back)
  };
  uint cubeIndices[] = {
      0,  1,  2,  2,  3,  0,  //  (front)
      4,  5,  6,  6,  7,  4,  //  (right)
      8,  9,  10, 10, 11, 8,  //  (top)
      12, 13, 14, 14, 15, 12, //  (left)
      16, 17, 18, 18, 19, 16, //  (bottom)
      20, 21, 22, 22, 23, 20  //  (back)
  };
  return loadPrimitive(cubeVertices, sizeof(cubeVertices) / sizeof(float), 3, cubeIndices,
                       sizeof(cubeIndices) / sizeof(uint), GL_TRIANGLES);
}

Primitive RenderDefaults::loadPlane() {
  float planeVertices[] = {
      -1.0f, 1.0f,  // Top Left
      -1.0f, -1.0f, // Bottom Left
      1.0f,  1.0f,  // Top Right
      1.0f,  -1.0f  // bottom Right
  };
  return loadPrimitive(planeVertices, sizeof(planeVertices) / sizeof(float), 2, nullptr, 0,
                       GL_TRIANGLE_STRIP);
}

Primitive RenderDefaults::loadPrimitive(const float *vertices, uint verticesCount, uint dim,
                                        const uint *indices, uint indicesCount, GLenum mode) {
  assert(vertices && "Atleast vertices must be valid.");

  GLuint vao = 0, vbo = 0, ibo = 0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesCount, vertices, GL_STATIC_DRAW);
  if (indices) {
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indicesCount, indices, GL_STATIC_DRAW);
  }
  // set vertex attribe pointers
  glEnableVertexAttribArray(shader::vertex::attribute::POSITION_LOC);
  glVertexAttribPointer(shader::vertex::attribute::POSITION_LOC, dim, GL_FLOAT, GL_FALSE, 0,
                        (void *)0);
  glBindVertexArray(0);
  if (indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ibo);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &vbo);
  if (indices)
    return {vao, mode, GL_UNSIGNED_INT, (GLsizei)indicesCount, (void *)0};
  else
    return {vao, mode, 0, (GLsizei)verticesCount / 2, (void *)0};
}

RenderDefaults::~RenderDefaults() {
  glDeleteVertexArrays(1, &cube.vao);
  glDeleteVertexArrays(1, &plane.vao);
}
} // namespace render_system
