#pragma once
#include "details/macros.hpp"

AUXILIA_EXPORT
namespace accat::auxilia {
class Monostate;
/// @brief a concept that checks if the types are variantable for my custom
/// @link Variant @endlink class,
/// where the first type must be @link Monostate @endlink
/// @tparam Types the types to check
template <typename... Types>
concept Variantable = requires {
  std::is_same_v<std::tuple_element_t<0, std::tuple<Types...>>, Monostate>;
};
/// @brief represents a value that can be stored in a
/// @link StatusOr @endlink object
/// @tparam Ty the type of the value
/// @remarks similiar to Microsoft's @link std::_SMF_control @endlink class,
/// which was used in @link std::optional @endlink
template <typename Ty>
concept Storable = true;
// FIXME: not working
// std::conjunction_v<std::is_default_constructible<Ty>,
//                                       std::is_nothrow_destructible<Ty>,
//                                       std::is_nothrow_constructible<Ty>>;

template <Variantable... Types> class Variant;
class Status;
template <Storable Ty> class StatusOr;
// class file_reader;
using string = ::std::string;
using string_view = ::std::string_view;
using path = ::std::filesystem::path;
using ifstream = ::std::ifstream;
using ostringstream = ::std::ostringstream;
using namespace ::std::string_view_literals;
using namespace ::std::string_literals;
constexpr auto isspacelike = [](const char &c) noexcept -> bool {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
};
inline consteval const char *raw(const char *str) {
  while (*str && (*str == '\n')) {
    ++str;
  }
  char *ptr = const_cast<char *>(str);
  while (*ptr)
    ptr++;

  // remove trailing newline and space
  while (ptr != str && isspacelike(*ptr)) {
    *ptr = '\0';
    ptr--;
  }
  return str;
}
template <typename T = void> inline T *alloc(const size_t size) {
  if (auto ptr = malloc(size))
    return static_cast<T *>(ptr);

  fprintf(stderr, "Failed to allocate memory. The program will now exit.");
  contract_assert(0, "Failed to allocate memory");
  std::abort();
}

template <const auto &Str> struct array_size_t {
  inline static constexpr auto size =
      std::extent_v<std::remove_reference_t<decltype(Str)>>;
};
template <const auto &Str> using array_size_v = typename array_size_t<Str>::size;
} // namespace accat::auxilia
