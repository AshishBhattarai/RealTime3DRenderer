#include "flat_forward_program.h"

namespace render_system::shader {
FlatForwardProgram::FlatForwardProgram()
    : Program(
          {{ShaderStage::VERTEX_SHADER, "shaders/flat_forward_vert.spv"},
           {ShaderStage::FRAGMENT_SHADER, "shaders/flat_forward_frag.spv"}}) {}
} // namespace render_system::shader
