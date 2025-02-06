#include "deps.hh"

#include "luce/repl/expression.hpp"
#include "luce/repl/IVisitor.hpp"
#include "luce/repl/evaluation.hpp"
#include "luce/repl/repl_fwd.hpp"
namespace accat::luce::repl::expression {
#define ACCEPT_VISIT(_type_)                                                   \
  auto _type_::accept(expression::Visitor &_visitor_) const                    \
      -> evaluation::result_type {                                             \
    return _visitor_.visit(*this);                                             \
  }

ACCEPT_VISIT(Undefined)
ACCEPT_VISIT(Grouping)
ACCEPT_VISIT(Literal)
ACCEPT_VISIT(Unary)
ACCEPT_VISIT(Binary)
ACCEPT_VISIT(Logical)

#undef ACCEPT_VISIT
} // namespace accat::luce::repl::expression
