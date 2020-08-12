#include "texture_forward_material.h"
#include "../mesh.h"
#include "config.h"

namespace render_system::shader {

TextureForwardMaterial::TextureForwardMaterial(const StageCodeMap &codeMap)
    : FlatForwardMaterial(codeMap) {}

void TextureForwardMaterial::loadMaterial(const TextureMaterial &material) {
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_ALBEDO_BND);
  material.albedo.bind();
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_METALLIC_ROUGHNESS_BND);
  material.metallicRoughness.bind();
  glActiveTexture(GL_TEXTURE0 + forward::fragment::uniform::textured::PBR_ALBEDO_BND);
  material.ao.bind();
}

} // namespace render_system::shader
