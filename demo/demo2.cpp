#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <array>
#include <cstddef>
#include <compare>
#include <iterator>
#include <span>
#include <string>
#include <variant>
#include <vector>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <spdlog/common.h>

#include <accat/auxilia/auxilia.hpp>

#include "luce/Support/isa/Word.hpp"
#include "luce/Support/isa/architecture.hpp"
#include "luce/Support/isa/constants/riscv32.hpp"
#include "luce/Support/isa/riscv32/Register.hpp"
#include "luce/Support/isa/riscv32/instruction/Base.hpp"
template <size_t N> consteval auto ArrOf() {
  std::array<std::byte, N> arr;
  for (size_t i = 0; i < N; i++) {
    arr[i] = std::byte(i);
  }
  return arr;
}
using namespace accat::luce::isa;
using GPR = GeneralPurposeRegisters;
using namespace accat::auxilia;
using FP = FormatPolicy;
using enum FP;

void t1() {
  GPR registers;
  using B = std::byte;
  constexpr auto arr = ArrOf<128>();
  size_t i = 0;
  for (auto &reg : registers.view()) {
    reg = {arr[i], arr[i + 1], arr[i + 2], arr[i + 3]};
    i += 4;
  }
  std::cout << "\n\n\n" << registers.to_string(kDefault) << std::endl;
  std::cout << "\n\n\n" << registers.to_string(kDetailed) << std::endl;
  std::cout << "\n\n\n" << registers.to_string(kBrief) << std::endl;

  Word w;
  w.num() = -64;
  std::cout << "Bits: " << w.to_string(kDefault) << std::endl;
  std::cout << "Detailed: " << w.to_string(kDetailed) << std::endl;
  std::cout << "Brief: " << w.to_string(kBrief) << std::endl;
}

void t2() {
  //  0b 1111 1110 0111 0010 1100 1010 1110 0011
  auto w = instruction::base::Blt(0xfe72cae3);
  std::cout << w.to_string(kDefault) << std::endl;
  // 8, 12, 25, 31, 7, 8, 31, 32
  // 1010 0111 111 1 1
  std::cout << (w.imm()) << std::endl;

  auto neg = -0xabc;
  fmt::println("neg: {:#03x}", neg);
}
int main() {
  spdlog::set_level(spdlog::level::trace);
  t2();
  return 0;
}
