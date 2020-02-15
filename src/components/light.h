#pragma once
#include <glm/glm.hpp>

enum class LightType { POINT_LIGHT, DIRECTION_LIGHT, SPOT_LIGHT, AREA_LIGHT };

namespace component {
struct Light {
  glm::vec3 color;
  float range;
  LightType type;
};

} // namespace component
