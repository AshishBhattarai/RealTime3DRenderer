#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace component {
class Transform {
private:
  glm::vec3 position_;
  glm::vec3 scale_;
  glm::quat rotation_;

public:
  Transform(const glm::vec3 &position = glm::vec3(0.0f),
            const glm::vec3 &rotation = glm::vec3(0.0f), const glm::vec3 &scale = glm::vec3(1.0f))
      : position_(position), scale_(scale) {
    glm::quat quatX = glm::angleAxis(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat quatY = glm::angleAxis(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quatZ = glm::angleAxis(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    rotation_ = quatX * quatY * quatZ;
  }

  Transform(const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
      : position_(position), rotation_(rotation), scale_(scale) {}

  glm::vec3 position() const { return position_; }

  glm::vec3 rotationEuler() const { return glm::eulerAngles(rotation_); }
  glm::quat rotation() const { return rotation_; }
  glm::vec3 scale() const { return scale_; }

  void position(const glm::vec3 &position) { position_ = position; }

  void rotationEuler(const glm::vec3 &rotation) {
    glm::quat quatX = glm::angleAxis(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat quatY = glm::angleAxis(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat quatZ = glm::angleAxis(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    rotation_ = quatX * quatY * quatZ;
  }

  void scale(const glm::vec3 &scale) { scale_ = scale; }

  glm::mat4 transformation() {
    auto transform = glm::translate(glm::mat4(1.0f), position_);
    transform *= glm::mat4_cast(rotation_);
    return glm::scale(transform, scale_);
  }
};
} // namespace component
