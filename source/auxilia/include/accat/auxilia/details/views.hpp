#pragma once

#include "./config.hpp"

namespace accat::auxilia::ranges::views::detail {
struct _invert_endianness_fn {
  template <std::ranges::viewable_range R>
  auto operator()(R &&r) const {
    // clang-format off
    return std::forward<R>(r) 
              | std::views::chunk(2) 
              | std::views::transform([](auto chunk) { 
                  return chunk | std::views::reverse; 
                }) 
              | std::views::reverse
              | std::views::join;
    // clang-format on
  }

  // Allow piping
  template <std::ranges::viewable_range R>
  friend auto operator|(R &&r, const _invert_endianness_fn &e) {
    return e(std::forward<R>(r));
  }
};
} // namespace accat::auxilia::ranges::views::detail
namespace accat::auxilia::ranges::views {
inline constexpr detail::_invert_endianness_fn invert_endianness;
} // namespace accat::auxilia::ranges::views

namespace accat::auxilia::ranges {
using namespace views;
} // namespace accat::auxilia::ranges
