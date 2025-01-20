#pragma once

#include "luce/repl/ExprVisitor.hpp"
namespace accat::luce::repl::expression {
eval_result_t ASTPrinter::visit(const Unknown &expr) {
  return {};
}
eval_result_t ASTPrinter::visit(const Grouping &expr) {
  buffer << "(" << expr.expression->to_string() << ")" << '\n';
  return {};
}
eval_result_t ASTPrinter::visit(const Literal &expr) {
  buffer << expr.value << '\n';
  return {};
}
eval_result_t ASTPrinter::visit(const Unary &expr) {
  buffer << expr.op.to_string() << expr.right->to_string() << '\n';
  return {};
}
eval_result_t ASTPrinter::visit(const Binary &expr) {
  buffer << expr.left->to_string() << expr.op.to_string() << expr.right->to_string() << '\n';
  return {};
}
eval_result_t ASTPrinter::visit(const Logical &expr) {
  buffer << expr.left->to_string() << expr.op.to_string() << expr.right->to_string() << '\n';
  return {};
}
} // namespace accat::luce::repl::expression
