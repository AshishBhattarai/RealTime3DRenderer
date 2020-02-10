#pragma once

#include "types.h"
#include <string>

namespace component {
struct Mesh {
  std::string name;
  uint modelId;

  Mesh(std::string name, uint modelId) : name(name), modelId(modelId) {}
  Mesh(std::pair<std::string, uint> nameIdPair)
      : name(nameIdPair.first), modelId(nameIdPair.second) {}
};
} // namespace component
