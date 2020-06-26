#pragma once
#include "frame_buffer.h"
#include "shaders/visual_prep.h"

namespace render_system {
class PostProcessor {
private:
  shader::VisualPrep visualPrep;
  GLuint plane;

public:
  PostProcessor(const shader::StageCodeMap &visualPrep);

  void applyVisualPrep(const FrameBuffer &frameBuffer);
};
} // namespace render_system
