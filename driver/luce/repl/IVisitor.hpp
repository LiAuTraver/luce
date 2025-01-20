#pragma once

#include "expression.hpp"
namespace accat::luce::repl {
interface IVisitor{};
namespace expression {
interface Visitor {
  virtual eval_result_t visit(const Unknown &) = 0;
  virtual eval_result_t visit(const Grouping &) = 0;
  virtual eval_result_t visit(const Literal &) = 0;
  virtual eval_result_t visit(const Unary &) = 0;
  virtual eval_result_t visit(const Binary &) = 0;
  virtual eval_result_t visit(const Logical &) = 0;
};
}
// namespace statement {
// interface Visitor{};
// }
}