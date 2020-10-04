#pragma once

#include "uniform_buffer.h"
#include <glm/gtc/type_ptr.hpp>

// general vertex shader data
namespace render_system::shader {
class GeneralVSUBO : public UniformBuffer {
private:                                                     // Bytes
  static constexpr int PROJECTION_MAT_OFFSET = 0;            // 0
  static constexpr int VIEW_MAT_OFFSET = SIZE_MATRIX4;       // 64
  static constexpr int CAMERA_POS_OFFSET = 2 * SIZE_MATRIX4; // 128

  // total size
  static constexpr int TOTAL_SIZE = CAMERA_POS_OFFSET + SIZE_VEC3; // 144 bytes

public:
  GeneralVSUBO();

  void setProjectionMatrix(const glm::mat4 &mat) {
    setBufferData(glm::value_ptr(mat), PROJECTION_MAT_OFFSET, SIZE_MATRIX4);
  }

  void setViewMatrix(const glm::mat4 &mat) {
    setBufferData(glm::value_ptr(mat), VIEW_MAT_OFFSET, SIZE_MATRIX4);
  }

  void setCameraPos(const glm::vec3 &pos) {
    setBufferData(glm::value_ptr(pos), CAMERA_POS_OFFSET, SIZE_VEC3);
  }
};
} // namespace render_system::shader
