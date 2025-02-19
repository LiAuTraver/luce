#include <bitset>
#include <cstddef>
#include <mdspan>
#include <span>
#include <iostream>
[[maybe_unused]] static constexpr uint32_t img[] = {
    0x00000297, // auipc t0,0 000000000000 00000000001010010111
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
int main() {
  auto code = std::as_bytes(std::span{img});
  // std::mdspan<const std::byte,std::dextents<size_t,2>> code_span(
  //   code.data(), // pointer to data
  //   5,           // first dimension
  //   4            // second dimension
  // );
  {
    constexpr uint32_t inst = img[0];
    uint32_t opcode = inst & 0x7f;    // bits 0-6
    uint32_t rd = (inst >> 7) & 0x1f; // bits 7-11
    uint32_t imm = inst >> 12;        // bits 12-31

    std::cout << "Instruction: " << inst << "\n"
              << "Opcode: " << opcode << "\n"
              << "rd: " << rd << "\n"
              << "imm: " << imm << "\n";
  }
  auto first = code.subspan(0, 4);
  
}
