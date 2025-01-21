#pragma once

#include <fmt/format.h>
#include <limits>

#include "./luce/repl/repl_fwd.hpp"
#include "accat/auxilia/details/Monostate.hpp"
#include "accat/auxilia/details/Variant.hpp"
#include "accat/auxilia/details/format.hpp"
namespace accat::luce::repl::evaluation {
using namespace std::literals;

interface Evaluatable{};
interface Value : Evaluatable{};
struct Undefined : /* extends */ auxilia::Monostate,
                 /* implements */ Evaluatable,
                 /* implements */ auxilia::Printable<Undefined>,
                 /* implements */ auxilia::Viewable<Undefined> {
  constexpr Undefined() = default;
  constexpr Undefined(const Undefined &) noexcept = default;
  constexpr Undefined(Undefined &&) noexcept = default;
  auto operator=(const Undefined &) noexcept -> Undefined & = default;
  auto operator=(Undefined &&) noexcept -> Undefined & = default;
  constexpr ~Undefined() noexcept = default;
  constexpr auto to_string(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault)
      const -> auxilia::Monostate::string_type {
    return "undefined"s;
  }
  constexpr auto to_string_view(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault) const noexcept
      -> auxilia::Monostate::string_view_type {
    return "undefined"sv;
  }
};
struct Number : /* implements */ Value,
                /* implements */ auxilia::Printable<Number> {
  long double value{std::numeric_limits<long double>::quiet_NaN()};
  constexpr Number() = default;
  constexpr Number(long double value) : value(value) {}
  constexpr ~Number() = default;
  auto to_string(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault)
      const -> string_type {
    return fmt::format("{}", value);
  }
};
struct String : /* implements */ Value,
                /* implements */ auxilia::Printable<String> {
  string_type value;
  constexpr String() = default;
  constexpr String(string_type value) : value(std::move(value)) {}
  constexpr ~String() = default;
  auto to_string(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault)
      const -> string_type {
    return value;
  }
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
  auto to_string(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault)
      const -> string_type {
    return value ? "true"s : "false"s;
  }
  auto to_string_view(const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault) const noexcept
      -> string_view_type {
    return value ? "true"sv : "false"sv;
  }

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
  constexpr auto to_string_view(const auxilia::FormatPolicy &) const noexcept
      -> string_view_type {
    return "nil"sv;
  }
  constexpr auto to_string(const auxilia::FormatPolicy & =
                               auxilia::FormatPolicy::kDefault) const noexcept
      -> string_type {
    return "nil"s;
  }
} inline constexpr nil = Nil{};
} // namespace accat::luce::repl::evaluation
