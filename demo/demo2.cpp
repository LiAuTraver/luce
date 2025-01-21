#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <array>
#include <cstddef>
#include <compare>
#include <iterator>
#include <variant>
#include <vector>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>

#include <accat/auxilia/auxilia.hpp>
#include "accat/auxilia/details/Monostate.hpp"
#include "accat/auxilia/details/Variant.hpp"
#include "accat/auxilia/details/format.hpp"
#include "luce/repl/ExprVisitor.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"
#include "luce/repl/evaluation.hpp"

int main() {
  accat::luce::repl::Lexer lexer;
  auto expression = "1 + 2 * 3 - 4 / 5";
  auto coro = lexer.load_string(expression).lex();
  accat::luce::repl::Parser parser{std::move(coro)};
  auto ast = parser.next_expression();
  std::cout << typeid(ast).name() << std::endl;
  accat::luce::repl::expression::ASTPrinter printer{std::cout};
  printer.evaluate(*ast);
  accat::luce::repl::expression::Evaluator evaluator;
  auto res = evaluator.evaluate(*ast);
  if (!res.ok()) {
    fmt::println("Error: {}", res.message());
  }
  std::cout << "Result: "
            << res.value().underlying_string(
                   accat::auxilia::FormatPolicy::kBrief)
            << std::endl;
}
