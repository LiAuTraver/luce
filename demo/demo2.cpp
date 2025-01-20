#include <algorithm>
#include <fstream>
#include <iostream>
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
#include "luce/repl/ExprVisitor.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"

int main() {
  accat::luce::repl::Lexer lexer;
  auto expression = "1 + 2 * 3 - 4 / 5";
  auto res = lexer.load(expression);
  auto coro = lexer.lex();
  accat::luce::repl::Parser parser{std::move(coro)};
  auto ast = parser.next_expression();
  std::cout << typeid(ast).name() << std::endl;
  accat::luce::repl::expression::ASTPrinter printer;
  printer.visit(ast);
  std::cout << printer.buffer.str();
}
