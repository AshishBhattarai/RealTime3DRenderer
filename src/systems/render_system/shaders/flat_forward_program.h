#pragma once

#include "../mesh.h"
#include "../point_light.h"
#include "config.h"
#include "program.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

namespace render_system::shader {
class FlatForwardProgram : public Program {
public:
  FlatForwardProgram(const StageCodeMap &codeMap);

  void loadProjectionMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(vertex::uniform::PROJECTION_LOC, 1, GL_FALSE,
                       glm::value_ptr(matrix));
  }

  void loadViewMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(vertex::uniform::VIEW_LOC, 1, GL_FALSE,
                       glm::value_ptr(matrix));
  }

  void loadTransformMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(vertex::uniform::TRANSFORMATION_LOC, 1, GL_FALSE,
                       glm::value_ptr(matrix));
  }

  void loadPointLight(const PointLight &pointLight, uint idx) {
    assert(idx < fragment::PointLight::MAX && "Invalid point light index");
    glUniform3fv(fragment::uniform::POINT_LIGHT_LOC[idx] +
                     fragment::PointLight::POSITION,
                 1, glm::value_ptr(pointLight.position));
    glUniform3fv(fragment::uniform::POINT_LIGHT_LOC[idx] +
                     fragment::PointLight::COLOR,
                 1, glm::value_ptr(pointLight.color));
    glUniform1f(fragment::uniform::POINT_LIGHT_LOC[idx] +
                    fragment::PointLight::RADIUS,
                pointLight.radius);
  }

  void loadCameraPosition(const glm::vec3 &pos) {
    glUniform3fv(fragment::uniform::CAM_POS_LOC, 1, glm::value_ptr(pos));
  }

  void loadPointLightSize(int size) {
    glUniform1i(fragment::uniform::POINT_LIGHT_SIZE_LOC, size);
  }

  void loadMaterial(const FlatMaterial &material) {
    glUniform3fv(fragment::uniform::PBR_ALBEDO_LOC, 1,
                 glm::value_ptr(material.albedo));
    glUniform1f(fragment::uniform::PBR_METALLIC_LOC, material.metallic);
    glUniform1f(fragment::uniform::PBR_ROUGHNESS_LOC, material.roughtness);
    glUniform1f(fragment::uniform::PBR_AO_LOC, material.ao);
  }
};
} // namespace render_system::shader
