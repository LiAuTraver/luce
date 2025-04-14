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
  return (*(eval.evaluate(**expr))).to_string();
}

TEST(exprEvaluation, arthmetic) {
  auto str = "(2 + (3 * (4 + 5))) / 2";
  EXPECT_EQ(evaluate(str), "14.5");
}

TEST(exprEvaluation, ȧℝŧħṃëṭïçßüṭiℕℂöṙṙĕℂţļꝡFöṙMäṮţëḋḓ) {
  auto str = "\f(2+ \v    (3 "
             " * (4 + 5)\t)) /\n "
             " 2                  "; // same expr as above
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

TEST(exprEvaluation, decimal) {
  auto str = "1.0";
  EXPECT_EQ(evaluate(str), "1");
}

TEST(exprEvaluation, equality) {
  auto str = "1 == 1";
  EXPECT_EQ(evaluate(str), "true");
}

TEST(exprEvaluation, grouping) {
  auto str = "(1 + 2) * 3";
  EXPECT_EQ(evaluate(str), "9");
}

TEST(exprEvaluation, logical) {
  auto str = "true and false";
  EXPECT_EQ(evaluate(str), "false");
}

TEST(exprEvaluation, unary) {
  auto str = "-1";
  EXPECT_EQ(evaluate(str), "-1");
}

TEST(exprEvaluation, binary) {
  auto str = "0b1001";
  EXPECT_EQ(evaluate(str), "9");
}

TEST(exprEvaluation, hex) {
  auto str = "0x10";
  EXPECT_EQ(evaluate(str), "16");
}

TEST(exprEvaluation, octal) {
  auto str = "0o10";
  EXPECT_EQ(evaluate(str), "8");
}

TEST(exprEvaluation, zero) {
  auto str = "0x123/0";
  EXPECT_EQ(evaluate(str), "inf");
}
