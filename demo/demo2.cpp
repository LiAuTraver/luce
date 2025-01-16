#include <ranges>
#include <array>
#include <cstddef>
#include <compare>
#include <iterator>
#include <vector>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <ranges>
#include <array>
#include <cstddef>
#include <compare>
#include <iterator>
#include <vector>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <accat/auxilia/auxilia.hpp>

int main() {
  auto bytes = std::vector<std::byte>{
      std::byte{0xDE}, std::byte{0xAD}, std::byte{0xBE}, std::byte{0xEF}};

  auto reversed = bytes | accat::auxilia::ranges::invert_endianness |
                  std::ranges::to<std::vector<std::byte>>();
  assert(bytes == reversed);

  fmt::println("Original: {:#04x}", fmt::join(bytes, " "));
  fmt::println("Reversed: {:#04x}", fmt::join(reversed, " "));
  return 0;
}
