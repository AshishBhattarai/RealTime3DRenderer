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

  void loadTransformMatrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(forward::vertex::uniform::TRANSFORMATION_LOC, 1,
                       GL_FALSE, glm::value_ptr(matrix));
  }

  void loadPointLight(const PointLight &pointLight, uint idx) {
    assert(idx < forward::fragment::PointLight::MAX &&
           "Invalid point light index");
    glUniform3fv(forward::fragment::uniform::POINT_LIGHT_LOC[idx] +
                     forward::fragment::PointLight::POSITION,
                 1, glm::value_ptr(*pointLight.position));
    glUniform3fv(forward::fragment::uniform::POINT_LIGHT_LOC[idx] +
                     forward::fragment::PointLight::COLOR,
                 1, glm::value_ptr(*pointLight.color));
    glUniform1f(forward::fragment::uniform::POINT_LIGHT_LOC[idx] +
                    forward::fragment::PointLight::RADIUS,
                *pointLight.radius);
    glUniform1f(forward::fragment::uniform::POINT_LIGHT_LOC[idx] +
                    forward::fragment::PointLight::INTENSITY,
                *pointLight.intensity);
  }

  void loadPointLightSize(int size) {
    glUniform1i(forward::fragment::uniform::POINT_LIGHT_SIZE_LOC, size);
  }

  void loadMaterial(const FlatMaterial &material) {
    glUniform3fv(forward::fragment::uniform::PBR_ALBEDO_LOC, 1,
                 glm::value_ptr(material.albedo));
    glUniform1f(forward::fragment::uniform::PBR_METALLIC_LOC,
                material.metallic);
    glUniform1f(forward::fragment::uniform::PBR_ROUGHNESS_LOC,
                material.roughtness);
    glUniform1f(forward::fragment::uniform::PBR_AO_LOC, material.ao);
  }
};
} // namespace render_system::shader
