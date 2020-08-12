#include "flat_forward_material.h"
#include "../mesh.h"
#include "../point_light.h"
#include "../texture.h"
#include "config.h"

namespace render_system::shader {
FlatForwardMaterial::FlatForwardMaterial(const StageCodeMap &codeMap) : Program(codeMap) {}

void FlatForwardMaterial::loadTransformMatrix(const glm::mat4 &matrix) {
  glUniformMatrix4fv(forward::vertex::uniform::TRANSFORMATION_LOC, 1, GL_FALSE,
                     glm::value_ptr(matrix));
}

void FlatForwardMaterial::loadPointLight(const PointLight &pointLight, uint idx) {
  assert(idx < forward::fragment::PointLight::MAX && "Invalid point light index");
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

void FlatForwardMaterial::loadPointLightSize(int size) {
  glUniform1i(forward::fragment::uniform::POINT_LIGHT_SIZE_LOC, size);
}

void FlatForwardMaterial::loadMaterial(const FlatMaterial &material) {
  glUniform3fv(forward::fragment::uniform::flat::PBR_ALBEDO_LOC, 1,
               glm::value_ptr(material.albedo));
  glUniform1f(forward::fragment::uniform::flat::PBR_METALLIC_LOC, material.metallic);
  glUniform1f(forward::fragment::uniform::flat::PBR_ROUGHNESS_LOC, material.roughtness);
  glUniform1f(forward::fragment::uniform::flat::PBR_AO_LOC, material.ao);
}

void FlatForwardMaterial::loadIrradianceMap(const Texture &tex) {
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::PBR_IRRADIANCE_MAP_UNIT);
  tex.bind();
}

void FlatForwardMaterial::loadPrefilteredMap(const Texture &tex) {
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::PBR_PREFILETERED_MAP_UNIT);
  tex.bind();
}
void FlatForwardMaterial::loadBrdfIntegrationMap(const Texture &tex) {
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::PBR_BRDF_INTEGRATION_MAP_UNIT);
  tex.bind();
}
} // namespace render_system::shader
