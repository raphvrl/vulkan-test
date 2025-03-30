#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <memory>

#include "core/types.hpp"

namespace ecs
{

using EntityID = u32;
using ComponentID = u8;

constexpr EntityID MAX_ENTITIES = 10000;
constexpr ComponentID MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace ecs