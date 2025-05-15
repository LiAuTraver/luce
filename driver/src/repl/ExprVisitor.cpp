#include "deps.hh"

// order matters here
#include "luce/repl/evaluation.hpp"
#include <accat/auxilia/auxilia.hpp>

#include "luce/repl/ExprVisitor.hpp"
#include "luce/Monitor.hpp"
#include "luce/repl/Token.hpp"
#include "luce/repl/expression.hpp"
#include "luce/repl/repl_fwd.hpp"

namespace accat::luce::repl::expression {
using evaluation::result_type;
result_type ASTPrinter::evaluate(const Expr &expr) {
  return expr.accept(*this);
}
#define VISIT_PRINT(_type_)                                                    \
  result_type ASTPrinter::visit(const _type_ &expr) {                          \
    outs << expr.to_string(auxilia::FormatPolicy::kBrief) << '\n';             \
    return {};                                                                 \
  }
VISIT_PRINT(Undefined)
VISIT_PRINT(Grouping)
VISIT_PRINT(Literal)
VISIT_PRINT(Variable)
VISIT_PRINT(Unary)
VISIT_PRINT(Binary)
VISIT_PRINT(Logical)
#undef VISIT_PRINT

result_type Evaluator::evaluate(const Expr &expr) {
  return expr.accept(*this);
}
result_type Evaluator::visit(const Undefined &expr) {
  spdlog::error("Unknown expression: {}",
                expr.to_string(auxilia::FormatPolicy::kDetailed));
  return {};
}
result_type Evaluator::visit(const Grouping &expr) {
  return evaluate(*expr.expression);
}

using enum Token::Type;
using auxilia::InvalidArgumentError;
using auxilia::match;

result_type Evaluator::visit(const Literal &expr) {
  switch (expr.value.type()) {
  case kIdentifier:
    dbg_break
    break;
  case kString:
    return {{evaluation::String{std::string{
        expr.value.lexeme().begin() + 1,
        expr.value.lexeme().end() - 1}}}}; // remove `"` from the string
  case kNumber:
    return {{evaluation::Number::from_variant(expr.value.number())}};
  case kFalse:
    return {{evaluation::False}};
  case kNil:
    return {{evaluation::nil}};
  case kTrue:
    return {{evaluation::True}};
  case kLexError:
    return {InvalidArgumentError("Lex error: {} at line {}",
                                 expr.value.error_message(),
                                 expr.value.line())};
  case kEndOfFile:
    return {
        InvalidArgumentError("unexpected EOF at line {}", expr.value.line())};
  default:
    break;
  }

  // TODO: implement the rest of the literal types
  return {auxilia::UnimplementedError(
      "Unimplemented literal type: {}",
      expr.to_string(auxilia::FormatPolicy::kDetailed))};
}
result_type Evaluator::visit(const Variable &expr) {
  const auto ident = expr.identifier();
  if (ident.front() == '$') {
    if (ident.size() <= 2)
      return {InvalidArgumentError("'{}' is not a valid identifier", ident)};

    if (!this->mediator)
      return {auxilia::UnavailableError("Running evaluator without mediator, "
                                        "accessing registers is unavailable.")};

    auto *monitor = static_cast<Monitor *>(this->mediator);

    if (ident.substr(1) == "pc")
      // currently just cpus[0]
      return {{evaluation::Number::make_integer(monitor->cpus().pc(0).num())}};

    if (auto maybe_bytes = monitor->registers().read(ident.substr(1)))
      return {{evaluation::Number::make_integer(maybe_bytes->num())}};

    return {InvalidArgumentError("unknown register: {}", ident.substr(1))};
  }
  // TODO(variable evaluation is not implemented)
  return {auxilia::UnimplementedError(
      "Unimplemented variable evaluation: {}",
      expr.to_string(auxilia::FormatPolicy::kDetailed))};
}
result_type Evaluator::visit(const Unary &expr) {
  auto maybe_right = evaluate(*expr.right);
  if (!maybe_right.ok()) {
    return maybe_right;
  }
  auto right = *std::move(maybe_right);
  if (right.empty()) { // holds a unknown (represents monostate)
    return {};
  }
  auto pattern = match(
      [&](const evaluation::Number &num) -> result_type {
        switch (expr.op.type()) {
        case kMinus:
          return {{evaluation::Number{-num}}};
        case kBang:
          return {{evaluation::Boolean{!num}}};
        case kStar:
          // in this repl, a number might represent an addr
          if (auto myInt = num.integer()) {
            // may be a pointer
            if (this->mediator) {
              precondition(dynamic_cast<Monitor *>(mediator),
                           "mediator must be a Monitor");
              auto monitor = static_cast<Monitor *>(mediator);
              if (auto res = monitor->memory().read_n(*myInt, 1)) {
                return {{evaluation::Byte{res->front()}}};
              } else {
                return {res.as_status()};
              }
            }
            return {
                InvalidArgumentError("Running evaluator without mediator, "
                                     "pointer dereference is unavailable.")};
          }
          return {InvalidArgumentError("cannot dereference a number")};
        default:
          break;
        }
        DebugUnreachable()
        return {InvalidArgumentError("unknown unary operator")};
      },
      [&](const evaluation::String &str) -> result_type {
        switch (expr.op.type()) {
        case kMinus:
          return {InvalidArgumentError("cannot negate a string")};
        case kBang:
          return {{evaluation::Boolean{!!str}}};
        case kStar:
          return {InvalidArgumentError("cannot dereference or a string")};
        case kAmpersand:
          return {InvalidArgumentError("cannot take address of a string")};
        default:
          break;
        }
        DebugUnreachable()
        return {InvalidArgumentError("unknown unary operator")};
      },
      [&](const evaluation::Nil &) -> result_type {
        switch (expr.op.type()) {
        case kMinus:
          return {InvalidArgumentError("cannot negate a nil")};
        case kBang:
          return {{evaluation::True}};
        case kStar:
          return {InvalidArgumentError("null pointer dereference")};
        case kAmpersand:
          return {InvalidArgumentError("cannot take address of a nil")};
        default:
          break;
        }
        return {InvalidArgumentError("unknown unary operator for nil: " +
                                     expr.op.to_string())};
      },
      [&](const evaluation::Boolean &b) -> result_type {
        switch (expr.op.type()) {
        case kMinus:
          return {InvalidArgumentError("cannot negate a boolean")};
        case kBang:
          return {{evaluation::Boolean{!b}}};
        case kStar:
          return {InvalidArgumentError("cannot dereference a boolean")};
        case kAmpersand:
          return {InvalidArgumentError("cannot take address of a boolean")};
        default:
          break;
        }
        return {InvalidArgumentError("unknown unary operator")};
      },
      [](const auto &whatever) -> result_type {
        spdlog::error("Unknown unary evaluation type: {}",
                      typeid(whatever).name());
        return {InvalidArgumentError("unknown evaluation type")};
      });
  return right.visit(pattern);
}
result_type Evaluator::visit(const Binary &expr) {
  auto maybe_left = evaluate(*expr.left);
  if (!maybe_left.ok() || maybe_left->empty()) {
    return maybe_left;
  }
  auto maybe_right = evaluate(*expr.right);
  if (!maybe_right.ok() || maybe_right->empty()) {
    return maybe_right;
  }
  auto left = *std::move(maybe_left);
  auto right = *std::move(maybe_right);
  auto pattern = match(
      [&](const evaluation::Number &lhs,
          const evaluation::Number &rhs) -> result_type {
        switch (expr.op.type()) {
        case kPlus:
          return {{lhs + rhs}};
        case kMinus:
          return {{lhs - rhs}};
        case kStar:
          return {{lhs * rhs}};
        case kSlash:
          return {{lhs / rhs}};
        case kGreater:
          return {{evaluation::Boolean{lhs > rhs}}};
        case kGreaterEqual:
          return {{evaluation::Boolean{lhs >= rhs}}};
        case kLess:
          return {{evaluation::Boolean{lhs < rhs}}};
        case kLessEqual:
          return {{evaluation::Boolean{lhs <= rhs}}};
        case kEqualEqual:
          return {{evaluation::Boolean{lhs == rhs}}};
        case kBangEqual:
          return {{evaluation::Boolean{lhs != rhs}}};
        default:
          break;
        }
        return {InvalidArgumentError("unknown binary operator")};
      },
      [&](const evaluation::String &lhs,
          const evaluation::String &rhs) -> result_type {
        switch (expr.op.type()) {
        case kPlus:
          if (!lhs)
            return {{rhs}};
          if (!rhs)
            return {{lhs}};
          return {{lhs + rhs}};
        case kEqualEqual:
          return {{evaluation::Boolean{lhs == rhs}}};
        case kBangEqual:
          return {{evaluation::Boolean{lhs != rhs}}};
        case kMinus:
          return {InvalidArgumentError("cannot subtract strings")};
        default:
          break;
        }
        return {InvalidArgumentError("unknown binary operator")};
      },
      [&](const evaluation::Boolean &lhs,
          const evaluation::Boolean &rhs) -> result_type {
        switch (expr.op.type()) {
        case kOr:
          return {{lhs || rhs}};
        case kAnd:
          return {{lhs && rhs}};
        case kEqualEqual:
          return {evaluation::Boolean{lhs == rhs}};
        case kBangEqual:
          return {evaluation::Boolean{lhs != rhs}};
        default:
          break;
        }
        return {InvalidArgumentError("unknown binary operator for boolean")};
      },
      [&](const evaluation::Nil &, const evaluation::Nil &) -> result_type {
        switch (expr.op.type()) {
        case kEqualEqual:
          return {{evaluation::True}};
        case kBangEqual:
          return {{evaluation::False}};
        default:
          break;
        }
        return {InvalidArgumentError("unknown binary operator for nil")};
      },
      [&](const auto &lhs, const auto &rhs) -> result_type {
        spdlog::error("Unknown binary evaluation type: {} with {}",
                      typeid(lhs).name(),
                      typeid(rhs).name());
        return {InvalidArgumentError("unknown evaluation type")};
      });
  return auxilia::visit(pattern, std::move(left), std::move(right));
}
result_type Evaluator::visit(const Logical &expr) {
  auto maybe_left = evaluate(*expr.left);
  if (!maybe_left.ok() || maybe_left->empty()) {
    return maybe_left;
  }
  auto maybe_right = evaluate(*expr.right);
  if (!maybe_right.ok() || maybe_right->empty()) {
    return maybe_right;
  }
  auto left = *std::move(maybe_left);
  auto right = *std::move(maybe_right);
  auto pattern = match(
      [&](const evaluation::Boolean &lhs,
          const evaluation::Boolean &rhs) -> result_type {
        switch (expr.op.type()) {
        case kOr:
          return {{lhs || rhs}};
        case kAnd:
          return {{lhs && rhs}};
        default:
          break;
        }
        return {InvalidArgumentError("unknown logical operator")};
      },
      [&](const auto &lhs, const auto &rhs) -> result_type {
        spdlog::error("Unknown logical evaluation type: {} with {}",
                      typeid(lhs).name(),
                      typeid(rhs).name());
        return {InvalidArgumentError("unknown evaluation type")};
      });
  return auxilia::visit(pattern, std::move(left), std::move(right));
}
} // namespace accat::luce::repl::expression
