#pragma once

#include <fmt/base.h>
#include <fmt/color.h>
#include <any>
#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <cmath>

#include "config.hpp"
#include <fmt/core.h>
#include <fmt/format.h>

namespace accat::auxilia {
template <Variantable... Ts> class Variant;
enum FormatPolicy : uint8_t;
template <typename Derived> class Printable;
template <typename Derived> class Viewable;
using ::fmt::format;
using ::fmt::format_to;
using ::fmt::print;
using ::fmt::println;
template <class... T>
void println(const fmt::text_style &ts,
             fmt::format_string<T...> fmt,
             T &&...args) {
  fmt::print(ts, fmt, std::forward<decltype(args)>(args)...);
  putchar('\n');
}
template <class... T>
void println(FILE *f,
             const fmt::text_style &ts,
             fmt::format_string<T...> fmt,
             T &&...args) {
  fmt::print(f, ts, fmt, std::forward<decltype(args)>(args)...);
  putchar('\n');
}

// clang-format off
/*!
   @brief cast the literal to the specified type and also log the value in debug mode.
   @note clang-cl.exe does not fully support `try` and `catch` blocks but
   `any_cast` will throw an exception if the cast fails. For more information, see
   <a href="https://clang.llvm.org/docs/MSVCCompatibility.html#asynchronous-exceptions">Asynchronous Exceptions</a>,
   <a href="https://stackoverflow.com/questions/7049502/c-try-and-try-catch-finally">try-catch-finally</a>,
   and <a href="https://learn.microsoft.com/en-us/cpp/cpp/try-except-statement">try-except-statement</a>.
   @deprecated use @link std::any_cast @endlink instead.
 */
 void _deprecated_any_cast();
// the note's function has been removed so the note is not needed above.
// clang-format on
template <typename Ty>
  requires std::is_arithmetic_v<std::remove_cvref_t<Ty>>
bool is_integer(Ty &&value) noexcept {
  return std::trunc(std::forward<Ty>(value)) == value;
}
template <typename... Ts> struct match : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> match(Ts...) -> match<Ts...>;
enum FormatPolicy : uint8_t {
  kDefault = 0,
};
/// @interface Printable
/// @brief A class that represents a printable object; can be directly
/// printed via `std::cout` or `fmt::print`.
template <typename Derived> class AC_NOVTABLE Printable {
public:
  using string_type = std::string;

public:
  constexpr Printable() = default;
  constexpr Printable(const Printable &) = default;
  constexpr Printable(Printable &&) noexcept = default;
  constexpr auto operator=(const Printable &)
      -> Printable & = default;
  constexpr auto operator=(Printable &&) noexcept
      -> Printable & = default;

protected:
  constexpr ~Printable() = default;

public:
  [[nodiscard]] auto
  to_string(const FormatPolicy &format_policy = kDefault) const
      -> string_type
    requires requires(const Derived &d) {
      {
        d.to_string_impl(format_policy)
      } -> std::convertible_to<string_type>;
    }
  {
    return static_cast<const Derived *>(this)->to_string_impl(
        format_policy);
  }

private:
  friend auto operator<<(std::ostream &os, const Printable &p)
      -> std::ostream & {
    return os << p.to_string();
  }
  friend auto format_as(const Printable &p,
                        const FormatPolicy &format_policy = kDefault)
      -> string_type {
    return p.to_string(format_policy);
  }
};

/// @interface Viewable
template <typename Derived> class AC_NOVTABLE Viewable {
public:
  using string_view_type = std::string_view;

public:
  constexpr Viewable() = default;
  constexpr Viewable(const Viewable &) = default;
  constexpr Viewable(Viewable &&) noexcept = default;
  constexpr auto operator=(const Viewable &) -> Viewable & = default;
  constexpr auto operator=(Viewable &&) noexcept
      -> Viewable & = default;

protected:
  constexpr ~Viewable() = default;

public:
  [[nodiscard]] auto
  to_string_view(const FormatPolicy &format_policy = kDefault) const
      -> string_view_type
    requires requires(const Derived &d) {
      {
        d.to_string_view_impl(format_policy)
      } -> std::convertible_to<string_view_type>;
    }
  {
    return static_cast<const Derived *>(this)->to_string_view_impl(
        format_policy);
  }
};
} // namespace accat::auxilia

template <typename Derived>
  requires std::is_base_of_v<
      ::accat::auxilia::Printable<Derived>,
      Derived>
struct ::std::formatter<Derived> { // NOLINT(cert-dcl58-cpp)
  constexpr auto parse(::std::format_parse_context &ctx) const {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(const Derived &p, FormatContext &ctx) const {
    return std::format_to(
        ctx.out(),
        "{}",
        p.to_string(
            ::accat::auxilia::FormatPolicy::kDefault));
  }
};
