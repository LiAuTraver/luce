#pragma once
#include "luce/repl/expression.hpp"
#include "luce/repl/IVisitor.hpp"
namespace accat::luce::repl::expression {

auto Literal::accept(expression::Visitor &visitor) const -> eval_result_t {
  return visitor.visit(*this);
}
auto Unary::accept(expression::Visitor &visitor) const -> eval_result_t {
  return visitor.visit(*this);
}
auto Binary::accept(expression::Visitor &visitor) const -> eval_result_t {
  return visitor.visit(*this);
}
auto Grouping::accept(expression::Visitor &visitor) const -> eval_result_t {
  return visitor.visit(*this);
}
auto Logical::accept(expression::Visitor &visitor) const -> eval_result_t {
  return visitor.visit(*this);
}
auto Unknown::accept(expression::Visitor &visitor) const -> eval_result_t {
  return visitor.visit(*this);
}
} // namespace accat::luce::repl::expression
