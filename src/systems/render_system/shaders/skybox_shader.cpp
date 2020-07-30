#include "skybox_shader.h"
#include "config.h"

namespace render_system::shader {
SkyboxShader::SkyboxShader(const StageCodeMap &codeMap) : Program(codeMap) {}

void SkyboxShader::bindTexture(const Texture &texture) {
  glActiveTexture(GL_TEXTURE0 + skybox::fragment::TEXTURE_UNIT);
  texture.bind();
}
} // namespace render_system::shader
