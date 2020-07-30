#include "visual_prep.h"
#include "../texture.h"
#include "config.h"

namespace render_system::shader {
VisualPrep::VisualPrep(const StageCodeMap &codeMap) : Program(codeMap) {
  bind();
  setExposure(DEFAULT_EXPOSURE);
  setGamma(DEFAULT_GAMMA);
  unBind();
}

void VisualPrep::setTexture(const Texture &texture) {
  glActiveTexture(GL_TEXTURE0 + visualprep::fragment::TEXTURE_UNIT);
  texture.bind();
}

void VisualPrep::setExposure(float exposure) {
  glUniform1f(visualprep::fragment::EXPOSURE_LOC, exposure);
}

void VisualPrep::setGamma(float gamma) {
  glUniform1f(visualprep::fragment::GAMMA_LOC, gamma);
}

} // namespace render_system::shader
