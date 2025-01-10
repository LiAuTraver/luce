#pragma once

#include "./variadic-inl.h"

#if defined(AC_CPP_DEBUG)
/// @def AC_UTILS_DEBUG_ENABLED
/// @note only defined in debug mode; never define it when submitting
/// the code or it'll mess up the test. #AC_CPP_DEBUG was defined in
/// the CMakeLists.txt, which can be turned on by set the environment
/// variable `AC_CPP_DEBUG` to `ON`.
/// @attention debug mode needs external libraries to work properly,
/// namely, `fmt`, `spdlog`, and potentially `gtest` and `Google
/// Benchmark`. release mode carries no dependencies and only requires
/// C++23.
#  define AC_UTILS_DEBUG_ENABLED 1
/// @def AC_UTILS_USE_FMT_FORMAT
/// @note use fmt::print, fmt::println when compiling with
/// clang-cl.exe will cause some wired error: Critical error detected
/// c0000374 A breakpoint instruction (__debugbreak() statement or a
/// similar call) was executed, which related to heap corruption. The
/// program will terminate.
/// @internal now it's ok again, why? maybe I called vcvarsall.bat?
#  define AC_UTILS_USE_FMT_FORMAT 1
/// @def AC_NO_SANITIZE_ADDRESS
#  ifdef __clang__
#    define AC_NO_SANITIZE_ADDRESS [[clang::no_sanitize("address")]]
#  elifdef __GNUC__
#    define AC_NO_SANITIZE_ADDRESS __attribute__((no_sanitize("address")))
#  elif defined(_MSC_VER)
#    define AC_NO_SANITIZE_ADDRESS __declspec(no_sanitize_address)
#  else
#    define AC_NO_SANITIZE_ADDRESS
#  endif
#endif

#ifdef AUXILIA_BUILD_MODULE
#  define EXPORT_AUXILIA export
#  define AUXILIA_USE_STD_MODULE
#else
#  define EXPORT_AUXILIA
#  if AC_UTILS_DEBUG_ENABLED
#    include <spdlog/spdlog.h>
#  endif
#endif

#include "./includes-inl.hpp"

#if __cpp_static_call_operator >= 202207L
#  define AC_STATIC_CALL_OPERATOR static
#else
#  define AC_STATIC_CALL_OPERATOR
#endif

/// @note GNU on Windows seems failed to perform linking for
/// `stacktrace` and `spdlog`.
#if defined(AC_UTILS_DEBUG_ENABLED)
#  if __has_include(<fmt/format.h>)
#    define AC_UTILS_STACKTRACE                                                \
      (::std::format("\n{}", ::std::stacktrace::current()))
#  else
template <>
struct ::fmt::formatter<::std::stacktrace> : ::fmt::formatter<::std::string> {
  template <typename FormatContext>
  auto format(const ::std::stacktrace &st, FormatContext &ctx)
      -> decltype(ctx.out()) {
    std::string result;
    for (const auto &entry : st) {
      result += fmt::format("{} {} {} {}\n",
                            entry.description(),
                            entry.native_handle(),
                            entry.source_file(),
                            entry.source_line());
    }
    return ::fmt::format_to(ctx, "{}", result);
  }
};
#    define AC_UTILS_STACKTRACE                                                \
      ::fmt::format("\n{}", ::std::stacktrace::current())
#  endif
#else
#  define AC_UTILS_STACKTRACE ("<no further information>")
#endif
#ifdef AC_UTILS_DEBUG_ENABLED
#  define AC_UTILS_DEBUG_LOGGING(_level_, _msg_, ...)                          \
    ::spdlog::_level_(_msg_ __VA_OPT__(, ) __VA_ARGS__);

namespace accat::auxilia::detail {
EXPORT_AUXILIA
struct _dbg_block_helper_struct_ {};
template <class Fun_> struct _dbg_block_ {
  inline constexpr _dbg_block_(Fun_ f) {
    (void)f();
    this->~_dbg_block_();
  }
  inline constexpr ~_dbg_block_() = default;
};
EXPORT_AUXILIA
template <class Fun_>
AC_STATIC_CALL_OPERATOR inline constexpr auto
operator*(_dbg_block_helper_struct_, Fun_ f_) noexcept(noexcept(f_()))
    -> _dbg_block_<Fun_> {
  return {f_};
}
} // namespace accat::auxilia::detail
#  define AC_UTILS_DEBUG_BLOCK                                                 \
    ::accat::auxilia::detail::_dbg_block_helper_struct_{}                      \
        *[&]() // NOLINT(bugprone-macro-parentheses)

