#pragma once
#include <random>
#include <limits>
#include <type_traits>

#include "config.hpp"

namespace accat::auxilia {
namespace detail {
/**
 * @brief A random integer generator for integral types.
 *
 * This struct provides functionality to generate random integers of a specified
 * integral type. It uses a uniform integer distribution to generate random
 * numbers.
 *
 * @tparam Ty The integral type for which random numbers will be generated.
 * @requires Ty must be an integral type.
 */
template <typename Ty>
  requires std::is_integral_v<Ty> || std::is_same_v<Ty, __uint128_t>
struct _random_integer_generator {
  /**
   * @brief Generates a random integer of type Ty.
   *
   * This function generates a random integer of type Ty using a uniform integer
   * distribution.
   *
   * @return A random integer of type Ty.
   */
  inline Ty operator()() const;

  /**
   * @brief Generates a random integer of type Ty within a specified range.
   *
   * This function generates a random integer of type Ty within the range [min,
   * max).
   *
   * @param min The lower bound of the range (inclusive).
   * @param max The upper bound of the range (exclusive).
   * @return A random integer of type Ty within the specified range.
   * @pre min must be less than max.
   */
  inline Ty operator()(Ty min, Ty max) const;
};

template <typename Ty>
  requires std::is_integral_v<Ty> || std::is_same_v<Ty, __uint128_t>
inline Ty _random_integer_generator<Ty>::operator()() const {
  static std::uniform_int_distribution<Ty> dist(std::numeric_limits<Ty>::min(),
                                                std::numeric_limits<Ty>::max());
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return dist(gen);
}

template <typename Ty>
  requires std::is_integral_v<Ty> || std::is_same_v<Ty, __uint128_t>
inline Ty _random_integer_generator<Ty>::operator()(const Ty min,
                                                    const Ty max) const {
  contract_assert(min < max, "min must be less than max");
  return (((*this).operator()()) % (max - min)) + min;
}

} // namespace detail
/// @brief A random integer generator for 8-bit unsigned integers.
/// @todo not working: uniform_int_distribution cannot accept unsigned char
inline constexpr detail::_random_integer_generator<uint8_t> rand_u8;
/// @brief A random integer generator for 16-bit unsigned integers.
inline constexpr detail::_random_integer_generator<uint16_t> rand_u16;
/// @brief A random integer generator for 32-bit unsigned integers.
inline constexpr detail::_random_integer_generator<uint32_t> rand_u32;
/// @brief A random integer generator for 64-bit unsigned integers.
inline constexpr detail::_random_integer_generator<uint64_t> rand_u64;
#if defined(__SIZEOF_INT128__)
/// @brief A random integer generator for 128-bit unsigned integers.
inline constexpr detail::_random_integer_generator<__uint128_t> rand_u128;
#endif
} // namespace accat::auxilia
