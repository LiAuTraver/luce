#pragma once

#include <iostream>
#include <string>

#include "repl_fwd.hpp"
#include "luce/utils/Pattern.hpp"
#include "luce/repl/IVisitor.hpp"
namespace accat::luce::repl::expression {
struct ASTPrinter : Visitor {
  ASTPrinter() = default;
  explicit ASTPrinter(const std::ostream &ofs) {
    outs.rdbuf(ofs.rdbuf());
  }
  virtual ~ASTPrinter() = default;
  virtual evaluation::result_type evaluate(const Expr &) override;
  virtual evaluation::result_type visit(const Undefined &) override;
  virtual evaluation::result_type visit(const Grouping &) override;
  virtual evaluation::result_type visit(const Literal &) override;
  virtual evaluation::result_type visit(const Variable &) override;
  virtual evaluation::result_type visit(const Unary &) override;
  virtual evaluation::result_type visit(const Binary &) override;
  virtual evaluation::result_type visit(const Logical &) override;

private:
  std::ostream outs{std::cout.rdbuf()};
};

struct Evaluator : Visitor, Component {
  Evaluator() = default;
  explicit Evaluator(Mediator *parent) : Component(parent) {}
  virtual ~Evaluator() = default;
  virtual evaluation::result_type evaluate(const Expr &) override;
  virtual evaluation::result_type visit(const Undefined &) override;
  virtual evaluation::result_type visit(const Grouping &) override;
  virtual evaluation::result_type visit(const Literal &) override;
  virtual evaluation::result_type visit(const Variable &) override;
  virtual evaluation::result_type visit(const Unary &) override;
  virtual evaluation::result_type visit(const Binary &) override;
  virtual evaluation::result_type visit(const Logical &) override;
};
} // namespace accat::luce::repl::expression
