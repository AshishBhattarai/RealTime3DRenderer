#pragma once

#include "types.h"
#include <map>

namespace component {
/**
 * @brief The Model struct represent entity model(mesh + material)
 * to change model an entity should remove and add the component again.
 */
struct Model {
  uint meshId;
  // primitive id to material id
  std::map<PrimitiveId, MaterialId> primIdToMatId;
};
} // namespace component
