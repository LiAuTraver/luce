#pragma once

#include "luce/repl/IVisitor.hpp"
namespace accat::luce::repl::expression {
struct ASTPrinter : Visitor {
  eval_result_t visit(const Unknown &);
  eval_result_t visit(const Grouping &);
  eval_result_t visit(const Literal &);
  eval_result_t visit(const Unary &);
  eval_result_t visit(const Binary &);
  eval_result_t visit(const Logical &);
  auxilia::ostringstream buffer;
};
}