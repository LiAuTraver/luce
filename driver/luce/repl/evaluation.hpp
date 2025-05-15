#pragma once

#include <fmt/format.h>
#include <compare>
#include <limits>
#include <optional>
#include <utility>
#include <variant>

#include "./luce/repl/repl_fwd.hpp"
#include <accat/auxilia/auxilia.hpp>

namespace accat::luce::repl::evaluation {
using namespace std::literals;

struct AC_EMPTY_BASES AC_NOVTABLE Evaluatable{};
struct AC_EMPTY_BASES AC_NOVTABLE Value : Evaluatable {
  friend constexpr bool operator==(const Value &, const Value &) noexcept {
    return true;
  }
  friend constexpr bool operator!=(const Value &, const Value &) noexcept {
    return false;
  }
};
struct Undefined : /* extends */ auxilia::Monostate,
                   /* implements */ Evaluatable {
  constexpr Undefined() = default;
  constexpr Undefined(const Undefined &) noexcept = default;
  constexpr Undefined(Undefined &&) noexcept = default;
  auto operator=(const Undefined &) noexcept -> Undefined & {
    return *this;
  }
  auto operator=(Undefined &&) noexcept -> Undefined & {
    return *this;
  }
  friend bool operator==(const Undefined &, const Undefined &) {
    return false;
  }
  friend bool operator!=(const Undefined &lhs, const Undefined &rhs) {
    return !(lhs == rhs);
  }
  constexpr ~Undefined() noexcept = default;
  constexpr auto to_string(
      const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault) const
      -> auxilia::Monostate::string_type {
    return "undefined"s;
  }
  constexpr auto
  to_string_view(const auxilia::FormatPolicy & =
                     auxilia::FormatPolicy::kDefault) const noexcept
      -> auxilia::Monostate::string_view_type {
    return "undefined"sv;
  }
};
struct Number : /* implements */ Value,
                /* implements */ auxilia::Printable {
  template <typename... Args> using match = auxilia::match<Args...>;
  using I = long long;
  using F = long double;
  using ValueType = std::variant<I, F>;
  constexpr Number() = default;
  inline static constexpr Number make_integer(I value) noexcept {
    return Number{value};
  }
  inline static constexpr Number make_floating(F value) noexcept {
    return Number{value};
  }
  inline static constexpr Number from_variant(ValueType &&value) noexcept {
    return Number{std::forward<ValueType>(value)};
  }

protected:
  inline explicit constexpr Number(I value) noexcept : value(value) {}
  inline explicit constexpr Number(F value) noexcept : value(value) {}
  inline explicit constexpr Number(ValueType &&value) noexcept
      : value(std::move(value)) {}
  inline explicit constexpr Number(const ValueType &value) noexcept
      : value(value) {}

public:
  constexpr ~Number() = default;
  auto to_string(const auxilia::FormatPolicy & =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    return std::visit(match(
                          [](const I &value) -> string_type {
                            return fmt::format("{:#x}", value);
                          },
                          [](const auto &value) -> string_type {
                            return fmt::format("{}", value);
                          }),
                      value);
  }

  friend auto operator<=>(const Number &lhs, const Number &rhs) {
    return std::visit(
        match([](const I &l,
                 const I &r) -> std::partial_ordering { return l <=> r; },
              [](const auto &l, const auto &r) -> std::partial_ordering {
                using auxilia::as;
                return as<F>(l) <=> as<F>(r);
              }),
        lhs.value,
        rhs.value);
  }
  constexpr friend auto operator==(const Number &lhs, const Number &rhs)
      -> bool {
    return lhs <=> rhs == std::partial_ordering::equivalent;
  }
  template <typename Num>
    requires std::is_arithmetic_v<Num>
  constexpr auto operator==(const Num &rhs) const -> bool {
    return std::visit([&](const auto &value) -> bool { return value == rhs; },
                      value);
  }
  constexpr explicit operator bool() const {
    return std::visit(
        [](const auto &value) -> bool { return static_cast<bool>(value); },
        value);
  }
  // unary operator-
  auto operator-() const {
    return std::visit(
        [](const auto &value) -> Number { return Number{-value}; }, value);
  }
#define BINARY_OP(_binary_op_)                                                 \
  friend auto operator _binary_op_(const Number &lhs, const Number &rhs)       \
      ->Number {                                                               \
    return std::visit(                                                         \
        match([](const I &l, const I &r) { return Number{l _binary_op_ r}; },  \
              [](const auto &l, const auto &r) {                               \
                using auxilia::as;                                             \
                return Number{as<F>(l) _binary_op_ as<F>(r)};                  \
              }),                                                              \
        lhs.value,                                                             \
        rhs.value);                                                            \
  }
  BINARY_OP(+)
  BINARY_OP(-)
  BINARY_OP(*)
#undef BINARY_OP
  friend auto operator/(const Number &lhs, const Number &rhs) -> Number {
    using auxilia::as;
    if (rhs == 0)
      return Number{std::numeric_limits<F>::infinity()};
    return std::visit(match(
                          [](const I &l, const I &r) {
                            if (l % r == 0)
                              return Number{l / r};
                            return Number{as<F>(l) / as<F>(r)};
                          },
                          [](const auto &l, const auto &r) {
                            return Number{as<F>(l) / as<F>(r)};
                          }),
                      lhs.value,
                      rhs.value);
  }
  constexpr auto integer() const noexcept -> std::optional<I> {
    if (std::holds_alternative<I>(value))
      return std::make_optional(std::get<I>(value));
    return std::nullopt;
  }
  constexpr auto floating() const noexcept -> std::optional<F> {
    if (std::holds_alternative<F>(value))
      return std::make_optional(std::get<F>(value));
    return std::nullopt;
  }

private:
  ValueType value;
};

struct Byte : /* implements */ Value,
              /* implements */ auxilia::Printable {
  using ValueType = std::byte;
  constexpr Byte() = default;
  explicit constexpr Byte(ValueType value) : value(value) {}
  constexpr ~Byte() = default;
  auto to_string(const auxilia::FormatPolicy & =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    return fmt::format("0x{:02x}", static_cast<int>(value));
  }
  friend auto operator==(const Byte &lhs, const Byte &rhs) -> bool {
    return lhs.value == rhs.value;
  }
  friend auto operator<=>(const Byte &lhs, const Byte &rhs) {
    return lhs.value <=> rhs.value;
  }
  ValueType value;
};
struct String : /* implements */ Value,
                /* implements */ auxilia::Printable {
  constexpr String() = default;
  constexpr String(string_type value) : value(std::move(value)) {}
  constexpr ~String() = default;
  constexpr explicit operator bool() const {
    return not value.empty();
  }
  auto to_string(const auxilia::FormatPolicy & =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    return "\""s + value + "\""s;
  }
  constexpr friend auto operator==(const String &lhs, const String &rhs)
      -> bool {
    return lhs.value == rhs.value;
  }
  friend auto operator<=>(const String &lhs, const String &rhs) {
    return lhs.value <=> rhs.value;
  }
  friend auto operator+(const String &lhs, const String &rhs) -> String {
    return String{lhs.value + rhs.value};
  }
  string_type value;
};
struct Boolean : /* implements */ Value,
                 /* implements */ auxilia::Printable,
                 /* implements */ auxilia::Viewable {
  static Boolean make_true() {
    return {true};
  }
  static Boolean make_false() {
    return {false};
  }
  constexpr Boolean() = default;
  constexpr Boolean(bool value) : value(value) {}
  constexpr ~Boolean() = default;
  auto to_string(const auxilia::FormatPolicy & =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    return value ? "true"s : "false"s;
  }
  auto to_string_view(const auxilia::FormatPolicy & =
                          auxilia::FormatPolicy::kDefault) const noexcept
      -> string_view_type {
    return value ? "true"sv : "false"sv;
  }
  constexpr explicit operator bool() const {
    return value;
  }
  friend auto operator==(const Boolean &lhs, const Boolean &rhs) {
    return lhs.value == rhs.value;
  }
  friend auto operator!=(const Boolean &lhs, const Boolean &rhs) {
    return lhs.value != rhs.value;
  }
  friend auto operator&&(const Boolean &lhs, const Boolean &rhs) {
    return Boolean{lhs.value && rhs.value};
  }
  friend auto operator||(const Boolean &lhs, const Boolean &rhs) {
    return Boolean{lhs.value || rhs.value};
  }

private:
  bool value{};
} inline constexpr True{true}, False{false};

struct Nil : /* implements */ Value,
             /* implements */ auxilia::Printable,
             /* implements */ auxilia::Viewable {
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
