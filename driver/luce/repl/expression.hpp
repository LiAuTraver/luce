#pragma once

#include <memory>
#include "Token.hpp"
#include "accat/auxilia/details/Monostate.hpp"
#include "accat/auxilia/details/Variant.hpp"
#include "accat/auxilia/details/format.hpp"
#include "luce/repl/evaluation.hpp"
namespace accat::luce::repl::expression {
using eval_result_t = auxilia::StatusOr<evaluation::variant_type>;
interface Visitor;

interface Expr {
  virtual auto to_string(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kTokenOnly) const
      -> auxilia::Printable<Expr>::string_type = 0;
  virtual auto accept(expression::Visitor &) const -> eval_result_t = 0;
};
/// @remark save some typings(`__interface` cannot have alias)
#define EVAL_NAME_USINGS()                                                     \
  using expr_ptr_t = std::shared_ptr<Expr>;                                    \
  using token_t = Token

struct Unknown : /* extends */ auxilia::Monostate,
                 /* implements */ Expr,
                 /* implements */ auxilia::Printable<Unknown> {
  EVAL_NAME_USINGS();
  token_t token;
  Unknown() = default;
  Unknown(Token token) : token(std::move(token)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy) const
      -> auxilia::Monostate::string_type override final {
    return token.to_string(policy);
  }
  virtual auto accept(expression::Visitor &) const
      -> eval_result_t override final;
};
struct Literal : Expr, auxilia::Printable<Literal> {
  EVAL_NAME_USINGS();
  token_t value;
  Literal() = default;
  Literal(token_t value) : value(std::move(value)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy) const
      -> string_type override final {
    return value.to_string(policy);
  }
  virtual auto accept(expression::Visitor &) const
      -> eval_result_t override final;
};
struct Unary : Expr, auxilia::Printable<Unary> {
  EVAL_NAME_USINGS();
  token_t op;
  expr_ptr_t right;
  Unary() = default;
  Unary(token_t op, expr_ptr_t right)
      : op(std::move(op)), right(std::move(right)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy) const
      -> string_type override final {
    return fmt::format("{}{}", op.to_string(policy), right->to_string(policy));
  }
  virtual auto accept(expression::Visitor &) const
      -> eval_result_t override final;
};
struct Binary : Expr, auxilia::Printable<Binary> {
  EVAL_NAME_USINGS();
  token_t op;
  expr_ptr_t left;
  expr_ptr_t right;
  Binary() = default;
  Binary(token_t op, expr_ptr_t left, expr_ptr_t right)
      : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy) const
      -> string_type override final {
    return fmt::format("{} {} {}",
                       op.to_string(policy),
                       left->to_string(policy),
                       right->to_string(policy));
  }
  virtual auto accept(expression::Visitor &) const
      -> eval_result_t override final;
};
struct Grouping : Expr, auxilia::Printable<Grouping> {
  EVAL_NAME_USINGS();
  expr_ptr_t expression;
  Grouping() = default;
  Grouping(expr_ptr_t expression) : expression(std::move(expression)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy) const
      -> string_type override final {
    return expression->to_string(policy);
  }
  virtual auto accept(expression::Visitor &) const
      -> eval_result_t override final;
};
struct Logical : Expr, auxilia::Printable<Logical> {
  EVAL_NAME_USINGS();
  token_t op;
  expr_ptr_t left;
  expr_ptr_t right;
  Logical() = default;
  Logical(token_t op, expr_ptr_t left, expr_ptr_t right)
      : op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
  virtual auto to_string(const auxilia::FormatPolicy &policy) const
      -> string_type override final {
    return fmt::format("{} {} {}",
                       op.to_string(policy),
                       left->to_string(policy),
                       right->to_string(policy));
  }
  virtual auto accept(expression::Visitor &) const
      -> eval_result_t override final;
};
using expr =
    auxilia::Variant<Unknown, Literal, Unary, Binary, Grouping, Logical>;
#undef EVAL_NAME_USINGS
} // namespace accat::luce::repl::expression
