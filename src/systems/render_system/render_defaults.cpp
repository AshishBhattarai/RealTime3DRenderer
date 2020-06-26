#include "render_defaults.h"
#include "core/image.h"
#include "texture.h"
#include "types.h"
#include <glad/glad.h>
#include <string>

namespace render_system {
RenderDefaults::RenderDefaults(const Image *checkerImage)
    : checkerTexture(0), blackTexture(0), whiteTexture(0),
      camera(glm::vec3(0, 0, -10.0f)) {

  assert(checkerImage && "Invalid checker image received.");

  const uchar black[] = {255, 255, 255, 255};
  const uchar white[] = {0, 0, 0, 0};

  this->checkerTexture =
      Texture(*checkerImage, toUnderlying(TextureFlags::DISABLE_MIPMAP))
          .release();
  this->blackTexture = Texture(Image(Buffer(black, 4, 0), 1, 1, 4),
                               toUnderlying(TextureFlags::DISABLE_MIPMAP))
                           .release();
  this->whiteTexture = Texture(Image(Buffer(white, 4, 0), 1, 1, 4),
                               toUnderlying(TextureFlags::DISABLE_MIPMAP))
                           .release();

  // load cube
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

  unsigned int indices[] = {
      0,  1,  2,  2,  3,  0,  //  (front)
      4,  5,  6,  6,  7,  4,  //  (right)
      8,  9,  10, 10, 11, 8,  //  (top)
      12, 13, 14, 14, 15, 12, //  (left)
      16, 17, 18, 18, 19, 16, //  (bottom)
      20, 21, 22, 22, 23, 20  //  (back)
  };

  glGenVertexArrays(1, &cube);
  GLuint vbo[2]; // vbo[0] cube vbo, vbo[1] cube ibo
  glGenBuffers(2, vbo);

  // load data
  glBindVertexArray(cube);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
               GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // set vertex attrib pointers
  glEnableVertexAttribArray(shader::vertex::attribute::POSITION_LOC);
  glVertexAttribPointer(shader::vertex::attribute::POSITION_LOC, 3, GL_FLOAT,
                        GL_FALSE, 0, (void *)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(2, vbo);

  // load plane
  float planeVertices[] = {
      -1.0f, 1.0f,  // Top Left
      -1.0f, -1.0f, // Bottom Left
      1.0f,  1.0f,  // Top Right
      1.0f,  -1.0f  // bottom Right
  };
  GLuint planeVbo = 0;
  glGenVertexArrays(1, &plane);
  glGenBuffers(1, &planeVbo);

  glBindVertexArray(plane);
  glBindBuffer(GL_ARRAY_BUFFER, planeVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
               GL_STATIC_DRAW);
  // set vertex attribe pointers
  glEnableVertexAttribArray(shader::vertex::attribute::POSITION_LOC);
  glVertexAttribPointer(shader::vertex::attribute::POSITION_LOC, 2, GL_FLOAT,
                        GL_FALSE, 0, (void *)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &planeVbo);
} // namespace render_system

RenderDefaults::~RenderDefaults() {
  glDeleteTextures(1, &checkerTexture);
  glDeleteTextures(1, &blackTexture);
  glDeleteTextures(1, &whiteTexture);
  checkerTexture = 0;
  blackTexture = 0;
  whiteTexture = 0;
}
} // namespace render_system
