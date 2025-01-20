#pragma once

#include <limits>
#include "accat/auxilia/details/Monostate.hpp"
#include "accat/auxilia/details/Variant.hpp"
#include "accat/auxilia/details/format.hpp"
namespace accat::luce::repl::evaluation {
using namespace std::literals;

interface Evaluatable{};
interface Value : Evaluatable{};
struct Unknown : /* extends */ auxilia::Monostate,
                 /* implements */ Evaluatable,
                 /* implements */ auxilia::Printable<Unknown> {
  constexpr Unknown() = default;
  constexpr Unknown(const Unknown &) noexcept = default;
  constexpr Unknown(Unknown &&) noexcept = default;
  auto operator=(const Unknown &) noexcept -> Unknown & = default;
  auto operator=(Unknown &&) noexcept -> Unknown & = default;
  constexpr ~Unknown() noexcept = default;
};
struct Number : /* implements */ Value,
                /* implements */ auxilia::Printable<Number> {
  long double value{std::numeric_limits<long double>::quiet_NaN()};
  constexpr Number() = default;
  constexpr Number(long double value) : value(value) {}
  constexpr ~Number() = default;
};
struct String : /* implements */ Value,
                /* implements */ auxilia::Printable<String> {
  string_type value;
  constexpr String() = default;
  constexpr String(string_type value) : value(std::move(value)) {}
  constexpr ~String() = default;
};
struct Boolean : /* implements */ Value,
                 /* implements */ auxilia::Printable<Boolean>,
                 /* implements */ auxilia::Viewable<Boolean> {
  bool value{};
  static Boolean make_true() {
    return {true};
  }
  static Boolean make_false() {
    return {false};
  }
  constexpr Boolean() = default;
  constexpr Boolean(bool value) : value(value) {}
  constexpr ~Boolean() = default;

} inline constexpr True{true}, False{false};

struct Nil : /* implements */ Value,
             /* implements */ auxilia::Printable<Nil>,
             /* implements */ auxilia::Viewable<Nil> {
  constexpr Nil() noexcept = default;
  constexpr Nil(const Nil &) noexcept = default;
  constexpr Nil(Nil &&) noexcept = default;
  constexpr auto operator=(const Nil &) noexcept -> Nil & = default;
  /* consteval */ auto operator=(Nil &&) noexcept -> Nil & = default;
  constexpr ~Nil() noexcept = default;
  consteval auto to_string_view(const auxilia::FormatPolicy &) const noexcept
      -> string_view_type {
    return "nil"sv;
  }
  consteval auto to_string(const auxilia::FormatPolicy & =
                               auxilia::FormatPolicy::kDefault) const noexcept
      -> string_type {
    return "nil"s;
  }
} inline constexpr nil = Nil{};

using variant_type = auxilia::Variant<Unknown, Number, String, Boolean, Nil>;
using eval_result_t = auxilia::StatusOr<variant_type>;
} // namespace accat::luce::repl::evaluation
