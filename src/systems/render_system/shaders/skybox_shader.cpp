#include "skybox_shader.h"
#include "config.h"

namespace render_system::shader {
SkyboxShader::SkyboxShader(const StageCodeMap &codeMap) : Program(codeMap) {
  bind();
  glUniform1i(shader::skybox::fragment::TEXTURE_LOC, textureUnit);
  unBind();
}
} // namespace render_system::shader
