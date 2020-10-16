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
  loadScale(400.0);
  unBind();
}
void GridPlane::loadScale(float scale) {
  glUniformMatrix4fv(gridPlane::vertex::uniform::TRANSFORMATION_MAT_LOC, 1, GL_FALSE,
                     glm::value_ptr(glm::eulerAngleXYZ(glm::radians(-90.0f), 0.0f, 0.0f) *
                                    glm::scale(glm::vec3(scale, scale, scale))));
}
} // namespace render_system::shader
