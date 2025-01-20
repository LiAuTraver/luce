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
#include "luce/debugging/Token.hpp"
#include <magic_enum/magic_enum.hpp>
#include "accat/auxilia/details/config.hpp"
#include "accat/auxilia/details/format.hpp"

namespace accat::luce {
class Lexer {
public:
  using size_type = typename auxilia::string::size_type;
  using string_type = auxilia::string;
  using string_view_type = auxilia::string_view;
  using path_type = auxilia::path;
  using status_t = auxilia::Status;
  using token_t = Token;
  using token_type_t = Token::Type;
  using tokens_t = std::vector<token_t>;
  using lexeme_views_t = std::vector<string_view_type>;
  using char_t = typename string_type::value_type;
  using enum token_type_t;

public:
  Lexer() = default;
  Lexer(const Lexer &other) = delete;
  Lexer(Lexer &&other) noexcept;
  Lexer &operator=(const Lexer &other) = delete;
  Lexer &operator=(Lexer &&other) noexcept;
  ~Lexer() = default;

public:
  /// @brief load the contents of the file
  /// @return OkStatus() if successful, NotFoundError() otherwise
  status_t load(const path_type &) const;
  /// @copydoc load(const path_type &)
  status_t load(string_type &&);
  /// @brief lex the contents of the file
  /// @return OkStatus() if successful, NotFoundError() otherwise
  auto lex() -> auxilia::Generator<token_t, uint_least32_t>;
  auto get_tokens() -> tokens_t &;
  bool ok() const noexcept;
  uint_least32_t error() const noexcept;

private:
  token_t add_identifier_and_keyword();
  token_t add_number();
  token_t add_string();
  token_t add_comment();
  token_t next_token();
  token_t add_token(token_type_t);
  token_t add_token(long double) const;
  token_t add_error_token(string_type&&);
  bool is_at_end(size_t = 0) const;
  auto lex_string() -> Lexer::status_t::Code;
  auto lex_identifier() -> string_view_type;
  auto lex_number(bool) -> std::optional<long double>;

private:
  /// @brief convert a string to a number
  /// @tparam Num the number type
  /// @param value the string to convert
  /// @return the number if successful
  template <typename Num>
    requires std::is_arithmetic_v<Num>
  auto to_number(string_view_type value) -> std::optional<Num>;

private:
  /// @brief lookaheads; we have only consumed the character before the cursor
  char_t peek(size_t offset = 0) const;
  /// @brief get current character and advance the cursor
  /// @note does not check if @code cursor + offset >= contents.size() @endcode
  const char_t &get(size_t offset = 1);

  /// @brief advance the cursor if the character is the expected character
  /// @param expected the expected character
  /// @return true if the character is the expected character and the cursor is
  /// advanced, false otherwise
  bool advance_if_is(char_t expected);

  /// @brief advance the cursor if the predicate is true
  /// @tparam Predicate the predicate to check
  /// @param predicate the predicate to check
  /// @return true if the predicate is true and the cursor is advanced, false
  template <typename Predicate>
  bool advance_if(Predicate &&predicate)
    requires std::invocable<Predicate, char_t> &&
             std::convertible_to<Predicate, bool>;

public:
  // [[nodiscard]] const tokens_t &get_tokens() const {
  //   return tokens;
  // }

private:
  /// @brief head of a token
  size_type head = 0;
  /// @brief current cursor position
  size_type cursor = 0;
  /// @brief the contents of the file
  const string_type contents = string_type();
  // /// @brief lexme views(non-owning)
  // lexeme_views_t lexeme_views = lexeme_views_t();
  /// @brief current source line number
  uint_least32_t current_line = 1;
  // /// @brief tokens
  // tokens_t tokens = tokens_t();
  /// @brief errors
  uint_least32_t error_count = 0;
};

} // namespace accat::luce
