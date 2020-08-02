#pragma once
#include "cubemap.h"

namespace render_system {
class Texture;
namespace shader {
class IBLSpecularConvolution : public Cubemap {
public:
  IBLSpecularConvolution(const StageCodeMap &codeMap);
  void loadTexture(const Texture &texture);
  void loadRoughness(float roughness);
};
} // namespace shader
} // namespace render_system
