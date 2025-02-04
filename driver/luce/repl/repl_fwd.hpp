#pragma once
#include <accat/auxilia/details/config.hpp>

namespace accat::auxilia {
template <typename> class StatusOr;
template <typename... Types> class Variant;
} // namespace accat::auxilia

namespace accat::luce {}

namespace accat::luce::repl {}

namespace accat::luce::repl::expression {
interface Expr;
struct Undefined;
struct Grouping;
struct Literal;
struct Unary;
struct Binary;
struct Logical;
} // namespace accat::luce::repl::expression

namespace accat::luce::repl::statement {
interface Stmt;
}

namespace accat::luce::repl::evaluation {
interface Evaluatable;
interface Value;
struct Undefined;
struct Number;
struct String;
struct Boolean;
struct Nil;

using variant_type = auxilia::Variant<Undefined, Number, String, Boolean, Nil>;
using result_type = auxilia::StatusOr<variant_type>;
} // namespace accat::luce::repl::evaluation
