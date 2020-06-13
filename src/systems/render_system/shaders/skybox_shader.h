#pragma once

#include "../texture.h"
#include "program.h"
namespace render_system::shader {

class SkyboxShader : public Program {
public:
  static constexpr uint textureUnit = 0;
  SkyboxShader(const StageCodeMap &codeMap);

  void bindTexture(const Texture &texture) { texture.bind(); }
};
} // namespace render_system::shader
