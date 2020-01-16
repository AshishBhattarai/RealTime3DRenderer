#pragma once

#include "../types.h"
#include <bitset>
#include <cassert>

namespace ecs {
using Entity = u32;
using ComponentFamily = size_t;
using SystemFamily = size_t;
using EventFamily = size_t;

constexpr Entity INVALID_ENTITY = 0;
constexpr ComponentFamily INVALID_COMPONENT_FAMILY = 0;
constexpr SystemFamily INVALID_SYSTEM_FAMILY = 0;
constexpr EventFamily INVALID_EVENT_FAMILY = 0;

const Entity MAX_ENTITES = 5000;
const size_t MAX_COMPONENTS = 32;
// used to track component type in an entity
using Signature = std::bitset<MAX_COMPONENTS>;
} // namespace ecs
