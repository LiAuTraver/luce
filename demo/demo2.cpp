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
#include <absl/strings/string_view.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <accat/auxilia/auxilia.hpp>
#include "luce/Support/isa/architecture.hpp"
#include "luce/Support/isa/riscv32/Register.hpp"
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


int main() {
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
