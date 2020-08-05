#pragma once
#include "frame_buffer.h"
#include "shaders/visual_prep.h"

namespace render_system {
class PostProcessor {
private:
  shader::VisualPrep visualPrep;

public:
  PostProcessor(const shader::StageCodeMap &visualPrep);

  void applyVisualPrep(const Texture &texture);
};
} // namespace render_system
