#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace accat::luce::isa {
/// @note currently not supported/tested, so left as is.

// RISC-V 64-bit
namespace riscv64 {
  using virtual_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint32_t;
  inline static constexpr virtual_address_t physical_base_address = 0x80000000;
  inline static constexpr std::size_t general_purpose_register_count = 32;
  inline static constexpr std::size_t instruction_alignment = 4;
  inline static constexpr bool has_floating_point = true;
  } // namespace riscv64
  
  // ARM 32-bit
  namespace arm32 {
  using virtual_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint32_t;
  inline static constexpr virtual_address_t physical_base_address =
      0x00008000; // Common ARM Linux base
  inline static constexpr std::size_t general_purpose_register_count = 16;
  inline static constexpr std::size_t instruction_alignment = 4;
  inline static constexpr bool has_floating_point = true;
  } // namespace arm32
  
  // ARM 64-bit
  namespace arm64 {
  using virtual_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint32_t;
  inline static constexpr virtual_address_t physical_base_address =
      0xFFFF000000000000; // Typical AArch64 base
  inline static constexpr std::size_t general_purpose_register_count =
      31; // X0-X30
  inline static constexpr std::size_t instruction_alignment = 4;
  inline static constexpr bool has_floating_point = true;
  } // namespace arm64
  
  // x86 32-bit
  namespace x86 {
  using virtual_address_t = std::uint32_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint8_t; // Variable length instructions
  inline static constexpr virtual_address_t physical_base_address =
      0x08048000; // Traditional ELF base
  inline static constexpr std::size_t general_purpose_register_count = 8;
  inline static constexpr std::size_t instruction_alignment = 1;
  inline static constexpr bool has_floating_point = true;
  } // namespace x86
  
  // x86-64
  namespace x64 {
  using virtual_address_t = std::uint64_t;
  using minimal_addressable_unit_t = std::uint8_t;
  using instruction_size_t = std::uint8_t; // Variable length instructions
  inline static constexpr virtual_address_t physical_base_address =
      0x400000; // Common x64 base
  inline static constexpr std::size_t general_purpose_register_count = 16;
  inline static constexpr std::size_t instruction_alignment = 1;
  inline static constexpr bool has_floating_point = true;
  } // namespace x64
  
}