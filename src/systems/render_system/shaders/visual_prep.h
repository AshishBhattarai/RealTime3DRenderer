#pragma once
#include "program.h"
#include "types.h"

namespace render_system {
class Texture;
namespace shader {
class VisualPrep : public Program {
public:
  static constexpr float DEFAULT_EXPOSURE = 1.5f;
  static constexpr float DEFAULT_GAMMA = 2.2f;

  VisualPrep(const StageCodeMap &codeMap);
  void setTexture(const Texture &texture);
  void setExposure(float exposure);
  void setGamma(float gamma);
};
} // namespace shader
} // namespace render_system
