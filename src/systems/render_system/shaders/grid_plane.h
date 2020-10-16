#pragma once

#include "program.h"
#include <glm/glm.hpp>

namespace render_system::shader {

class GridPlane : public Program {
private:
  int gridMode; // bits 0th distance, 1st discard

public:
  GridPlane(const StageCodeMap &codeMap);
  void loadScale(float scale);
};

} // namespace render_system::shader
