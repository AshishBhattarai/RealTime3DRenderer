#include "gui_shader.h"
#include "../texture.h"
#include "config.h"
#include <glm/gtc/type_ptr.hpp>

namespace render_system::shader {

GuiShader::GuiShader(const StageCodeMap &codeMap) : Program(codeMap) {
  bind();
  loadLod(0);
  loadFace(0);
  unBind();
}

void GuiShader::loadProjectionMat(const glm::mat4 &projection) {
  glUniformMatrix4fv(gui::vertex::uniform::PROJECTION_MAT_LOC, 1, GL_FALSE,
                     glm::value_ptr(projection));
}

void GuiShader::loadTexture(const Texture &texture) {
  glActiveTexture(GL_TEXTURE0 + gui::fragment::TEXTURE_BND);
  texture.bind();
}

void GuiShader::loadLod(const float lod) { glUniform1f(gui::fragment::LOD_LOC, lod); }
void GuiShader::loadFace(const int face) { glUniform1i(gui::fragment::FACE_LOC, face); }

} // namespace render_system::shader
