#pragma once
#include <cstdint>
#include <limits>

#include "accat/auxilia/config.hpp"

namespace accat::luce::isa {
// TODO: implement this
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
  unknown = std::numeric_limits<std::uint8_t>::max()
};
AC_BITMASK_OPS(instruction_set);

template <instruction_set> class Architecture {
  static_assert(false, "Unsupported instruction set");
};

// Host
template <> class Architecture<instruction_set::host> {
public:
  using physical_address_t = std::uintptr_t;
  // using minimal_addressable_unit_t = std::uint8_t;
  // using maximal_instruction_size_t = std::uintptr_t;
  // static inline constexpr physical_address_t base_address =
  //     0x00000000;
};

// RISC-V 32-bit
template <> class Architecture<instruction_set::riscv32> {
public:
  using physical_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using maximal_instruction_size_t = std::uint32_t;
  static inline constexpr physical_address_t base_address =
      0x80000000;
};

// RISC-V 64-bit
template <> class Architecture<instruction_set::riscv64> {
public:
  using physical_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using maximal_instruction_size_t = std::uint64_t;
  static inline constexpr physical_address_t base_address =
      0x80000000;
};

// ARM 32-bit
template <> class Architecture<instruction_set::arm32> {
public:
  using physical_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using maximal_instruction_size_t = std::uint32_t;
  static inline constexpr physical_address_t base_address =
      0x00000000;
};

// ARM 64-bit
template <> class Architecture<instruction_set::arm64> {
public:
  using physical_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using maximal_instruction_size_t = std::uint64_t;
  static inline constexpr physical_address_t base_address =
      0x00000000;
};

// x86 32-bit
template <> class Architecture<instruction_set::x86> {
public:
  using physical_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using maximal_instruction_size_t = std::uint32_t;
  static inline constexpr physical_address_t base_address =
      0x00000000;
};

// x86-64
template <> class Architecture<instruction_set::x64> {
public:
  using physical_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using maximal_instruction_size_t = std::uint64_t;
  static inline constexpr physical_address_t base_address =
      0x00000000;
};
} // namespace accat::luce::isa
