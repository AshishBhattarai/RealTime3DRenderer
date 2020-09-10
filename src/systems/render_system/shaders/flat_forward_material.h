#pragma once

#include "program.h"
#include <glm/mat4x4.hpp>

namespace render_system {
struct Mesh;
struct PointLight;
struct FlatMaterial;
class Texture;
namespace shader {
/**
 * @brief
 * FlatForwardMaterial is a shader for forward colored material rendering.
 */
class FlatForwardMaterial : public Program {
public:
  FlatForwardMaterial(const StageCodeMap &codeMap);

  void loadTransformMatrix(const glm::mat4 &matrix);
  void loadPointLight(const PointLight &pointLight, uint idx);
  void loadPointLightSize(int size);
  void loadMaterial(const FlatMaterial &material);

  void loadIrradianceMap(const Texture &tex);
  void loadPrefilteredMap(const Texture &tex);
  void loadBrdfIntegrationMap(const Texture &tex);
};
} // namespace shader
} // namespace render_system
