#pragma once

#include "../mesh.h"
#include "../point_light.h"
#include "../texture.h"
#include "config.h"
#include "program.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

namespace render_system::shader {
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
} // namespace render_system::shader
