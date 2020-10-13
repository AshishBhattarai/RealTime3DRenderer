#include "grid_plane.h"
#include "config.h"
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace render_system::shader {

GridPlane::GridPlane(const StageCodeMap &codeMap) : Program(codeMap), gridMode(0) {
  // default
  bind();
  loadScale(1000.0);
  loadPlaneColor(glm::vec3(1.0f, 1.0f, 1.0f));
  loadGridColor(glm::vec3(0.6f, 0.6f, 0.6f));
  loadDiscardMode(false);
  loadDistanceMode(true);
  loadDistanceLimit(80);
  unBind();
}
void GridPlane::loadScale(float scale) {
  glUniformMatrix4fv(gridPlane::vertex::uniform::TRANSFORMATION_MAT_LOC, 1, GL_FALSE,
                     glm::value_ptr(glm::eulerAngleXYZ(glm::radians(-90.0f), 0.0f, 0.0f) *
                                    glm::scale(glm::vec3(scale, scale, scale))));
}
void GridPlane::loadPlaneColor(const glm::vec3 &color) {
  glUniform3fv(gridPlane::fragment::PLANE_COLOR_LOC, 1, glm::value_ptr(color));
}
void GridPlane::loadGridColor(const glm::vec3 &color) {
  glUniform3fv(gridPlane::fragment::GRID_COLOR_LOC, 1, glm::value_ptr(color));
}
void GridPlane::loadDiscardMode(bool discard) {
  gridMode = discard ? gridMode | 0x2 : gridMode & ~0x2;
  glUniform1i(gridPlane::fragment::GRID_MODE_LOC, gridMode);
}
void GridPlane::loadDistanceMode(bool distance) {
  gridMode = distance ? gridMode | 0x1 : gridMode & ~0x1;
  glUniform1i(gridPlane::fragment::GRID_MODE_LOC, gridMode);
}
void GridPlane::loadDistanceLimit(int limit) {
  glUniform1i(gridPlane::fragment::DISTANCE_LIMIT_LOC, limit);
}

} // namespace render_system::shader
