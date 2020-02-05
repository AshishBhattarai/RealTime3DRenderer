#include "program.h"
#include "types.h"
#include "utils/slogger.h"
#include <vector>

namespace render_system::shader {

GLuint Program::createShader(std::string_view path, GLenum type) {
  char *source;
  size_t size = 0;
  std::ifstream file(std::string(path).c_str(),
                     std::ios::binary | std::ios::ate);
  // read from file
  if (file.is_open()) {
    size = file.tellg();
    source = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(source, size);
    file.close();
  }
  // check for errors
  if (file.fail()) {
    SLOG("Failed to read form shader source file.", path);
    assert(false && "Failed to read shader source.");
    return 0;
  }

  GLuint shader = glCreateShader(type);
  glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, source, size);
  delete[] source;
  return shader;
}

Program::Program(std::map<ShaderStage, std::string_view> shaderPaths)
    : program(0), shaderStageFlags(0) {

  // compile shaders
  std::vector<GLuint> shaders;
  for (const auto &pair : shaderPaths) {
    GLenum shaderType = stageToGLenum(pair.first);
    shaders.emplace_back(createShader(pair.second, shaderType));
  }

  // create, attach & link program
  program = glCreateProgram();
  for (const GLuint &shader : shaders) {
    glAttachShader(program, shader);
  }
  glLinkProgram(program);

  // cleanup
  for (const GLuint &shader : shaders) {
    glDeleteShader(shader);
  }

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
}
} // namespace render_system::shader
