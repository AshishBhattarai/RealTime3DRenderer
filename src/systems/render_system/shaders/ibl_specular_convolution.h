#pragma once
#include "program.h"

namespace render_system {
class Texture;
namespace shader {
class IBLSpecularConvolution : public Program {
public:
  IBLSpecularConvolution(const StageCodeMap &codeMap);
  void envMap(const Texture &texture);
  void roughness(float roughness);
};
} // namespace shader
} // namespace render_system
