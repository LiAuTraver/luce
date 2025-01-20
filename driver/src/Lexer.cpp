#include <algorithm>
#include <charconv>
#include <locale>
#include <utility>
#include "deps.hh"

#include "luce/debugging/Lexer.hpp"

namespace accat::luce {
using auxilia::operator""s;
using auxilia::operator""sv;
inline static constexpr auto tolerable_chars = "_`$"sv;
// inline static constexpr auto conditional_tolerable_chars = "@$#"sv;
inline static constexpr auto whitespace_chars = " \t\r"sv;
inline static constexpr auto newline_chars = "\n\v\f"sv;
inline static const auto keywords =
    std::unordered_map<std::string_view, Token::Type>{
        {"and"sv, {Token::Type::kAnd}},
        {"class"sv, {Token::Type::kClass}},
        {"else"sv, {Token::Type::kElse}},
        {"false"sv, {Token::Type::kFalse}},
        {"for"sv, {Token::Type::kFor}},
        {"fun"sv, {Token::Type::kFun}},
        {"if"sv, {Token::Type::kIf}},
        {"nil"sv, {Token::Type::kNil}},
        {"or"sv, {Token::Type::kOr}},
        {"print"sv, {Token::Type::kPrint}},
        {"return"sv, {Token::Type::kReturn}},
        {"super"sv, {Token::Type::kSuper}},
        {"this"sv, {Token::Type::kThis}},
        {"true"sv, {Token::Type::kTrue}},
        {"var"sv, {Token::Type::kVar}},
        {"while"sv, {Token::Type::kWhile}},
    };
template <typename Predicate>
bool Lexer::advance_if(Predicate &&predicate)
  requires std::invocable<Predicate, char_t> &&
           std::convertible_to<Predicate, bool>
{
  if (is_at_end() || !predicate(contents[cursor]))
    return false;
  cursor++;
  return true;
}
Lexer::Lexer(Lexer &&other) noexcept
    : head(std::exchange(other.head, 0)),
      cursor(std::exchange(other.cursor, 0)),
      contents(std::move(const_cast<string_type &>(other.contents))),
      // lexeme_views(std::move(other.lexeme_views)),
      current_line(std::exchange(other.current_line, 1)),
      // tokens(std::move(other.tokens)),
      error_count(std::exchange(other.error_count, 0)) {}
Lexer &Lexer::operator=(Lexer &&other) noexcept {
  if (this == &other)
    return *this;
  head = std::exchange(other.head, 0);
  cursor = std::exchange(other.cursor, 0);
  const_cast<string_type &>(contents) =
      std::move(const_cast<string_type &>(other.contents));
  // lexeme_views = std::move(other.lexeme_views);
  current_line = std::exchange(other.current_line, 1);
  // tokens = std::move(other.tokens);
  error_count = std::exchange(other.error_count, 0);

  return *this;
}
template <typename Num>
  requires std::is_arithmetic_v<Num>
auto Lexer::to_number(string_view_type value) -> std::optional<Num> {
  Num number;
  const auto &[p, ec] =
      std::from_chars(value.data(), value.data() + value.size(), number);
  if (ec == std::errc())
    return {number};
  dbg(error, "Unable to convert string to number: {}", value)
  dbg(error, "Error code: {}", std::to_underlying(ec))
  dbg(error, "Error position: {}", p)
  return {};
}
Lexer::status_t Lexer::load(const path_type &filepath) const {
  if (not contents.empty())
    return auxilia::AlreadyExistsError("File already loaded");
  if (not std::filesystem::exists(filepath))
    return auxilia::NotFoundError("File does not exist: " + filepath.string());
  std::ifstream file(filepath);
  if (not file)
    return {};

  std::ostringstream buffer;
  buffer << file.rdbuf();

  const_cast<string_type &>(contents) = buffer.str();
  return {};
}
Lexer::status_t Lexer::load(string_type &&str) {
  if (not contents.empty())
    return auxilia::AlreadyExistsError("Content already loaded");
  const_cast<string_type &>(contents) = std::move(str);
  // tokens.clear();
  // lexeme_views.clear();
  return {};
}

auto Lexer::lex() -> auxilia::Generator<Lexer::token_t, uint_least32_t> {
  while (not is_at_end()) {
    head = cursor;
    if (auto token = next_token(); token.type() != kMonostate) {
      co_yield token;
    }
    // else, skip the token and continue
    // continue;
  }
  co_yield add_token(kEndOfFile);
  co_return error_count;
}
Lexer::token_t Lexer::add_identifier_and_keyword() {
  auto value = lex_identifier();
  auto it = keywords.find(value);
  if (it == keywords.end()) {
    // a normal identifier
    return add_token(kIdentifier);
  }
  switch (it->second) {
  case kTrue:
    return add_token(kTrue);
    break;
  case kFalse:
    return add_token(kFalse);
    break;
  default:
    dbg(trace, "keyword: {}", value)
    return add_token(it->second);
  }
  std::unreachable();
}
Lexer::token_t Lexer::add_number() {
  if (auto value = lex_number(false)) {
    return add_token(*value);
  }
  // dbg(error, "invalid number.")
  // return {};
  return add_error_token("Invalid number: "s + contents.substr(head, cursor - head));
}
Lexer::token_t Lexer::add_string() {
  // hard to do...
  auto status = lex_string();
  auto value = string_view_type(contents.data() + head + 1, cursor - head - 2);
  if (status != auxilia::Status::kOk) {
    return add_error_token("Unterminated string: "s + value.data());
  }
  dbg(trace, "string value: {}", value)
  return add_token(kString);
}
Lexer::token_t Lexer::add_comment() {
  while (peek() != '\n' && !is_at_end())
    get();
  return {};
}
Lexer::token_t Lexer::next_token() {
  // token1 token2
  // 			 ^ cursor position
  precondition(cursor < contents.size(), "cursor out of bounds")

  // we use `monostate` to indicate that the token is a
  switch (auto c = get()) {
  case '(':
    return add_token(kLeftParen);
  case ')':
    return add_token(kRightParen);
  case '{':
    return add_token(kLeftBrace);
  case '}':
    return add_token(kRightBrace);
  case ',':
    return add_token(kComma);
  case '.':
    return add_token(kDot);
  case '-':
    return add_token(kMinus);
  case '+':
    return add_token(kPlus);
  case ';':
    return add_token(kSemicolon);
  case '*':
    return add_token(kAsterisk);
  case '&':
    return add_token(kAmpersand);
  case '!':
    return add_token(advance_if_is('=') ? kBangEqual : kBang);
  case '=':
    return add_token(advance_if_is('=') ? kEqualEqual : kEqual);
  case '<':
    return add_token(advance_if_is('=') ? kLessEqual : kLess);
  case '>':
    return add_token(advance_if_is('=') ? kGreaterEqual : kGreater);
  case '/':
    return advance_if_is('/') ? add_comment() : add_token(kSlash);
  default:
    if (whitespace_chars.contains(c))
      return {}; // continue
    if (newline_chars.contains(c)) {
      current_line++;
      return {}; // continue
    }
    if (c == '"') {
      return add_string();
    }
    // first, numbers(order matters!)
    if (std::isdigit(c, std::locale())) {
      return add_number();
    }
    // finally, letters
    if (std::isalpha(c, std::locale()) or tolerable_chars.contains(c)) {
      return add_identifier_and_keyword();
    }
    return add_error_token("Unexpected character: "s + c);
  }
  std::unreachable();
}
Lexer::char_t Lexer::peek(const size_t offset) const {
  if (is_at_end(offset))
    return 0; // equivalent to '\0'
  return contents[cursor + offset];
}
const Lexer::char_t &Lexer::get(const size_t offset) {
  precondition(cursor < contents.size(), "cursor out of bounds")
  postcondition(cursor <= contents.size(), "cursor out of bounds")
  auto &c = contents[cursor];
  cursor += offset;
  return c;
}
bool Lexer::advance_if_is(const char_t expected) {
  if (is_at_end() || contents[cursor] != expected)
    return false;
  cursor++;
  return true;
}
bool Lexer::is_at_end(const size_t offset) const {
  return cursor + offset >= contents.size();
}
Lexer::token_t Lexer::add_token(Token::Type type) {
  if (type == kEndOfFile) { // FIXME: lexeme bug at EOF(not critical)
                            // tokens.emplace_back(type, "", current_line);
                            // return;
    return token_t::eof(current_line);
  }
  auto lexeme = string_view_type(contents.data() + head, cursor - head);
  dbg(trace, "lexeme: {}", lexeme)
  // tokens.emplace_back(type, lexeme, current_line);
  // lexeme_views.emplace_back(lexeme);
  return token_t::Lexeme(type, lexeme, current_line);
}
Lexer::token_t Lexer::add_token(long double number) const {
  dbg(trace, "lexeme: {}", number)
  return token_t::Number(number, current_line);
  // tokens.emplace_back(type, literal, current_line);
  // lexeme_views.emplace_back(std::to_string(literal));
}
Lexer::token_t Lexer::add_error_token(string_type&& msg) {
  error_count++;
  // return {kLexError, msg, current_line};
  return token_t::Error(std::move(msg), current_line);
}
Lexer::status_t::Code Lexer::lex_string() {
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n')
      current_line++; // multiline string, of course we dont want act like C/C++
                      // which will result in a compile error if the string is
                      // not closed at the same current_line.
    get();
  }
  if (is_at_end() && peek() != '"') {
    dbg(error, "Unterminated string.")
    return status_t::kInvalidArgument;
  }
  // "i am a string..."
  // 						      ^ cursor position
  else
    get(); // consume the closing quote.
  return status_t::kOk;
}
auto Lexer::lex_number(const bool is_negative) -> std::optional<long double> {
  while (std::isdigit(peek(), std::locale())) {
    get();
  }
  bool is_floating_point = false;
  // maybe a '.'?
  if (peek() == '.' && std::isdigit(peek(1), std::locale())) {
    get(); // consume the '.'
    while (std::isdigit(peek(), std::locale())) {
      get();
    }
    // 123.456_
    // 		    ^ cursor position
    is_floating_point = true;
  }
  // 789_
  //    ^ cursor position
  auto value = contents.substr(head, cursor - head);
  (void)is_floating_point;

  (void)is_negative;

  return to_number<long double>(value);
}
// auto Lexer::get_tokens() -> tokens_t & {
//   return tokens;
// }
bool Lexer::ok() const noexcept {
  return !error_count;
}
uint_least32_t Lexer::error() const noexcept {
  return error_count;
}
Lexer::string_view_type Lexer::lex_identifier() {
  while (std::isalnum(peek(), std::locale()) ||
         tolerable_chars.find(peek()) != string_view_type::npos) {
    get();
  }
  // 123_abc
  //       ^ cursor position
  return {contents.data() + head, cursor - head};
}
} // namespace accat::luce
