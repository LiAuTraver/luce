#include <array>
#include <bit>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <mdspan>
#include <span>
#include <iostream>
#include <variant>
#include "accat/auxilia/details/Status.hpp"
#include "accat/auxilia/details/format.hpp"
#include "luce/Support/isa/architecture.hpp"
[[maybe_unused]] static constexpr uint32_t img[] = {
    0x00000297, // auipc t0,0
    0x00028823, // sb  zero,16(t0)
    0x0102c503, // lbu a0,16(t0)
    0x00100073, // ebreak (used as nemu_trap)
    0xdeadbeef, // some data
};
// std::array<std::byte,25> code = {
//   0x97, 0x02, 0x00, 0x00, // Instruction 1
//   0x23, 0x88, 0x02, 0x00, // Instruction 2
//   0x03, 0xC5, 0x02, 0x01, // Instruction 3
//   0x73, 0x00, 0x10, 0x00, // Instruction 4
//   0xef, 0xbe, 0xad, 0xde  // Invalid instruction
// };
// static const char code2[] = {
// #embed "Z:/luce/data/image.bin"
// };
template <size_t N = 32>
  requires(N % 8 == 0)
auto makeBitSet(const std::span<const std::byte, N / 8> bytes) {
  std::bitset<N> bits;
  // ...
  return bits;
}
// a test
#include <cstddef>
#include <cstdint>
#include <array>
#include <bitset>
#include <type_traits>
#include "luce/Support/isa/Word.hpp"
int main() {
  
}