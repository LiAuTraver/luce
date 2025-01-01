#pragma once
#include <cstdint>
#include <limits>

#include <accat/auxilia/auxilia.hpp>

namespace accat::luce::isa {

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

template <instruction_set> struct Architecture {
  static_assert(false, "Unsupported instruction set");
};

// Host
template <> struct Architecture<instruction_set::host> {
  using virtual_address_t = std::uintptr_t;
  using physical_address_t = std::uintptr_t;
};

// RISC-V 32-bit
template <> struct Architecture<instruction_set::riscv32> {
  using virtual_address_t = std::uint32_t;
  using physical_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint32_t;
  static constexpr physical_address_t physical_base_address = 0x80000000;
  static constexpr physical_address_t physical_memory_size = 0x8000000;
  static constexpr physical_address_t physical_memory_begin =
      physical_base_address; // same as base
  static constexpr physical_address_t physical_memory_end =
      physical_memory_begin + physical_memory_size - 1;
  static constexpr std::size_t general_purpose_register_count = 32;
  static constexpr std::size_t instruction_alignment = 4;
  static constexpr bool has_floating_point = true;
};

// RISC-V 64-bit
template <> struct Architecture<instruction_set::riscv64> {
  using virtual_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t =
      std::uint32_t; // Fixed: RISC-V uses 32-bit instructions
  static constexpr virtual_address_t physical_base_address = 0x80000000;
  static constexpr std::size_t general_purpose_register_count = 32;
  static constexpr std::size_t instruction_alignment = 4;
  static constexpr bool has_floating_point = true;
};

// ARM 32-bit
template <> struct Architecture<instruction_set::arm32> {
  using virtual_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint32_t;
  static constexpr virtual_address_t physical_base_address =
      0x00008000; // Common ARM Linux base
  static constexpr std::size_t general_purpose_register_count = 16;
  static constexpr std::size_t instruction_alignment = 4;
  static constexpr bool has_floating_point = true;
};

// ARM 64-bit
template <> struct Architecture<instruction_set::arm64> {
  using virtual_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t =
      std::uint32_t; // Fixed: ARM64 uses 32-bit instructions
  static constexpr virtual_address_t physical_base_address =
      0xFFFF000000000000; // Typical AArch64 base
  static constexpr std::size_t general_purpose_register_count = 31; // X0-X30
  static constexpr std::size_t instruction_alignment = 4;
  static constexpr bool has_floating_point = true;
};

// x86 32-bit
template <> struct Architecture<instruction_set::x86> {
  using virtual_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint8_t; // Variable length instructions
  static constexpr virtual_address_t physical_base_address =
      0x08048000; // Traditional ELF base
  static constexpr std::size_t general_purpose_register_count = 8;
  static constexpr std::size_t instruction_alignment = 1;
  static constexpr bool has_floating_point = true;
};

// x86-64
template <> struct Architecture<instruction_set::x64> {
  using virtual_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint8_t; // Variable length instructions
  static constexpr virtual_address_t physical_base_address =
      0x400000; // Common x64 base
  static constexpr std::size_t general_purpose_register_count = 16;
  static constexpr std::size_t instruction_alignment = 1;
  static constexpr bool has_floating_point = true;
};
} // namespace accat::luce::isa
