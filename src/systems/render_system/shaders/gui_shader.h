#pragma once
#include "program.h"
#include <glm/mat4x4.hpp>

namespace render_system {
class Texture;
namespace shader {
class GuiShader : public Program {
public:
  GuiShader(const StageCodeMap &codeMap);
  void loadProjectionMat(const glm::mat4 &projection);
  void loadTexture(const Texture &texture);
  void loadLod(const float lod);
  void loadFace(const int face);
};

} // namespace shader
} // namespace render_system
