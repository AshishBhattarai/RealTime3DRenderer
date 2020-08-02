#pragma once

#include "program.h"
#include <glm/glm.hpp>

namespace render_system {
class Texture;
namespace shader {
class Cubemap : public Program {
public:
  Cubemap(const StageCodeMap &stageCodeMap);

  void loadTexture(const Texture &texture);
  void loadProjection(const glm::mat4 &projection);
  void loadView(const glm::mat4 &view);
};
} // namespace shader
} // namespace render_system
