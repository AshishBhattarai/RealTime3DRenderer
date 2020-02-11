#pragma once

#include "config.h"
#include "program.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

namespace render_system::shader {
class FlatForwardProgram : public Program {
public:
  FlatForwardProgram(const StageCodeMap &codeMap);

  void loadProjectionMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(vertex::uniform::PROJECTION_LOC, 1, GL_FALSE,
                       glm::value_ptr(matrix));
  }

  void loadViewMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(vertex::uniform::VIEW_LOC, 1, GL_FALSE,
                       glm::value_ptr(matrix));
  }

  void loadTransformMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(vertex::uniform::TRANSFORMATION_LOC, 1, GL_FALSE,
                       glm::value_ptr(matrix));
  }
};
} // namespace render_system::shader