#  define AC_UTILS_DEBUG_ONLY(...) __VA_ARGS__
/// @note detect if gtest was included, if so, emit a different message.
#  ifdef GTEST_API_

#    define AC_UTILS_DEBUG_LOGGING_SETUP(_exec_, _level_, _msg_, ...)          \
      ::spdlog::set_level(spdlog::level::_level_);                             \
      ::spdlog::set_pattern("[\033[33m" #_exec_ ":\033[0m %^%5l%$] %v");       \
      AC_UTILS_DEBUG_LOGGING(_level_,                                          \
                             "\033[33m" _msg_ " with gtest."                   \
                             "\033[0m" __VA_OPT__(, ) __VA_ARGS__)
#  else
#    define AC_UTILS_DEBUG_LOGGING_SETUP(_exec_, _level_, _msg_, ...)          \
      ::spdlog::set_level(spdlog::level::_level_);                             \
      ::spdlog::set_pattern("[%^%l%$] %v");                                    \
      AC_UTILS_DEBUG_LOGGING(_level_,                                          \
                             "\033[33m" _msg_ "."                              \
                             "\033[0m" __VA_OPT__(, ) __VA_ARGS__)
#  endif
#else
#  define AC_UTILS_DEBUG_LOGGING(...) (void)0;
#endif
/// @note magic_enum seems to require __PRETTY_FUNCTION__ to be defined
#if !defined(__PRETTY_FUNCTION__)
#  if defined(__FUNCSIG__)
#    define __PRETTY_FUNCTION__ __FUNCSIG__
#  else
#    define __PRETTY_FUNCTION__ __func__
#  endif
#endif
#ifdef __clang__
#  define AC_UTILS_FORCEINLINE [[clang::always_inline]]
#  define AC_UTILS_DEBUG_BREAK __builtin_debugtrap();
#  define AC_UTILS_DEBUG_FUNCTION_NAME __PRETTY_FUNCTION__
// Visual Studio's intellisense cannot recognize `elifdef` yet.
// Use the old, good `#elif` here.
#elif defined(__GNUC__)
#  define AC_UTILS_FORCEINLINE [[gnu::always_inline]]
#  define AC_UTILS_DEBUG_BREAK __builtin_trap();
#  define AC_UTILS_DEBUG_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define AC_UTILS_FORCEINLINE [[msvc::forceinline]]
#  define AC_UTILS_DEBUG_BREAK __debugbreak();
#  define AC_UTILS_DEBUG_FUNCTION_NAME __FUNCSIG__
#  pragma warning(disable : 4716) // must return a value
#  pragma warning(disable : 4530) // /EHsc
#else
#  include <csignal>
#  define AC_UTILS_FORCEINLINE inline
#  define AC_UTILS_DEBUG_BREAK raise(SIGTRAP);
#  define AC_UTILS_DEBUG_FUNCTION_NAME __func__
#endif
#ifdef AC_UTILS_DEBUG_ENABLED
#  define AC_UTILS_AMBIGUOUS_ELSE_BLOCKER                                      \
    switch (0)                                                                 \
    case 0:                                                                    \
    default:
#  define AC_UTILS_FILENAME (::std::source_location::current().file_name())
#  define AC_UTILS_FUNCTION_NAME AC_UTILS_DEBUG_FUNCTION_NAME
#  define AC_UTILS_LINE (::std::source_location::current().line())
#  define AC_UTILS_COLUMN (::std::source_location::current().column())
#  define AC_UTILS_RUNTIME_DEBUG_RAISE AC_UTILS_DEBUG_BREAK
#  define AC_UTILS_PRINT_ERROR_MSG_IMPL_SINGLE(x)                              \
    spdlog::critical("in file {0}, line {2} column {3},\n"                     \
                     "           function {1},\n"                              \
                     "           Constraints not satisfied:\n"                 \
                     "           Expect `{4}` to be true.\n"                   \
                     "Stacktrace:{5}",                                         \
                     AC_UTILS_FILENAME,                                        \
                     AC_UTILS_FUNCTION_NAME,                                   \
                     AC_UTILS_LINE,                                            \
                     AC_UTILS_COLUMN,                                          \
                     #x,                                                       \
                     AC_UTILS_STACKTRACE);
#  define AC_UTILS_PRINT_ERROR_MSG_IMPL_WITH_MSG(x, _msg_)                     \
    spdlog::critical("in file {0}, line {2} column {3},\n"                     \
                     "           function {1},\n"                              \
                     "           Constraints not satisfied:\n"                 \
                     "           Expect `{4}` to be true.\n"                   \
                     "Additional message: {5}\n"                               \
                     "Stacktrace:{6}",                                         \
                     AC_UTILS_FILENAME,                                        \
                     AC_UTILS_FUNCTION_NAME,                                   \
                     AC_UTILS_LINE,                                            \
                     AC_UTILS_COLUMN,                                          \
                     #x,                                                       \
                     (_msg_),                                                  \
                     AC_UTILS_STACKTRACE);
#  define AC_UTILS_PRINT_ERROR_MSG_IMPL_1(x)                                   \
    AC_UTILS_PRINT_ERROR_MSG_IMPL_SINGLE(x)
#  define AC_UTILS_PRINT_ERROR_MSG_IMPL_2(x, y)                                \
    AC_UTILS_PRINT_ERROR_MSG_IMPL_WITH_MSG(x, y)
#  define AC_UTILS_PRINT_ERROR_MSG(...)                                        \
    do {                                                                       \
      AC_UTILS_PRINT_ERROR_MSG_IMPL(__VA_ARGS__ __VA_OPT__(, ) 2,              \
                                    1)(__VA_ARGS__);                           \
      AC_UTILS_RUNTIME_DEBUG_RAISE                                             \
    } while (false);
#  define AC_UTILS_PRINT_ERROR_MSG_IMPL(_1, _2, N, ...)                        \
    AC_UTILS_PRINT_ERROR_MSG_IMPL_##N
#  define AC_UTILS_RUNTIME_REQUIRE_IMPL_SATISFY(x)                             \
    AC_UTILS_AMBIGUOUS_ELSE_BLOCKER                                            \
    if ((x))                                                                   \
      ;                                                                        \
    else {                                                                     \
      AC_UTILS_PRINT_ERROR_MSG(x)                                              \
    } //! <- do NOT add entraneous semicolon here, it'll confuse the
      //! preprocessor sometimes.
#  define AC_UTILS_RUNTIME_REQUIRE_IMPL_WITH_MSG(x, _msg_)                     \
    AC_UTILS_AMBIGUOUS_ELSE_BLOCKER                                            \
    if ((x))                                                                   \
      ;                                                                        \
    else {                                                                     \
      AC_UTILS_PRINT_ERROR_MSG(x, _msg_)                                       \
    }

#  define AC_UTILS_RUNTIME_REQUIRE_IMPL_1(_x_)                                 \
    AC_UTILS_RUNTIME_REQUIRE_IMPL_SATISFY(_x_)
#  define AC_UTILS_RUNTIME_REQUIRE_IMPL_2(_x_, _y_)                            \
    AC_UTILS_RUNTIME_REQUIRE_IMPL_WITH_MSG(_x_, _y_)
#  define AC_UTILS_RUNTIME_REQUIRE_IMPL(...)                                   \
    AC_UTILS_VFUNC(AC_UTILS_RUNTIME_REQUIRE_IMPL __VA_OPT__(, ) __VA_ARGS__)

#  ifdef AC_UTILS_USE_BOOST_CONTRACT
#    define AC_UTILS_RUNTIME_ASSERT(...)                                       \
      AC_UTILS_RUNTIME_REQUIRE_IMPL(__VA_ARGS__);
#    define AC_UTILS_PRECONDITION(...)                                         \
      AC_UTILS_VFUNC(AC_UTILS_PRECONDITION_IMPL __VA_OPT__(, ) __VA_ARGS__)
#    define AC_UTILS_POSTCONDITION(...)                                        \
      AC_UTILS_VFUNC(AC_UTILS_POSTCONDITION_IMPL __VA_OPT__(, ) __VA_ARGS__)
#  else
#    define AC_UTILS_RUNTIME_ASSERT(_arg_, ...)                                \
      AC_UTILS_RUNTIME_REQUIRE_IMPL(_arg_ __VA_OPT__(, ) __VA_ARGS__);
#    define AC_UTILS_PRECONDITION(...)                                         \
      AC_UTILS_RUNTIME_REQUIRE_IMPL(__VA_ARGS__)
#    define AC_UTILS_POSTCONDITION(...)                                        \
      AC_UTILS_RUNTIME_REQUIRE_IMPL(__VA_ARGS__)
#  endif
#  define AC_UTILS_NOEXCEPT_IF(...) // nothing
#  define AC_UTILS_NOEXCEPT         // nothing
#else
// if debug was turned off, do nothing.
#  define AC_UTILS_RUNTIME_ASSERT(...) (void)0;
#  define AC_UTILS_PRECONDITION(...) (void)0;
#  define AC_UTILS_POSTCONDITION(...) (void)0;
#  define AC_UTILS_DEBUG_LOGGING_SETUP(...) (void)0;
#  define AC_UTILS_DEBUG_BLOCK(...) (void)0;
#  define AC_UTILS_DEBUG_ONLY(...)
#  define AC_UTILS_NOEXCEPT_IF(...) noexcept(__VA_ARGS__)
#  define AC_UTILS_NOEXCEPT noexcept
#  define AC_UTILS_DBG_BREAK (void)0;
#endif
/// @def AC_SPDLOG_INITIALIZATION(_exec_, _log_level_) initializes the
/// spdlog framework
/// @note only call it once in the whole exec; never call it twice.
#define AC_SPDLOG_INITIALIZATION(_exec_, _log_level_)                          \
  [[maybe_unused]] /* AC_UTILS_API */                                          \
  static           /* <- msvc can't get through this.*/                        \
      const auto AC_SPDLOG_INITIALIZATION =                                    \
          [](void) /* static constexpr <- msvc can't get through this.*/       \
      -> ::std::nullptr_t {                                                    \
    AC_UTILS_DEBUG_LOGGING(info,                                               \
                           "\033[36mspdlog framework initialized.\033[0m")     \
    AC_UTILS_DEBUG_LOGGING_SETUP(_exec_, _log_level_, "Debug mode enabled")    \
    return nullptr;                                                            \
  }();
// ::std::cout << ::std::unitbuf;                                             \
// ::std::cerr << ::std::unitbuf;                                             \
// ::std::cout << ::std::flush;                                               \
// ::std::cerr << ::std::flush;                                               \
// clang-format off
#if (defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL) && !defined(__clang__)
/// @brief MSVC traditional preprocessor
/// @def dbg(_level_, _msg_, ...)
/// @note MSVC cross-platform compatible preprocessor acts like clang and gcc.
/// @see <a
/// href="https://learn.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview">MSVC
/// Preprocessor</a>
#  warning                                                                     \
      "MSVC traditional preprocessor was used. no additional debug info will be provided. to enable MSVC's new preprocessor, add compiler flag `/Zc:preprocessor`."
#  define dbg(_level_, _msg_, ...)                                             \
    AC_UTILS_DEBUG_LOGGING(_level_, _msg_ __VA_OPT__(, ) __VA_ARGS__)
#  define contract_assert(...)
#  define precondition(...)
#else
/// @def contract_assert(condition, message)
/// @note idea borrowed from c++2c's contract programming proposal. See
///  <a
///  href="https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2961r2.pdf">P2961R2</a>
#  define contract_assert(...) AC_UTILS_RUNTIME_ASSERT(__VA_ARGS__)
#  define precondition(...) AC_UTILS_PRECONDITION(__VA_ARGS__)
// clang-format on
#  define dbg(...) AC_UTILS_DEBUG_LOGGING(__VA_ARGS__)
#endif
#define dbg_block AC_UTILS_DEBUG_BLOCK
#define dbg_only(...) AC_UTILS_DEBUG_ONLY(__VA_ARGS__)
#define dbg_break AC_UTILS_DEBUG_BREAK

#ifdef AC_UTILS_DEBUG_ENABLED
#  define AC_UTILS_TODO_(...)                                                  \
    AC_UTILS_RUNTIME_ASSERT(false, "Not implemented: " #__VA_ARGS__);
// if exception was disabled, do nothing.
#elif defined(__cpp_exceptions) && __cpp_exceptions
#  include <stdexcept>
#  define AC_UTILS_TODO_(...)                                                  \
    throw ::std::logic_error(::std::format("TODO: " #__VA_ARGS__));
#elif __has_include(<spdlog/spdlog.h>)
#  define AC_UTILS_TODO_(...)                                                  \
    AC_UTILS_DEBUG_LOGGING(critical, "TODO: " #__VA_ARGS__);
#else
#  include <iostream>
#  define AC_UTILS_TODO_(...)                                                  \
    ::std::cerr << std::format("TODO: " #__VA_ARGS__) << ::std::endl;          \
    AC_UTILS_DEBUG_BREAK
#endif
/// @def TODO mimic from kotlin's `TODO` function, which throws an
/// exception and is also discoverable by IDE.
#define TODO(...) AC_UTILS_TODO_(__VA_ARGS__)

#if defined(_MSC_VER) && !defined(__clang__)
/// @remark currenty, MSVC's constexpr was really disgusting.
#  define AC_CONSTEXPR20
#  pragma warning(disable : 4244) // conversion from 'int' to 'char' warning
#else
#  define AC_CONSTEXPR20 constexpr
#endif

#ifdef _WIN32
/// @def AC_NOVTABLE
/// @note msvc-specific keyword to prevent vtable generation.
///     Used in(and should only in) interface classes.
#  define AC_NOVTABLE __declspec(novtable)
#  define AC_INTERFACE __interface
#else
#  define AC_NOVTABLE
#  define AC_INTERFACE class
#endif

namespace accat::auxilia::detail {
/// @see
/// https://stackoverflow.com/questions/32432450/what-is-standard-defer-finalizer-implementation-in-c
EXPORT_AUXILIA
struct _utils_defer_helper_struct_ {};
template <class Fun_> struct _utils_deferrer_ {
  Fun_ f_;
  inline constexpr _utils_deferrer_(Fun_ f) : f_(f) {}
  inline constexpr ~_utils_deferrer_() { f_(); }
};
EXPORT_AUXILIA
template <class Fun_>
AC_STATIC_CALL_OPERATOR inline AC_CONSTEXPR20 auto
operator*(_utils_defer_helper_struct_, Fun_ f_) -> _utils_deferrer_<Fun_> {
  return {f_};
}
} // namespace accat::auxilia::detail
#define AC_DEFER                                                               \
  const auto AC_UTILS_EXPAND_COUNTER(_utils_defer_block_at) =                  \
      ::accat::auxilia::detail::_utils_defer_helper_struct_{} *[&]()
#ifdef defer
#  warning "defer was already defined. please check the code."
#  pragma pop_macro("defer")
#endif
#define defer AC_DEFER
#define postcondition AC_DEFER

// NOLINTBEGIN(bugprone-macro-parentheses)
/// @def AC_BITMASK_OPS
/// @brief define the basic bitmask operations for the given bitmask
/// type. the bitmask type should be an enum class.
/// @param _bitmask_ the bitmask type
/// @note this macro was borrowed from Microsoft's STL implementation.
#define AC_BITMASK_OPS(_bitmask_)                                              \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ operator&(                                        \
      _bitmask_ _left_,                                                        \
      _bitmask_ _right_) noexcept { /* return _left_ & _right_ */              \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(static_cast<_intergral_type_>(_left_) &     \
                                   static_cast<_intergral_type_>(_right_)));   \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ operator|(                                        \
      _bitmask_ _left_,                                                        \
      _bitmask_ _right_) noexcept { /* return _left_ | _right_ */              \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(static_cast<_intergral_type_>(_left_) |     \
                                   static_cast<_intergral_type_>(_right_)));   \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ operator^(                                        \
      _bitmask_ _left_,                                                        \
      _bitmask_ _right_) noexcept { /* return _left_ ^ _right_ */              \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(static_cast<_intergral_type_>(_left_) ^     \
                                   static_cast<_intergral_type_>(_right_)));   \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ &operator&=(                                      \
      _bitmask_ &_left_,                                                       \
      _bitmask_ _right_) noexcept { /* return _left_ &= _right_ */             \
    return (_left_ = _left_ & _right_);                                        \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ &operator|=(                                      \
      _bitmask_ &_left_,                                                       \
      _bitmask_ _right_) noexcept { /* return _left_ |= _right_ */             \
    return (_left_ = _left_ | _right_);                                        \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ &operator^=(                                      \
      _bitmask_ &_left_,                                                       \
      _bitmask_ _right_) noexcept { /* return _left_ ^= _right_ */             \
    return (_left_ = _left_ ^ _right_);                                        \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr _bitmask_ operator~(                                        \
      _bitmask_ _left_) noexcept { /* return ~_left_ */                        \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(~static_cast<_intergral_type_>(_left_)));   \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr bool operator==(                                            \
      _bitmask_ _value_,                                                       \
      std::underlying_type_t<_bitmask_>                                        \
          _zero_) noexcept { /* return _left_ == _right_ */                    \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return static_cast<_intergral_type_>(_value_) == _zero_;                   \
  }                                                                            \
  [[nodiscard]]                                                                \
  inline constexpr bool operator!(                                             \
      _bitmask_ _left_) noexcept { /* return !_left_ */                        \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return !static_cast<_intergral_type_>(_left_);                             \
  }

/// @def AC_BITMASK_OPS_NESTED
/// @brief Useful if the enum class is nested inside a template class(usually
/// you won't do this).
#define AC_BITMASK_OPS_NESTED(_bitmask_)                                       \
  [[nodiscard]] friend inline constexpr _bitmask_ operator&(                   \
      _bitmask_ _left_,                                                        \
      _bitmask_ _right_) noexcept { /* return _left_ & _right_ */              \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(static_cast<_intergral_type_>(_left_) &     \
                                   static_cast<_intergral_type_>(_right_)));   \
  }                                                                            \
  [[nodiscard]] friend inline constexpr _bitmask_ operator|(                   \
      _bitmask_ _left_,                                                        \
      _bitmask_ _right_) noexcept { /* return _left_ | _right_ */              \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(static_cast<_intergral_type_>(_left_) |     \
                                   static_cast<_intergral_type_>(_right_)));   \
  }                                                                            \
  [[nodiscard]] friend inline constexpr _bitmask_ operator^(                   \
      _bitmask_ _left_,                                                        \
      _bitmask_ _right_) noexcept { /* return _left_ ^ _right_ */              \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(static_cast<_intergral_type_>(_left_) ^     \
                                   static_cast<_intergral_type_>(_right_)));   \
  }                                                                            \
  [[nodiscard]] friend inline constexpr _bitmask_ &operator&=(                 \
      _bitmask_ &_left_,                                                       \
      _bitmask_ _right_) noexcept { /* return _left_ &= _right_ */             \
    return (_left_ = _left_ & _right_);                                        \
  }                                                                            \
  [[nodiscard]] friend inline constexpr _bitmask_ &operator|=(                 \
      _bitmask_ &_left_,                                                       \
      _bitmask_ _right_) noexcept { /* return _left_ |= _right_ */             \
    return (_left_ = _left_ | _right_);                                        \
  }                                                                            \
  [[nodiscard]] friend inline constexpr _bitmask_ &operator^=(                 \
      _bitmask_ &_left_,                                                       \
      _bitmask_ _right_) noexcept { /* return _left_ ^= _right_ */             \
    return (_left_ = _left_ ^ _right_);                                        \
  }                                                                            \
  [[nodiscard]] friend inline constexpr _bitmask_ operator~(                   \
      _bitmask_ _left_) noexcept { /* return ~_left_ */                        \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return (static_cast<_bitmask_>(~static_cast<_intergral_type_>(_left_)));   \
  }                                                                            \
  [[nodiscard]]                                                                \
  friend inline constexpr bool operator==(                                     \
      _bitmask_ _value_,                                                       \
      std::underlying_type_t<_bitmask_>                                        \
          _zero_) noexcept { /* return _left_ == _right_ */                    \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return static_cast<_intergral_type_>(_value_) == _zero_;                   \
  }                                                                            \
                                                                               \
  [[nodiscard]]                                                                \
  friend inline constexpr bool operator!(                                      \
      _bitmask_ _left_) noexcept { /* return !_left_ */                        \
    using _intergral_type_ = ::std::underlying_type_t<_bitmask_>;              \
    return !static_cast<_intergral_type_>(_left_);                             \
  }                                                                            \
// NOLINTEND(bugprone-macro-parentheses)
