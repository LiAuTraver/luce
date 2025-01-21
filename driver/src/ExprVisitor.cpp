#include "deps.hh"

#include "luce/repl/ExprVisitor.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>
#include <variant>
#include "accat/auxilia/auxilia.hpp"
#include "accat/auxilia/details/format.hpp"
#include "luce/repl/Token.hpp"
#include "luce/repl/evaluation.hpp"
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
  case kIdentifier: // TODO: implement identifier
    break;
  case kString:
    return {{evaluation::String{std::string{expr.value.lexeme()}}}};
  case kNumber:
    return {{evaluation::Number{expr.value.number()}}};
  case kFalse:
    return {{evaluation::False}};
  case kNil:
    return {evaluation::nil};
  case kTrue:
    return {{evaluation::True}};
  case kLexError:
    return {InvalidArgumentError(fmt::format("Lex error: {} at line {}",
                                             expr.value.error_message(),
                                             expr.value.line()))};
  case kEndOfFile:
    return {InvalidArgumentError(
        fmt::format("unexpected EOF at line {}", expr.value.line()))};
  default:
    break;
  }

  // TODO: implement the rest of the literal types
  return {auxilia::UnimplementedError(
      fmt::format("Unimplemented literal type: {}",
                  expr.to_string(auxilia::FormatPolicy::kDetailed)))};
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
  auto pattern = match{
      [&](const evaluation::Number &num) -> result_type {
        if (expr.op.is_type(kMinus)) {
          return {{evaluation::Number{-num.value}}};
        }
        if (expr.op.is_type(kBang)) {
          return {{evaluation::Boolean{!num.value}}};
        }
        return {InvalidArgumentError("unknown unary operator")};
      },
      [&](const evaluation::String str) -> result_type {
        switch (expr.op.type()) {
        case kMinus:
          return {InvalidArgumentError("cannot negate a string")};
        case kBang:
          return {{evaluation::Boolean{str.value.empty()}}};
        case kStar:
          return {InvalidArgumentError("cannot dereference or a string")};
        case kAmpersand:
          return {InvalidArgumentError("cannot take address of a string")};
        default:
          break;
        }
        dbg_break
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
          return {{evaluation::Boolean{!b.value}}};
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
      }};
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
  auto pattern = match{
      [&](const evaluation::Number &lhs,
          const evaluation::Number &rhs) -> result_type {
        switch (expr.op.type()) {
        case kPlus:
          return {{evaluation::Number{lhs.value + rhs.value}}};
        case kMinus:
          return {{evaluation::Number{lhs.value - rhs.value}}};
        case kStar:
          return {{evaluation::Number{lhs.value * rhs.value}}};
        case kSlash:
          if (rhs.value == 0) {
            return {InvalidArgumentError("division by zero")};
          }
          return {{evaluation::Number{lhs.value / rhs.value}}};
        case kGreater:
          return {{evaluation::Boolean{lhs.value > rhs.value}}};
        case kGreaterEqual:
          return {{evaluation::Boolean{lhs.value >= rhs.value}}};
        case kLess:
          return {{evaluation::Boolean{lhs.value < rhs.value}}};
        case kLessEqual:
          return {{evaluation::Boolean{lhs.value <= rhs.value}}};
        case kEqualEqual:
          return {{evaluation::Boolean{lhs.value == rhs.value}}};
        case kBangEqual:
          return {{evaluation::Boolean{lhs.value != rhs.value}}};
        default:
          break;
        }
        return {InvalidArgumentError("unknown binary operator")};
      },
      [&](const evaluation::String &lhs,
          const evaluation::String &rhs) -> result_type {
        switch (expr.op.type()) {
        case kPlus:
          return {{evaluation::String{lhs.value + rhs.value}}};
        case kEqualEqual:
          return {{evaluation::Boolean{lhs.value == rhs.value}}};
        case kBangEqual:
          return {{evaluation::Boolean{lhs.value != rhs.value}}};
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
          return {{evaluation::Boolean{lhs.value || rhs.value}}};
        case kAnd:
          return {{evaluation::Boolean{lhs.value && rhs.value}}};
        case kEqualEqual:
          return {{evaluation::Boolean{lhs.value == rhs.value}}};
        case kBangEqual:
          return {{evaluation::Boolean{lhs.value != rhs.value}}};
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
      }};
  return std::visit(pattern, std::move(left).get(), std::move(right).get());
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
  auto pattern =
      match{[&](const evaluation::Boolean &lhs,
                const evaluation::Boolean &rhs) -> result_type {
              switch (expr.op.type()) {
              case kOr:
                return {{evaluation::Boolean{lhs.value || rhs.value}}};
              case kAnd:
                return {{evaluation::Boolean{lhs.value && rhs.value}}};
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
            }};
  return std::visit(pattern, std::move(left).get(), std::move(right).get());
}
} // namespace accat::luce::repl::expression
