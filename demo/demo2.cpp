#include <fstream>
#include <ranges>
#include <array>
#include <cstddef>
#include <compare>
#include <iterator>
#include <vector>
#include <fmt/core.h>
#include <fmt/ostream.h>
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

#include "accat/auxilia/details/format.hpp"
#include "luce/debugging/Lexer.hpp"

int main() {
  accat::luce::Lexer lexer;
  auto expression = "0x80100000+   ($a0 +5)*4 - *(  $t1 + 8) + number";
  auto res = lexer.load(expression);
  auto coro = lexer.lex();
  for (auto& token : coro) {
    fmt::print("{} ", token.to_string(accat::auxilia::FormatPolicy::kTokenOnly));
  } 
}
