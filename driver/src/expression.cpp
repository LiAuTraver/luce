#include "deps.hh"

#include "luce/repl/expression.hpp"
#include "luce/repl/IVisitor.hpp"
#include "luce/repl/evaluation.hpp"
#include "luce/repl/repl_fwd.hpp"
namespace accat::luce::repl::expression {

auto Literal::accept(expression::Visitor &visitor) const -> evaluation::result_type {
  return visitor.visit(*this);
}
auto Unary::accept(expression::Visitor &visitor) const -> evaluation::result_type {
  return visitor.visit(*this);
}
auto Binary::accept(expression::Visitor &visitor) const -> evaluation::result_type {
  return visitor.visit(*this);
}
auto Grouping::accept(expression::Visitor &visitor) const -> evaluation::result_type {
  return visitor.visit(*this);
}
auto Logical::accept(expression::Visitor &visitor) const -> evaluation::result_type {
  return visitor.visit(*this);
}
auto Undefined::accept(expression::Visitor &visitor) const -> evaluation::result_type {
  return visitor.visit(*this);
}
} // namespace accat::luce::repl::expression
