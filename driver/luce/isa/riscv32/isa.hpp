#pragma once

// RISC-V 32-bit
#include <array>
#include <cstddef>
#include <cstdint>
namespace accat::luce::isa::riscv32 {
using virtual_address_t = std::uint32_t;
using physical_address_t = std::uint32_t;
using minimal_addressable_unit_t = std::uint8_t;
using instruction_size_t = std::uint32_t;
inline static constexpr physical_address_t physical_base_address = 0x80000000;
// inline static constexpr physical_address_t physical_memory_size = 0x8000000;
// // 128MB
inline static constexpr physical_address_t physical_memory_size =
    0x800000; // 8MB, for testing
inline static constexpr physical_address_t physical_memory_begin =
    physical_base_address; // same as base
inline static constexpr physical_address_t physical_memory_end =
    physical_memory_begin + physical_memory_size - 1;
inline static constexpr std::size_t general_purpose_register_count = 32;
inline static constexpr std::size_t instruction_alignment = 4;
inline static constexpr std::size_t page_size = 0x1000; // 4KB
inline static constexpr bool has_floating_point = true;
namespace signal {
inline static constexpr auto trap = std::array{
    std::byte{0xde}, std::byte{0xad}, std::byte{0xbe}, std::byte{0xef}};
}

} // namespace accat::luce::isa::riscv32
