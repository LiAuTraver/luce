#include "deps.hh"

#include <gtest/gtest.h>

#include "luce/repl/ExprVisitor.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"
#include "luce/repl/expression.hpp"
#include "luce/repl/evaluation.hpp"

using namespace accat::luce::repl;

TEST(dummy, dummy) {
  EXPECT_EQ(1, 1);
  EXPECT_FALSE(false);
}

std::string evaluate(const std::string &str) {
  Lexer lexer;
  Lexer::generator_t coro = lexer.load_string(str).lex();
  Parser parser(std::move(coro));
  auto expr = parser.next_expression();
  expression::Evaluator eval;
  return eval.evaluate(*expr)->underlying_string();
}


TEST(exprEvaluation, arthmetic) {
  auto str = "(2 + (3 * (4 + 5))) / 2";
  EXPECT_EQ(evaluate(str), "14.5");
}

TEST(exprEvaluation, boolean) {
  auto str = "!true";
  EXPECT_EQ(evaluate(str), "false");
}

TEST(exprEvaluation, comparison) {
  auto str = "1 < 2";
  EXPECT_EQ(evaluate(str), "true");
}