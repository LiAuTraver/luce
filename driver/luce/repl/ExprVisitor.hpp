#pragma once

#include <iostream>
#include <string>

#include "repl_fwd.hpp"
#include "luce/repl/IVisitor.hpp"
namespace accat::luce::repl::expression {
struct ASTPrinter : Visitor {
  explicit ASTPrinter() = default;
  explicit ASTPrinter(const std::ostream &ofs) {
    outs.rdbuf(ofs.rdbuf());
  }
  virtual evaluation::result_type evaluate(const Expr &) override;
  virtual evaluation::result_type visit(const Undefined &) override;
  virtual evaluation::result_type visit(const Grouping &) override;
  virtual evaluation::result_type visit(const Literal &) override;
  virtual evaluation::result_type visit(const Unary &) override;
  virtual evaluation::result_type visit(const Binary &) override;
  virtual evaluation::result_type visit(const Logical &) override;

private:
  std::ostream outs{std::cout.rdbuf()};
};

struct Evaluator : Visitor {
  virtual evaluation::result_type evaluate(const Expr &) override;
  virtual evaluation::result_type visit(const Undefined &) override;
  virtual evaluation::result_type visit(const Grouping &) override;
  virtual evaluation::result_type visit(const Literal &) override;
  virtual evaluation::result_type visit(const Unary &) override;
  virtual evaluation::result_type visit(const Binary &) override;
  virtual evaluation::result_type visit(const Logical &) override;
};
} // namespace accat::luce::repl::expression
