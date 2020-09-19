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
  void loadPlaneColor(const glm::vec3 &color);
  void loadGridColor(const glm::vec3 &color);
  void loadDiscardMode(bool discard);
  void loadDistanceMode(bool distance);
  void loadDistanceLimit(int limit);
};

} // namespace render_system::shader
