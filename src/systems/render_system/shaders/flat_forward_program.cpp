#include "flat_forward_program.h"

namespace render_system::shader {
FlatForwardProgram::FlatForwardProgram(const StageCodeMap &codeMap)
    : Program(codeMap) {
  bind();
  glUniform1i(shader::forward::fragment::uniform::PBR_IRRADIANCE_MAP_LOC,
              Program::TEX_UNIT_DIFF_IBL);
  unBind();
}
} // namespace render_system::shader
