#include "uniform_buffer.h"
#include "common.h"

namespace render_system::shader {
UniformBuffer::UniformBuffer(GLuint size, GLuint bindingIndex)
    : totalSize(size), bindingIndex(bindingIndex) {
  createBuffer();
}

// free buffer
UniformBuffer::~UniformBuffer() {
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glDeleteBuffers(1, &UBO);
}

void UniformBuffer::createBuffer() {
  glGenBuffers(1, &UBO);
  // reserver space
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, totalSize, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  // set binding point
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, UBO);
}

void UniformBuffer::setBufferData(const GLvoid *data, GLuint offset,
                                  GLuint size) {
  // check if buffer has enough space;
  assert((size < totalSize) && "Cannot set UBO, Not enough space.");
  // set buffer
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
} // namespace render_system::shader
