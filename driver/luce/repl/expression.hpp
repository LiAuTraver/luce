#pragma once

#include <memory>
#include <accat/auxilia/auxilia.hpp>
#include <string>

#include "Token.hpp"
#include "luce/repl/repl_fwd.hpp"
namespace accat::luce::repl::evaluation {
using variant_type =
    auxilia::Variant<Undefined, Number, Byte, String, Boolean, Nil>;
using result_type = auxilia::StatusOr<variant_type>;
} // namespace accat::luce::repl::evaluation

namespace accat::luce::repl::expression {
struct Visitor;

struct Expr {
  virtual auto
  to_string(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kBrief) const
      -> std::string = 0;
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type = 0;

protected:
  Expr() = default;
  virtual ~Expr() = default;
  using expr_ptr_t = std::shared_ptr<Expr>;
  using token_t = Token;
  using enum auxilia::FormatPolicy;
};

struct Undefined : /* extends */ auxilia::Monostate,
                   /* implements */ Expr {
  token_t token;
  Undefined() = default;
  explicit Undefined(Token token) : token(std::move(token)) {}
  // ReSharper disable once CppHidingFunction
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> auxilia::Monostate::string_type override final {
    return token.to_string(policy);
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
  friend auto operator<<(std::ostream &os, const Undefined &u)
      -> std::ostream & {
    return os << u.to_string(kBrief);
  }
};
struct Literal : Expr, auxilia::Printable {
  token_t value;
  Literal() = default;
  explicit Literal(token_t value) : value(std::move(value)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> string_type override final {
    return value.to_string(policy);
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
};
struct Variable : Expr, auxilia::Printable {
  Variable() = default;
  explicit Variable(token_t name) : name(std::move(name)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> string_type override final {
    return name.to_string(policy);
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
  auto identifier() const noexcept {
    return name.lexeme();
  }

private:
  token_t name;
};
/// @note !, -, *(dereference), &(address-of)
struct Unary : Expr, auxilia::Printable {
  token_t op;
  expr_ptr_t right;
  Unary() = default;
  Unary(token_t op, expr_ptr_t right)
      : op(std::move(op)), right(std::move(right)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> string_type override final {
    return fmt::format("{}{}", op.to_string(policy), right->to_string(policy));
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
};
struct Binary : Expr, auxilia::Printable {
  token_t op;
  expr_ptr_t left;
  expr_ptr_t right;
  Binary() = default;
  Binary(token_t op, expr_ptr_t left, expr_ptr_t right)
      : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> string_type override final {
    return fmt::format("{} {} {}",
                       op.to_string(policy),
                       left->to_string(policy),
                       right->to_string(policy));
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
};
struct Grouping : Expr, auxilia::Printable {
  expr_ptr_t expression;
  Grouping() = default;
  explicit Grouping(expr_ptr_t expression)
      : expression(std::move(expression)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> string_type override final {
    return "(" + expression->to_string(policy) + ")";
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
};
struct Logical : Expr, auxilia::Printable {
  token_t op;
  expr_ptr_t left;
  expr_ptr_t right;
  Logical() = default;
  Logical(token_t op, expr_ptr_t left, expr_ptr_t right)
      : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy = kBrief) const
      -> string_type override final {
    return fmt::format("{} {} {}",
                       op.to_string(policy),
                       left->to_string(policy),
                       right->to_string(policy));
  }
  virtual auto accept(expression::Visitor &) const
      -> evaluation::result_type override final;
};
using expr =
    auxilia::Variant<Undefined, Literal, Unary, Binary, Grouping, Logical>;
} // namespace accat::luce::repl::expression
