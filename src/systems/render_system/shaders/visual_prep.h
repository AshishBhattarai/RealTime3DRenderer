#pragma once
#include "program.h"
#include "types.h"

namespace render_system::shader {
class VisualPrep : public Program {
public:
  static constexpr uint TEXTURE_UNIT = 0;
  static constexpr float DEFAULT_EXPOSURE = 1.5f;
  static constexpr float DEFAULT_GAMMA = 2.2f;

  VisualPrep(const StageCodeMap &codeMap);
  void setExposure(float exposure);
  void setGamma(float gamma);
};
} // namespace render_system::shader
