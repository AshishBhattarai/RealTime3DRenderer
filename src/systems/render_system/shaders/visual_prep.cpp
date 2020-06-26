#include "visual_prep.h"
#include "config.h"

namespace render_system::shader {
VisualPrep::VisualPrep(const StageCodeMap &codeMap) : Program(codeMap) {
  bind();
  glUniform1i(visualprep::fragment::TEXTURE_LOC, TEXTURE_UNIT);
  setExposure(DEFAULT_EXPOSURE);
  setGamma(DEFAULT_GAMMA);
  unBind();
}

void VisualPrep::setExposure(float exposure) {
  glUniform1f(visualprep::fragment::EXPOSURE_LOC, exposure);
}

void VisualPrep::setGamma(float gamma) {
  glUniform1f(visualprep::fragment::GAMMA_LOC, gamma);
}

} // namespace render_system::shader
