#pragma once
#include <glad/glad.h>

// Base class for all uniform buffers
namespace render_system::shader {
class UniformBuffer {
public:
  /**
   * NOTE: avoid using vec3 on uniform block, use vec4 and swizzle it (vec4.xyz)
   * types base alignment(ie: the space(in bytes) taken by the variable.
   **/
  // basic -> int, float, bool (N) - N = sizeof(float)
  constexpr static int SIZE_BASIC = 4;
  constexpr static int SIZE_VEC2 = 8;                // 2*N
  constexpr static int SIZE_VEC3 = 16;               // 4*N
  constexpr static int SIZE_VEC4 = 16;               // 4*N
  constexpr static int SIZE_MATRIX4 = 4 * SIZE_VEC4; // 16*N
  constexpr static int SIZE_MATRIX3 = 3 * SIZE_VEC4; // 12*N

private:
  GLuint UBO;
  const GLuint totalSize;
  const GLuint bindingIndex;

  // create a buffer of size =
  void createBuffer();

public:
  UniformBuffer(GLuint size, GLuint bindingIndex);
  ~UniformBuffer();

  // set buffer subdata
  void setBufferData(const GLvoid *data, GLuint offset, GLuint size);

  GLuint getUBO() const { return UBO; }
  GLuint getBindingPoint() const { return bindingIndex; }
  GLuint getTotalSize() const { return totalSize; }
};
} // namespace render_system::shader
