#include "post_processor.h"
#include "render_defaults.h"
#include "texture.h"

namespace render_system {
PostProcessor::PostProcessor(const shader::StageCodeMap &visualPrep)
    : visualPrep(visualPrep),
      plane(RenderDefaults::getInstance().getPlaneVao()) {}

void PostProcessor::applyVisualPrep(const Texture &texture) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  visualPrep.bind();
  visualPrep.setTexture(texture);
  glBindVertexArray(plane);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
} // namespace render_system
