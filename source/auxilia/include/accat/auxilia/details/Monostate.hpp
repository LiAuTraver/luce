#pragma once

#include "./config.hpp"
#include "./format.hpp"

namespace accat::auxilia {
/// @brief Represents a stateless type that can be used as a
/// placeholder
/// @implements Printable, Viewable
/// @remark Different from `std::monostate`, this class implements two
/// traits which can be extremely important in my @link Variant
/// @endlink class.
EXPORT_AUXILIA
class Monostate final :public Printable<Monostate>,
                        public Viewable<Monostate> {
public:
  inline  Monostate() = default;
  inline constexpr ~Monostate() = default;
  inline  Monostate(const Monostate &) {}
  inline  Monostate(Monostate &&) noexcept {}
  inline auto operator=(const Monostate &) -> Monostate&  {
    return *this;
  }
  inline auto operator=(Monostate &&) noexcept
      -> Monostate  &{
    return *this;
  }

public:
  inline  auto to_string(const FormatPolicy &) const
      -> string_type {
    return {};
  }
  inline  auto
  to_string_view(const FormatPolicy &) const
      -> string_view_type {
    return {};
  }
  friend inline  auto operator==(const Monostate &,
                                          const Monostate &) noexcept
      -> bool {
    return true;
  }
  friend inline  auto operator<=>(const Monostate &,
                                           const Monostate &) noexcept
      -> std::strong_ordering {
    return std::strong_ordering::equal;
  }
};
} // namespace accat::auxilia
