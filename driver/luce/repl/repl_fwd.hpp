#pragma once

#include <accat/auxilia/details/config.hpp>

namespace accat::auxilia {
template <typename> class StatusOr;
template <typename...> class Variant;
} // namespace accat::auxilia

namespace accat::luce::repl::expression {
struct Visitor;
struct Expr;
struct Undefined;
struct Grouping;
struct Literal;
struct Variable;
struct Unary;
struct Binary;
struct Logical;
} // namespace accat::luce::repl::expression

namespace accat::luce::repl::statement {
struct Stmt;
}

namespace accat::luce::repl::evaluation {
struct Evaluatable;
struct Value;
struct Undefined;
struct Number;
struct Byte;
struct String;
struct Boolean;
struct Nil;
} // namespace accat::luce::repl::evaluation
