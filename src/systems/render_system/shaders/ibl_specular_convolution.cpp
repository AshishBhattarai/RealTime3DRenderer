#include "ibl_specular_convolution.h"
#include "../texture.h"
#include "config.h"

namespace render_system::shader {
IBLSpecularConvolution::IBLSpecularConvolution(const StageCodeMap &stageCodeMap)
    : Cubemap(stageCodeMap) {}

void IBLSpecularConvolution::loadTexture(const Texture &texture) {
  glActiveTexture(GL_TEXTURE0 + iblSpecularConvolution::fragment::ENV_MAP_UNIT);
  texture.bind();
}

void IBLSpecularConvolution::loadRoughness(float roughness) {
  glUniform1f(iblSpecularConvolution::fragment::ROUGHNESS_LOC, roughness);
}
} // namespace render_system::shader
