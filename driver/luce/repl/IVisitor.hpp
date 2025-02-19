#pragma once

#include <optional>

#include "accat/auxilia/details/config.hpp"
#include "./repl_fwd.hpp"
namespace accat::auxilia {
template <typename> class StatusOr;
}
namespace accat::luce::repl {
struct IVisitor {
protected:
  IVisitor() = default;
  virtual ~IVisitor() = default;
};
} // namespace accat::luce::repl
namespace accat::luce::repl::evaluation {
using variant_type =
    auxilia::Variant<Undefined, Number, Byte, String, Boolean, Nil>;
using result_type = auxilia::StatusOr<variant_type>;
} // namespace accat::luce::repl::evaluation
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

protected:
  Visitor() = default;
  virtual ~Visitor() = default;
};
} // namespace accat::luce::repl::expression
