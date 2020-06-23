#include "skybox_shader.h"
#include "config.h"

namespace render_system::shader {
SkyboxShader::SkyboxShader(const StageCodeMap &codeMap) : Program(codeMap) {
  glUniformBlockBinding(this->program,
                        shader::forward::vertex::uniform::GENERAL_UB_LOC,
                        shader::vertex::generalUBOBinding);
  bind();
  glUniform1i(shader::skybox::fragment::TEXTURE_LOC, textureUnit);
  unBind();
}
} // namespace render_system::shader
