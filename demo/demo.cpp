#include <fmt/xchar.h>
#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <mdspan>
#include <memory>
#include <optional>
#include <span>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
// [[maybe_unused]] static constexpr uint32_t img[] = {
//     0x00000297, // auipc t0,0
//     0x00028823, // sb  zero,16(t0)
//     0x0102c503, // lbu a0,16(t0)
//     0x00100073, // ebreak (used as nemu_trap)
//     0xdeadbeef, // some data
// };
// test framework

struct Instruction {
  virtual auto execute() const -> void = 0;
  virtual ~Instruction() = default;
};

struct IDecoder {
  virtual auto decode(uint32_t) -> std::unique_ptr<Instruction> = 0;
  virtual ~IDecoder() = default;
};

struct RV32IDecoder : IDecoder {
  auto decode(uint32_t) -> std::unique_ptr<Instruction> override {
    // some implementation, returns concrete instruction
    return nullptr;
  }
};
struct Disassembler {
  std::vector<std::unique_ptr<IDecoder>> decoders;
  auto disassemble(uint32_t) -> std::unique_ptr<Instruction> {
    for (auto &decoder : decoders) {
      if (auto instr = decoder->decode(0); instr) {
        return instr;
      }
    }
    return nullptr;
  };
  void addDecoder(std::unique_ptr<IDecoder> decoder) {
    decoders.emplace_back(std::move(decoder));
  }
};
int main() {
  
}
