#pragma once

#include "./config.hpp"
EXPORT_AUXILIA
namespace accat::auxilia {
#if __has_include(<fmt/format.h>)
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
#else
using ::std::format;
using ::std::format_to;
using ::std::print;
using ::std::println;
template <class... T>
void println(const auto &, std::format_string<T...> fmt, T &&...args) {
  std::print(fmt, std::forward<decltype(args)>(args)...);
  putchar('\n');
}
template <class... T>
void println(FILE *f, const auto &, std::format_string<T...> fmt, T &&...args) {
  std::print(f, fmt, std::forward<decltype(args)>(args)...);
  putchar('\n');
}
#endif

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
} // namespace accat::auxilia

namespace accat::auxilia {
template <Variantable... Ts> class Variant;
enum class FormatPolicy : uint8_t;
template <typename Derived> class Printable;
template <typename Derived> class Viewable;
template <typename Ty>
  requires std::is_arithmetic_v<std::remove_cvref_t<Ty>>
bool is_integer(Ty &&value) noexcept {
  return std::trunc(std::forward<Ty>(value)) == value;
}
template <typename... Ts> struct match : Ts... {
  using Ts::operator()...;
};
template <typename... Ts> match(Ts...) -> match<Ts...>;
enum class FormatPolicy : uint8_t {
  kDefault = 0,
  kDetailed = 1,
  kTokenOnly = 2,
};
/// @interface Printable
/// @brief A class that represents a printable object; can be directly
/// printed via `std::cout` or `fmt::print`.
/// @note use public inheritance to make fmt::print work.
template <typename Derived> class AC_NOVTABLE Printable {
public:
  using string_type = string;

public:
  constexpr Printable() = default;
  constexpr Printable(const Printable &) = default;
  constexpr Printable(Printable &&) noexcept = default;
  constexpr auto operator=(const Printable &) -> Printable & = default;
  constexpr auto operator=(Printable &&) noexcept -> Printable & = default;

protected:
  constexpr ~Printable() = default;

private:
  [[nodiscard]] auto
  _to_string(const FormatPolicy &format_policy = FormatPolicy::kDefault) const
      -> string_type
  {
    return static_cast<const Derived *>(this)->to_string(format_policy);
  }

private:
  friend auto operator<<(std::ostream &os, const Printable &p)
      -> std::ostream & {
    return os << p._to_string();
  }
  friend auto
  format_as(const Printable &p,
            const FormatPolicy &format_policy = FormatPolicy::kDefault)
      -> string_type {
    return p._to_string(format_policy);
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
  constexpr auto operator=(Viewable &&) noexcept -> Viewable & = default;

protected:
  constexpr ~Viewable() = default;

private:
  [[nodiscard]] auto _to_string_view(
      const FormatPolicy &format_policy = FormatPolicy::kDefault) const
      -> string_view_type
    requires requires(const Derived &d) {
      {
        d.to_string_view(format_policy)
      } -> std::convertible_to<string_view_type>;
    }
  {
    return static_cast<const Derived *>(this)->to_string_view(format_policy);
  }
};
} // namespace accat::auxilia

namespace std {
template <typename Derived>
  requires is_base_of_v<::accat::auxilia::Printable<Derived>,
                        Derived>
struct formatter<Derived> { // NOLINT(cert-dcl58-cpp)
  constexpr auto parse(format_parse_context &ctx) const { return ctx.begin(); }
  template <typename FormatContext>
  constexpr auto format(const Derived &p, FormatContext &ctx) const {
    return format_to(
        ctx.out(), "{}", p.to_string(::accat::auxilia::FormatPolicy::kDefault));
  }
};
} // namespace std
