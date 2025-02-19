#pragma once

/// @namespace accat::luce::isa::host Host architecture(build platform) specific types
#include <cstdint>
namespace accat::luce::isa::host {
using virtual_address_t = std::uintptr_t;
using physical_address_t = std::uintptr_t;
} // namespace accat::luce::isa::host
