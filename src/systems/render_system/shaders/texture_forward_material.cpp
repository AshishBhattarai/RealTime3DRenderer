#include "texture_forward_material.h"
#include "../mesh.h"
#include "config.h"

namespace render_system::shader {

TextureForwardMaterial::TextureForwardMaterial(const StageCodeMap &codeMap)
    : FlatForwardMaterial(codeMap) {}

void TextureForwardMaterial::loadMaterial(const TextureMaterial &material) {
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_ALBEDO_UNIT);
  material.albedo.bind();
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_METALLIC_ROUGHNESS_UNIT);
  material.metallicRoughness.bind();
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_AO_UNIT);
  material.ao.bind();
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_MATERIAL_NORMAL_BND);
  material.normal.bind();
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_MATERIAL_EMISSION_BND);
  material.emission.bind();
}

} // namespace render_system::shader
