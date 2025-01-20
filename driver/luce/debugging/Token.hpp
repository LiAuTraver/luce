#pragma once
#include <fmt/format.h>
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

namespace accat::luce {
using namespace std::literals;
struct Token : auxilia::Printable<Token> {
  enum class Type : uint8_t {
    // clang-format off
    kMonostate = 0,
    // Single-character tokens.
    kLeftParen, kRightParen, kLeftBrace, kRightBrace, kComma, 
    kDot, kMinus, kPlus, kSemicolon, kSlash, kAmpersand, kAsterisk,
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
      : type_(type), lexeme_(lexeme), line_(line) {}
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
  ~Token() noexcept {
    if (type_ == Type::kLexError)
      error_message_.~string_type();
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
    return error_message_;
  }
  Type type() const noexcept {
    return type_;
  }
  uint_least32_t line() const noexcept {
    return line_;
  }
  auto to_string(const auxilia::FormatPolicy &format_policy =
                     auxilia::FormatPolicy::kDefault) const -> std::string {
    auto name = magic_enum::enum_name(type_);
    auto str = string_type{};
    if (format_policy == auxilia::FormatPolicy::kDefault) {
      str = fmt::format(
          "type: {}, {}, line: {}", name, _format_lexeme(format_policy), line_);
    } else if (format_policy == auxilia::FormatPolicy::kTokenOnly) {
      str = _format_lexeme(format_policy);
    } else {
      dbg_break
      return "not implemented";
    }
    return str;
  }

protected:
  Token(Type type, std::string &&error_message, uint_least32_t line)
      : type_(type), error_message_(std::move(error_message)), line_(line) {}
  Token(Type type, long double number, uint_least32_t line)
      : type_(type), number_(number), line_(line) {}

public:
  static auto Number(const long double value,
                     const uint_least32_t line) noexcept {
    return Token{Type::kNumber, value, line};
  }
  static auto Lexeme(const Type type,
                     const std::string_view lexeme,
                     const uint_least32_t line) noexcept {
    return Token{type, lexeme, line};
  }
  static auto Error(std::string &&message, const uint_least32_t line) noexcept {
    return Token{Type::kLexError, std::move(message), line};
  }
  static auto eof(const uint_least32_t line) noexcept {
    return Token{Type::kEndOfFile, ""sv, line};
  }

private:
  Type type_ = Type::kMonostate;
  union {
    std::monostate monostate_{};
    std::string_view lexeme_;
    long double number_;
    string_type error_message_;
  };
  uint_least32_t line_ = std::numeric_limits<uint_least32_t>::signaling_NaN();

private:
  Token &_do_move(Token &&that) noexcept {
    // Destroy old active member
    switch (type_) {
    case Type::kLexError: // non-trivially destructible
      error_message_.~string_type();
      break;
    case Type::kNumber: // trivially destructible, no need to destroy explicitly
      [[fallthrough]];
    case Type::kMonostate: // ditto
      [[fallthrough]];
    default:
      break;
    }

    type_ = that.type_;
    line_ = that.line_;

    switch (that.type_) {
    case Type::kLexError: // horrible! :(
      new (std::addressof(error_message_))
          string_type(std::move(that.error_message_));
      break;
    case Type::kNumber:
      number_ = that.number_;
      break;
    case Type::kMonostate:
      [[fallthrough]];
    default:
      break;
    }

    that.type_ = Type::kMonostate;
    return *this;
  }
  auto _format_lexeme(const auxilia::FormatPolicy format_policy) const
      -> string_type {
    auto str = string_type{};
    if (format_policy == auxilia::FormatPolicy::kDefault) {
      if (type_ == Type::kNumber)
        str = fmt::format("number: '{}'", number_);
      else if (type_ == Type::kLexError)
        str = fmt::format("error: '{}'", error_message_);
      else if (type_ == Type::kMonostate)
        str = "monostate"s;
      else
        str = fmt::format("lexeme: '{}'", lexeme_);
    } else if (format_policy == auxilia::FormatPolicy::kTokenOnly) {
      if (type_ == Type::kNumber)
        str = fmt::format("{}", number_);
      else if (type_ == Type::kLexError)
        str = fmt::format("{}", error_message_);
      else if (type_ == Type::kMonostate)
        str = "monostate"s;
      else
        str = fmt::format("{}", lexeme_);
    }
    return str;
  }
};
} // namespace accat::luce
