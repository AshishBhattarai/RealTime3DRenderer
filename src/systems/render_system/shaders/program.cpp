#include "program.h"
#include "core/buffer.h"
#include "types.h"
#include "utils/slogger.h"
#include <vector>

namespace render_system::shader {

GLuint Program::createShader(const Buffer &data, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, data.data(),
                 data.getSize());
  // set shade entry point, with argc & argv
  glSpecializeShader(shader, "main", 0, 0, 0);

  int success = 0;
  char infoLog[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    SLOG("Failed to compile", type, infoLog);
    glDeleteShader(shader);
    shader = 0;
    assert(success && "Shader compile failed.");
  }
  return shader;
}

Program::Program(const StageCodeMap &codeMap)
    : program(0), shaderStageFlags(0) {

  // compile shaders
  std::vector<GLuint> shaders;
  for (const auto &pair : codeMap) {
    GLenum shaderType = stageToGLenum(pair.first);
    shaders.emplace_back(createShader(pair.second, shaderType));
    shaderStageFlags = shaderStageFlags | toUnderlying<ShaderStage>(pair.first);
  }

  // create, attach & link program
  program = glCreateProgram();
  for (const GLuint &shader : shaders) {
    glAttachShader(program, shader);
  }

  glLinkProgram(program);

  // check for link errors
  int success = 0;
  char infoLog[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    SLOG("Failed to link shader program.", infoLog);
    glDeleteProgram(program);
    program = 0;
    assert(false && "Link program failed.");
  }

  // cleanup
  for (const GLuint &shader : shaders) {
    glDeleteShader(shader);
  }
}
} // namespace render_system::shader
