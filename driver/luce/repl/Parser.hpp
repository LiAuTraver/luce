#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include "Token.hpp"
#include "accat/auxilia/details/Status.hpp"
#include "expression.hpp"
#include "luce/repl/Lexer.hpp"
namespace accat::luce::repl {
class Parser {
public:
  using coro_t = Lexer::generator_t;
  using token_t = Lexer::token_t;
  using expr_ptr_t = std::shared_ptr<expression::Expr>;

public:
  explicit Parser(coro_t &&coro)
      : coro(std::move(coro)) {}
  Parser(const Parser &) = delete;
  Parser(Parser &&) noexcept = default;
  Parser &operator=(const Parser &) = delete;
  Parser &operator=(Parser &&) noexcept = default;

public:
  auto next_expression() -> auxilia::StatusOr<expr_ptr_t>;

private:
  auto next_expression_impl() -> expr_ptr_t;
  auto assignment() -> expr_ptr_t;
  auto logical_or() -> expr_ptr_t;
  auto logical_and() -> expr_ptr_t;
  auto equality() -> expr_ptr_t;
  auto comparison() -> expr_ptr_t;
  auto term() -> expr_ptr_t;
  auto factor() -> expr_ptr_t;
  auto unary() -> expr_ptr_t;
  auto call() -> expr_ptr_t;
  auto primary() -> expr_ptr_t;

private:
  bool is_at_end();
  token_t consume();
  const token_t &peek(size_t = 0);
  template <typename... Args>
    requires(std::is_enum_v<std::common_type_t<Args...>>)
  bool inspect(Args &&...);
  std::string error_message;

private:
  /// @brief the coroutine that generates tokens
  coro_t coro;
  /// @brief the queue of tokens that are yet to be processed,
  /// this is used to implement the lookahead feature
  std::deque<token_t> queued_tokens;
};
template <typename... Args>
  requires(std::is_enum_v<std::common_type_t<Args...>>)
bool Parser::inspect(Args &&...args) {
  if (is_at_end())
    return false;
  return (... || (peek().type() == args));
}
} // namespace accat::luce::repl
