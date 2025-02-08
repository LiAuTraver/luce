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
  Token(const Type type,
        const std::string_view lexeme,
        const uint_least32_t line)
      : type_(type), lexeme_(std::string{lexeme}), line_(line) {}
  Token(const Token &) = delete;
  Token &operator=(const Token &) = delete;
  Token(Token &&that) noexcept {
    _do_move(std::move(that));
  }
  Token &operator=(Token &&that) noexcept {
    if (this == &that)
      return *this;
    return _do_move(std::move(that)); // NOLINT(*-unconventional-assign-operator)
  }
  AC_CONSTEXPR20 ~Token() noexcept {
    if (type_ != Type::kNumber && type_ != Type::kMonostate)
      lexeme_.~string_type();
  }

public:
  auto lexeme() const noexcept -> std::string_view {
    precondition(type_ != Type::kNumber && type_ != Type::kLexError,
                 "lexeme() called on a non-lexeme token")
    return lexeme_;
  }
  // Changed number() to return a variant instead of long double.
  auto number() const noexcept {
    precondition(type_ == Type::kNumber,
                 "number() called on a non-number token");
    using NumberType = std::variant<long long, long double>;
    return number_is_integer_ ? NumberType{num_ll_} : NumberType{num_ld_};
  }
  auto error_message() const noexcept {
    precondition(type_ == Type::kLexError,
                 "error_message() called on a non-error token")
    return lexeme_;
  }
  constexpr auto type() const noexcept {
    return type_;
  }
  constexpr auto is_type(const Type type) const noexcept {
    return type_ == type;
  }
  constexpr auto line() const noexcept {
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
  Token(const Type type, std::string &&error_message, const uint_least32_t line)
      : type_(type), lexeme_(std::move(error_message)), line_(line) {}
  Token(const Type type, const long double number, const uint_least32_t line)
      : type_(type), num_ld_(number), number_is_integer_(false), line_(line) {}
  Token(const Type type, const long long number, const uint_least32_t line)
      : type_(type), num_ll_(number), number_is_integer_(true), line_(line) {}

public:
  static AC_CONSTEXPR20 auto
  Number(const std::variant<long long, long double> value,
         const uint_least32_t line) noexcept {
    return std::visit(
        [line](auto &&v) { return Token{Type::kNumber, v, line}; }, value);
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
      if (number_is_integer_ && that.number_is_integer_)
        return num_ll_ <=> that.num_ll_; // strong ordering convert to partial
      return num_ld_ <=> that.num_ld_;
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
    long double num_ld_;
    long long num_ll_;
  };
  bool number_is_integer_ = false;
  uint_least32_t line_ = std::numeric_limits<uint_least32_t>::signaling_NaN();

private:
  Token &_do_move(Token &&that) noexcept {
    // Destroy old active member
    if (type_ != Type::kNumber && type_ != Type::kMonostate)
      lexeme_.~string_type();

    type_ = that.type_;
    line_ = that.line_;

    if (that.type_ == Type::kNumber) {
      if (that.number_is_integer_) {
        num_ll_ = that.num_ll_;
        number_is_integer_ = true;
      } else {
        num_ld_ = that.num_ld_;
        number_is_integer_ = false;
      }
    } else {
      ::new (std::addressof(lexeme_)) std::string(std::move(that.lexeme_));
    }

    that.type_ = Type::kMonostate;
    return *this;
  }
  auto _do_format(const auxilia::FormatPolicy format_policy) const
      -> string_type {
    auto str = string_type{};
    auto format_number = [this]() -> long double {
      auto n = number();
      return std::holds_alternative<long long>(n)
                 ? static_cast<long double>(std::get<long long>(n))
                 : std::get<long double>(n);
    };
    if (format_policy == auxilia::FormatPolicy::kBrief) {
      if (type_ == Type::kNumber)
        str = fmt::format("{}", format_number());
      else if (type_ == Type::kLexError)
        str = lexeme_;
      else if (type_ == Type::kMonostate)
        str = "monostate"s;
      else
        str = magic_enum::enum_name(type_);
    } else {
      if (type_ == Type::kNumber)
        str = fmt::format("number: '{}'", format_number());
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
