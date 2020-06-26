#include "post_processor.h"
#include "render_defaults.h"

namespace render_system {
PostProcessor::PostProcessor(const shader::StageCodeMap &visualPrep)
    : visualPrep(visualPrep),
      plane(RenderDefaults::getInstance().getPlaneVao()) {}

void PostProcessor::applyVisualPrep(const FrameBuffer &frameBuffer) {
  auto textureId = frameBuffer.getColorAttachmentId(0);
  visualPrep.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0 + visualPrep.TEXTURE_UNIT);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glBindVertexArray(plane);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
} // namespace render_system
