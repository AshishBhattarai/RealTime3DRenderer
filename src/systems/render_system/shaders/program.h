#pragma once

#include <cassert>
#include <glad/glad.h>
#include <map>

class Buffer;
namespace render_system::shader {

enum class ShaderStage : short {
  VERTEX_SHADER = 0x0001,
  FRAGMENT_SHADER = 0x0002,
  GEOMETRY_SHADER = 0x0004,
  TESS_CONTROL_SHADER = 0x0008,
  TESS_EVAL_SHADER = 0x0016,
  COMPUTE_SHADER = 0x0032
}; // namespace ShaderStage

using StageCodeMap = std::map<ShaderStage, const Buffer &>;

class Program {
protected:
  GLuint program;

private:
  short shaderStageFlags;
  GLuint createShader(const Buffer &data, GLenum type);

public:
  constexpr static int TEX_UNIT_START = 4;
  // 1 - reserved for diffuse ibl
  constexpr static int TEX_UNIT_DIFF_IBL = 1;

  Program(const StageCodeMap &codeMap);

  void bind() { glUseProgram(program); }
  static void unBind() { glUseProgram(0); }

  // returns GLenum ShaderType equivalent to the ShaderStage
  static GLenum stageToGLenum(ShaderStage stage) {
    switch (stage) {
    case ShaderStage::VERTEX_SHADER:
      return GL_VERTEX_SHADER;
    case ShaderStage::FRAGMENT_SHADER:
      return GL_FRAGMENT_SHADER;
    case ShaderStage::GEOMETRY_SHADER:
      return GL_GEOMETRY_SHADER;
    case ShaderStage::TESS_EVAL_SHADER:
      return GL_TESS_EVALUATION_SHADER;
    case ShaderStage::TESS_CONTROL_SHADER:
      return GL_TESS_CONTROL_SHADER;
    case ShaderStage::COMPUTE_SHADER:
      return GL_COMPUTE_SHADER;
    }
    assert(false && "This shouldn't happen.");
    return GL_INVALID_ENUM;
  }
};

} // namespace render_system::shader
