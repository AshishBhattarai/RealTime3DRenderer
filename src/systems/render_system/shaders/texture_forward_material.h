#pragma once

#include "flat_forward_material.h"

namespace render_system {
struct TextureMaterial;
namespace shader {
/**
 * @brief
 * TextureForwardMaterial is a shader for forward textured material rendering.
 */
class TextureForwardMaterial : public FlatForwardMaterial {
public:
  TextureForwardMaterial(const StageCodeMap &codeMap);
  void loadMaterial(const TextureMaterial &material);
};
} // namespace shader
} // namespace render_syster
