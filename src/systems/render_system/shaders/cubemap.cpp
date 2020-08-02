#include "cubemap.h"
#include "../texture.h"
#include "config.h"
#include <glm/gtc/type_ptr.hpp>

namespace render_system::shader {
Cubemap::Cubemap(const StageCodeMap &stageCodeMap) : Program(stageCodeMap) {}

void Cubemap::loadTexture(const Texture &texture) {
  glActiveTexture(GL_TEXTURE0 + cubemap::fragment::TEXTURE_UNIT);
  texture.bind();
}

void Cubemap::loadProjection(const glm::mat4 &projection) {
  glUniformMatrix4fv(cubemap::vertex::PROJECTION_MAT_LOC, 1, false,
                     glm::value_ptr(projection));
}

void Cubemap::loadView(const glm::mat4 &view) {
  glUniformMatrix4fv(cubemap::vertex::VIEW_MAT_LOC, 1, false,
                     glm::value_ptr(view));
}
} // namespace render_system::shader
