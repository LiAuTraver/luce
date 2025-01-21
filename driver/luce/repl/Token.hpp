#pragma once
#include <fmt/format.h>
#include <compare>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include <unordered_map>
#include <magic_enum/magic_enum.hpp>
#include "accat/auxilia/details/format.hpp"

namespace accat::luce::repl {
using namespace std::literals;
struct Token : auxilia::Printable<Token> {
  enum class Type : uint8_t {
    // clang-format off
    kMonostate = 0,
    // Single-character tokens.
    kLeftParen, kRightParen, kLeftBrace, kRightBrace, kComma, 
    kDot, kMinus, kPlus, kSemicolon, kSlash, kAmpersand, kStar,
    // One or two character tokens.
    kBang, kBangEqual, kEqual, kEqualEqual, 
    kGreater, kGreaterEqual, kLess, kLessEqual,
    // Literals.
    kIdentifier, kString, kNumber,
    // Keywords.
    kAnd, kClass, kElse, kFalse, kFun, kFor, kIf, kNil, 
    kOr, kPrint, kReturn, kSuper, kThis, kTrue, kVar, kWhile,
    // lex error.
    kLexError,
    // end of file.
    kEndOfFile,
    // clang-format on
  };
  Token() = default;
  Token(Type type, std::string_view lexeme, uint_least32_t line)
      : type_(type), lexeme_(std::string{lexeme}), line_(line) {}
  Token(const Token &) = delete;
  Token &operator=(const Token &) = delete;
  Token(Token &&that) noexcept {
    _do_move(std::move(that));
  }
  Token &operator=(Token &&that) noexcept {
    if (this == &that)
      return *this;
    return _do_move(std::move(that));
  }
  AC_CONSTEXPR20 ~Token() noexcept {
    if (type_ != Type::kNumber && type_ != Type::kMonostate)
      lexeme_.~string_type();
  }

public:
  std::string_view lexeme() const noexcept {
    precondition(type_ != Type::kNumber && type_ != Type::kLexError,
                 "lexeme() called on a non-lexeme token")
    return lexeme_;
  }
  long double number() const noexcept {
    precondition(type_ == Type::kNumber,
                 "number() called on a non-number token")
    return number_;
  }
  string_type error_message() const noexcept {
    precondition(type_ == Type::kLexError,
                 "error_message() called on a non-error token")
    return lexeme_;
  }
  constexpr Type type() const noexcept {
    return type_;
  }
  constexpr bool is_type(const Type type) const noexcept {
    return type_ == type;
  }
  constexpr uint_least32_t line() const noexcept {
    return line_;
  }
  auto to_string(const auxilia::FormatPolicy &format_policy =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    auto str = string_type{};
    if (format_policy == auxilia::FormatPolicy::kBrief) {
      str = _do_format(format_policy);
    } else {
      auto name = magic_enum::enum_name(type_);
      str = fmt::format(
          "type: {}, {}, line: {}", name, _do_format(format_policy), line_);
    }

    return str;
  }

protected:
  Token(Type type, std::string &&error_message, uint_least32_t line)
      : type_(type), lexeme_(std::move(error_message)), line_(line) {}
  Token(Type type, long double number, uint_least32_t line)
      : type_(type), number_(number), line_(line) {}

public:
  static AC_CONSTEXPR20 auto Number(const long double value,
                                    const uint_least32_t line) noexcept {
    return Token{Type::kNumber, value, line};
  }
  static AC_CONSTEXPR20 auto Lexeme(const Type type,
                                    const std::string_view lexeme,
                                    const uint_least32_t line) noexcept {
    return Token{type, lexeme, line};
  }
  static AC_CONSTEXPR20 auto Error(std::string &&message,
                                   const uint_least32_t line) noexcept {
    return Token{Type::kLexError, std::move(message), line};
  }
  static auto eof(const uint_least32_t line) noexcept {
    return Token{Type::kEndOfFile, ""sv, line};
  }

public:
  auto operator<=>(const Token &that) const
      noexcept(noexcept(lexeme_ <=> that.lexeme_)) -> std::partial_ordering {
    if (this == std::addressof(that))
      return std::partial_ordering::equivalent;
    if (type_ != that.type_)
      return type_ <=> that.type_;
    switch (type_) {
    case Type::kLexError:
      return std::partial_ordering::unordered;
    case Type::kNumber:
      return number_ <=> that.number_;
    case Type::kMonostate:
      return std::partial_ordering::equivalent;
    default:
      return lexeme_ <=> that.lexeme_;
    }
  }
  auto operator!=(const Token &that) const
      noexcept(noexcept(this->operator<=>(that))) -> bool {
    return *this <=> that != std::partial_ordering::equivalent;
  }

private:
  Type type_ = Type::kMonostate;
  union {
    std::monostate monostate_{};
    string_type lexeme_;
    long double number_;
  };
  uint_least32_t line_ = std::numeric_limits<uint_least32_t>::signaling_NaN();

private:
  Token &_do_move(Token &&that) noexcept {
    // Destroy old active member
    if (type_ != Type::kNumber && type_ != Type::kMonostate)
      lexeme_.~string_type();

    type_ = that.type_;
    line_ = that.line_;

    switch (that.type_) {
    case Type::kNumber:
      number_ = that.number_;
      break;
    case Type::kMonostate:
      break;
    default:
      ::new (std::addressof(lexeme_)) std::string(std::move(that.lexeme_));
      break;
    }

    that.type_ = Type::kMonostate;
    return *this;
  }
  auto _do_format(const auxilia::FormatPolicy format_policy) const
      -> string_type {
    auto str = string_type{};
    if (format_policy == auxilia::FormatPolicy::kBrief) {
      if (type_ == Type::kNumber)
        str = fmt::format("{}", number_);
      else if (type_ == Type::kLexError)
        str = lexeme_;
      else if (type_ == Type::kMonostate)
        str = "monostate"s;
      else
        str = magic_enum::enum_name(type_);
    } else {
      if (type_ == Type::kNumber)
        str = fmt::format("number: '{}'", number_);
      else if (type_ == Type::kLexError)
        str = fmt::format("error: '{}'", lexeme_);
      else if (type_ == Type::kMonostate)
        str = "monostate"s;
      else
        str = fmt::format("lexeme: '{}'", magic_enum::enum_name(type_));
    }
    return str;
  }
} inline AC_CONSTEXPR20 nulltok{};
} // namespace accat::luce::repl
