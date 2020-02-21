#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace component {
struct Transform {
  glm::mat4 transformMat;

  Transform(const glm::mat4 &transformMat = glm::mat4(1.0f))
      : transformMat(transformMat) {}

  Transform(const glm::vec3 &translate = glm::vec3(0.0f),
            const glm::vec3 &rotate = glm::vec3(0.0f),
            const glm::vec3 &scale = glm::vec3(1.0f)) {
    transformMat = glm::translate(glm::mat4(1.0f), translate) *
                   glm::eulerAngleXYZ(rotate.x, rotate.y, rotate.z) *
                   glm::scale(scale);
  }

  glm::vec3 position() const { return glm::vec3(transformMat[3]); }

  glm::vec3 rotation() const {
    glm::vec3 eularAngles;
    glm::extractEulerAngleXYZ(transformMat, eularAngles[0], eularAngles[1],
                              eularAngles[2]);
    return eularAngles;
  }

  glm::vec3 scale() const {
    // lenght of first three column vectors
    float x = glm::length(glm::vec3(transformMat[0]));
    float y = glm::length(glm::vec3(transformMat[1]));
    float z = glm::length(glm::vec3(transformMat[2]));
    return glm::vec3(x, y, z);
  }

  void position(const glm::vec3 &value) {
    transformMat[3] = glm::vec4(value, transformMat[3].w);
  }

  void rotation(const glm::vec3 &eulerRad) {
    const glm::vec3 &tv = position();
    const glm::vec3 &sv = scale();
    transformMat = glm::translate(glm::mat4(1.0f), tv) *
                   glm::yawPitchRoll(eulerRad.x, eulerRad.y, eulerRad.z) *
                   glm::scale(sv);
  }

  void scale(const glm::vec3 &value) {
    const glm::vec3 &tv = position();
    const glm::vec3 &tr = rotation();
    transformMat = glm::translate(glm::mat4(1.0f), tv) *
                   glm::yawPitchRoll(tr.x, tr.y, tr.z) * glm::scale(value);
  }
};
} // namespace component
