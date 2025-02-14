#pragma once

#include <optional>
#include "accat/auxilia/details/config.hpp"
#include "./repl_fwd.hpp"
namespace accat::auxilia {
template <typename> class StatusOr;
}
namespace accat::luce::repl {
interface IVisitor{};
}
namespace accat::luce::repl::expression {


struct Visitor {
  virtual evaluation::result_type evaluate(const Expr &) = 0;
  virtual evaluation::result_type visit(const Undefined &) = 0;
  virtual evaluation::result_type visit(const Grouping &) = 0;
  virtual evaluation::result_type visit(const Literal &) = 0;
  virtual evaluation::result_type visit(const Variable &) = 0;
  virtual evaluation::result_type visit(const Unary &) = 0;
  virtual evaluation::result_type visit(const Binary &) = 0;
  virtual evaluation::result_type visit(const Logical &) = 0;
};
}

