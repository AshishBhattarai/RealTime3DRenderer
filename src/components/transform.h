#pragma once
#include <glm/glm.hpp>

struct Transform {
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;

  Transform(const glm::vec3 &position = glm::vec3(0.0f, 0.0f, 0.0f),
            const glm::vec3 &rotation = glm::vec3(0.0f, 0.0f, 0.0f),
            const glm::vec3 &scale = glm::vec3(1.0f, 1.0f, 1.0f))
      : position(position), rotation(rotation), scale(scale) {}
};
