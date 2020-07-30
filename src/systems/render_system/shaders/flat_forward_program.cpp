#include "flat_forward_program.h"

namespace render_system::shader {
FlatForwardProgram::FlatForwardProgram(const StageCodeMap &codeMap)
    : Program(codeMap) {}

void FlatForwardProgram::loadIrradianceMap(const Texture &tex) {
  glActiveTexture(GL_TEXTURE0 +
                  forward::fragment::uniform::PBR_IRRADIANCE_MAP_UNIT);
  tex.bind();
}

void FlatForwardProgram::loadPrefilteredMap(const Texture &tex) {
  glActiveTexture(GL_TEXTURE0 +
                  forward::fragment::uniform::PBR_PREFILETERED_MAP_UNIT);
  tex.bind();
}
void FlatForwardProgram::loadBrdfIntegrationMap(const Texture &tex) {
  glActiveTexture(GL_TEXTURE0 +
                  forward::fragment::uniform::PBR_BRDF_INTEGRATION_MAP_UNIT);
  tex.bind();
}
} // namespace render_system::shader
