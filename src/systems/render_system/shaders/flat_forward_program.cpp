#include "flat_forward_program.h"

namespace render_system::shader {
FlatForwardProgram::FlatForwardProgram(const StageCodeMap &codeMap)
    : Program(codeMap) {

  glUniformBlockBinding(this->program,
                        shader::forward::vertex::uniform::GENERAL_UB_LOC,
                        shader::vertex::generalUBOBinding);
}
} // namespace render_system::shader
