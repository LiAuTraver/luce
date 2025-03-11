#pragma once
#include <cstddef>
#include <cstdint>
#include <array>
namespace accat::luce::isa::riscv32 {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
using virtual_address_t = std::uint32_t;
using physical_address_t = std::uint32_t;
using minimal_addressable_unit_t = std::uint8_t;
/// @note from unpriv-isa-asciidoc or riscv: Instructions are stored in memory
/// as a sequence of 16-bit little-endian parcels, regardless of memory system
/// endianness.
using instruction_size_t = std::uint32_t;
inline static constexpr std::size_t instruction_size_bytes =
    sizeof(instruction_size_t) / sizeof(std::byte);
inline static constexpr physical_address_t physical_base_address = 0x80000000;
inline static constexpr virtual_address_t virtual_base_address = 0x80000000;
// inline static constexpr physical_address_t physical_memory_size = 0x8000000;
// 128MB
inline static constexpr physical_address_t physical_memory_size =
    0x800000; // 8MB, for testing
inline static constexpr physical_address_t physical_memory_begin =
    physical_base_address; // same as base
inline static constexpr physical_address_t physical_memory_end =
    physical_memory_begin + physical_memory_size - 1;
/// @note  the RV32E subset has 16 registers here we don't care about that
inline static constexpr std::size_t general_purpose_register_count = 32;
inline static constexpr std::size_t instruction_alignment = 4;
inline static constexpr std::size_t page_size = 0x1000; // 4KB
inline static constexpr bool has_floating_point = true;
namespace signal {
inline static constexpr auto deadbeef = std::array{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde}};
}
#pragma clang diagnostic pop
} // namespace accat::luce::isa::riscv32

namespace accat::luce::isa {
using namespace riscv32;
}