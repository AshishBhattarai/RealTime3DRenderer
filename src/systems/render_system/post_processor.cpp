#include "post_processor.h"
#include "default_primitives_renderer.h"
#include "render_defaults.h"
#include "texture.h"

namespace render_system {
PostProcessor::PostProcessor(const shader::StageCodeMap &visualPrep) : visualPrep(visualPrep) {}

void PostProcessor::applyVisualPrep(const Texture &texture) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  visualPrep.bind();
  visualPrep.setTexture(texture);
  DefaultPrimitivesRenderer::getInstance().drawPlane();
  glBindVertexArray(0);
}
} // namespace render_system
