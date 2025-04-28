#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

#include <accat/auxilia/defines.hpp>
#include "./config.hpp"
#include "./host.hpp"
#include "./constants/riscv32.hpp"
namespace accat::luce::isa {
using namespace riscv32;
enum class instruction_set : std::uint8_t {
  host = 0,
  is_32bit = 1,
  is_64bit = 2,
  is_arm = 4,
  is_x86 = 8,
  is_riscv = 16,
  riscv32 = is_riscv | is_32bit,
  riscv64 = is_riscv | is_64bit,
  arm32 = is_arm | is_32bit,
  arm64 = is_arm | is_64bit,
  x86 = is_x86 | is_32bit,
  x64 = is_x86 | is_64bit,
};
AC_BITMASK_OPS(instruction_set)
static_assert(sizeof(std::byte) == sizeof(isa::minimal_addressable_unit_t),
              "current implementation requires std::byte to be same size as "
              "minimal_addressable_unit_t");
#if _WIN32
static_assert(sizeof(std::bitset<32>) == 4 * sizeof(std::byte),
              "std::bitset<8> must be same size as std::byte");
#endif
} // namespace accat::luce::isa
