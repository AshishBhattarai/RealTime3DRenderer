#pragma once
#include <glm/glm.hpp>

enum class LightType { POINT_LIGHT, DIRECTION_LIGHT, SPOT_LIGHT, AREA_LIGHT };

namespace component {
struct Light {
  glm::vec3 color;
  float intensity;
  float range;
  LightType type;

  Light(const glm::vec3 &color, const float &intensity, const float &range,
        const LightType &type)
      : color(color), intensity(intensity), range(range), type(type) {}
};

} // namespace component
