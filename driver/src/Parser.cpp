#include "deps.hh"

#include <spdlog/spdlog.h>
#include <memory>

#include "luce/repl/Parser.hpp"
#include "luce/repl/Token.hpp"
#include "luce/repl/expression.hpp"
#include "luce/repl/evaluation.hpp"
namespace accat::luce::repl {
using enum Parser::token_t::Type;
auto Parser::next_expression() -> expr_ptr_t {
  return assignment();
}
auto Parser::assignment() -> expr_ptr_t {
  auto expr = logical_or();
  // currently skip assignment
  return expr;
}
auto Parser::logical_or() -> expr_ptr_t {
  auto expr = logical_and();
  while (inspect(kOr)) {
    auto or_op = consume();
    auto rhs = logical_or();
    expr = std::make_shared<expression::Logical>(
        std::move(or_op), std::move(expr), std::move(rhs));
  }
  return expr;
}
auto Parser::logical_and() -> expr_ptr_t {
  auto expr = equality();
  while (inspect(kAnd)) {
    auto and_op = consume();
    auto rhs = equality();
    expr = std::make_shared<expression::Logical>(
        std::move(and_op), std::move(expr), std::move(rhs));
  }
  return expr;
}
auto Parser::equality() -> expr_ptr_t {
  auto expr = comparison();
  while (inspect(kBangEqual, kEqualEqual)) {
    auto op = consume();
    auto rhs = comparison();
    expr = std::make_shared<expression::Binary>(
        std::move(op), std::move(expr), std::move(rhs));
  }
  return expr;
}
auto Parser::comparison() -> expr_ptr_t {
  auto expr = term();
  while (inspect(kGreater, kGreaterEqual, kLess, kLessEqual)) {
    auto op = consume();
    auto rhs = term();
    expr = std::make_shared<expression::Binary>(
        std::move(op), std::move(expr), std::move(rhs));
  }
  return expr;
}
auto Parser::term() -> expr_ptr_t {
  auto expr = factor();
  while (inspect(kMinus, kPlus)) {
    auto op = consume();
    auto rhs = factor();
    expr = std::make_shared<expression::Binary>(
        std::move(op), std::move(expr), std::move(rhs));
  }
  return expr;
}
auto Parser::factor() -> expr_ptr_t {
  auto expr = unary();
  while (inspect(kSlash, kStar)) { // kStar meaning 1: multiplication
    auto op = consume();
    auto rhs = unary();
    expr = std::make_shared<expression::Binary>(
        std::move(op), std::move(expr), std::move(rhs));
  }
  return expr;
}
auto Parser::unary() -> expr_ptr_t {
  if (inspect(kBang, // right-to-left associativity
              kMinus,
              kAmpersand,
              kStar)) { // kStar meaning 2: dereference
    auto op = consume();
    auto rhs = unary();
    return std::make_shared<expression::Unary>(std::move(op), std::move(rhs));
  }
  return call();
}
auto Parser::call() -> expr_ptr_t {
  auto expr = primary();
  // currently skip function call
  // while(inspect(kLeftParen)) {
  // TODO(implement function call)
  // }
  return expr;
}
auto Parser::primary() -> expr_ptr_t {
  if (inspect(kFalse, kTrue, kNil, kNumber, kString, kIdentifier)) {
    return std::make_shared<expression::Literal>(consume());
  }
  // groupings
  if (inspect(kLeftParen)) {
    consume();
    auto expr = next_expression();
    if (!inspect(kRightParen)) {
      dbg(trace, "expected ')' after expression")
      return std::make_shared<expression::Undefined>(consume());
    }
    consume();
    return std::make_shared<expression::Grouping>(std::move(expr));
  }
  return std::make_shared<expression::Undefined>(consume());
}
bool Parser::is_at_end() {
  return peek() <=> nulltok == 0;
}
Parser::token_t Parser::consume() {
  precondition(not queued_tokens.empty(), "unexpected end of file")
  auto token = std::move(queued_tokens.front());
  // queued_tokens.pop();
  queued_tokens.pop_front();
  return token; // must enable RVO to compile
}
const Parser::token_t &Parser::peek(const size_t offset) {
  while (queued_tokens.size() <= offset) {
    if (auto token = coro.next()) {
      // queued_tokens.emplace(std::move(*token));
      queued_tokens.emplace_back(std::move(*token));
    } else
      // optional<T&> is not available in C++23 currently, we uses monostate
      // to indicate to present the end of tokens
      return nulltok;
  }
  return queued_tokens[offset];
}
} // namespace accat::luce::repl
