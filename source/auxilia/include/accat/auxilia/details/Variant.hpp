#pragma once

#include <type_traits>
#include <utility>
#include "./config.hpp"
#include "./format.hpp"

#include "./Monostate.hpp"
EXPORT_AUXILIA
namespace accat::auxilia {
/// @brief A simple variant wrapper class around @link std::variant @endlink for
/// convenience when evaluating expressions, especially when the operation was
/// `to_string` or check the type's name when debugging.
/// @note exception-free variant wrapper
template <Variantable... Types>
class Variant : public Printable<Variant<Types...>>,
                public Viewable<Variant<Types...>> {
  using self_type = Variant<Types...>;

public:
  using variant_type = std::variant<Types...>;
  using string_type = typename Printable<self_type>::string_type;
  using string_view_type = typename Viewable<self_type>::string_view_type;

public:
  inline constexpr Variant() = default;

  template <typename Ty>
    requires(!std::same_as<std::decay_t<Ty>, Variant>)
  Variant(Ty &&value) : my_variant(std::forward<Ty>(value)) {}

  Variant(const Variant &) = default;
  Variant(Variant &&that) noexcept : my_variant(std::move(that.my_variant)) {
    that.my_variant.template emplace<Monostate>();
  }
  Variant &operator=(const Variant &) = default;
  Variant &operator=(Variant &&that) noexcept {
    my_variant = std::move(that.my_variant);
    that.my_variant.template emplace<Monostate>();
    return *this;
  }
  ~Variant() = default;

public:
  auto set(Variant &&that = {}) noexcept -> Variant & {
    my_variant = std::move(that.my_variant);
    that.my_variant.template emplace<Monostate>();
    return *this;
  }
  template <typename Callable>
  auto visit(this auto &&self, Callable &&callable) -> decltype(auto) {
    using ReturnType = decltype(std::forward<Callable>(callable)(
        std::declval<variant_type>()));
    static_assert(std::is_default_constructible_v<ReturnType> ||
                      std::is_same_v<ReturnType, void>,
                  "ReturnType must be default constructible");
    if constexpr (std::is_same_v<ReturnType, void>) {
      return std::visit(std::forward<Callable>(callable), self.my_variant);
    } else {
      return self.is_valid()
                 ? static_cast<ReturnType>(std::visit(
                       std::forward<Callable>(callable), self.my_variant))
                 : ReturnType{};
    }
  }
  auto type_name() const {
    return is_valid() ? this->visit([]([[maybe_unused]] const auto &value)
                                        -> string_view_type {
      return typeid(value).name();
    })
                      : "invalid state"sv;
  }
  auto index() const noexcept { return my_variant.index(); }
  template <typename... Args>
    requires requires {
      std::declval<variant_type>().template emplace<Args...>(
          std::declval<Args>()...);
    }
  auto emplace(Args &&...args) -> decltype(auto) {
    return my_variant.template emplace<Args...>(std::forward<Args>(args)...);
  }
  template <typename... Args>
    requires requires {
      std::declval<variant_type>().template emplace<Args...>(
          std::declval<Args>()...);
    }
  auto emplace_and_then(Args &&...args) -> decltype(auto) {
    my_variant.template emplace<Args...>(std::forward<Args>(args)...);
    return *this;
  }
  template <typename Args>
    requires requires { std::declval<variant_type>().template emplace<Args>(); }
  constexpr auto
  emplace() noexcept(noexcept(my_variant.template emplace<Args>()))
      -> decltype(auto) {
    return my_variant.template emplace<Args>();
  }
  auto &get() const { return my_variant; }
  constexpr auto swap(Variant &that) noexcept(
      std::conjunction_v<std::is_nothrow_move_constructible<Types...>,
                         std::is_nothrow_swappable<Types...>>) -> Variant & {
    my_variant.swap(that.my_variant);
    return *this;
  }
  constexpr auto clear(this auto &&self) noexcept(noexcept(
      self.my_variant.template emplace<Monostate>())) -> decltype(auto) {
    self.my_variant.template emplace<Monostate>();
    return self;
  }
  constexpr auto empty() const noexcept -> bool {
    return my_variant.index() == 0;
  }
  auto underlying_string(const FormatPolicy &format_policy =
                             FormatPolicy::kDefault) const -> string_type {
    return this->visit([&](const auto &value) -> string_type {
      return value.to_string(format_policy);
    });
  }

private:
  variant_type my_variant{Monostate{}};

private:
  inline bool is_valid() const noexcept {
    auto ans = my_variant.index() != std::variant_npos;
    contract_assert(ans)
    return ans;
  }

public:
  constexpr auto to_string(const FormatPolicy &format_policy) const
      -> string_type {
    if (format_policy == FormatPolicy::kDefault) {
      return typeid(decltype(*this)).name();
    } else if (format_policy == FormatPolicy::kDetailed) {
      return typeid(decltype(*this)).raw_name();
    }
    std::unreachable();
  }
  constexpr auto to_string_view(const FormatPolicy &format_policy) const
      -> string_view_type {
    if (format_policy == FormatPolicy::kDefault) {
      return typeid(decltype(*this)).name();
    } else if (format_policy == FormatPolicy::kDetailed) {
      return typeid(decltype(*this)).raw_name();
    }
    std::unreachable();
  }

private:
  /// @brief get the value of the variant; a wrapper around @link std::get_if
  /// @endlink
  template <class Ty, class... MyTypes>
  friend inline constexpr auto get_if(Variant<MyTypes...> *v) noexcept;
  /// @brief get the value of the variant; a wrapper around @link std::get
  /// @endlink
  template <class Ty, class... MyTypes>
  friend inline auto get(const Variant<MyTypes...> &v)
      -> decltype(auto);
};
/// @brief check if the variant holds a specific type;
///  a wrapper around @link std::holds_alternative @endlink
template <class Ty, class... MyTypes>
inline constexpr bool holds_alternative(const Variant<MyTypes...> &v) noexcept {
  return std::holds_alternative<Ty>(v.get());
}
template <class Ty, class... MyTypes>
inline auto get(const Variant<MyTypes...> &v) -> decltype(auto) {
  return v.is_valid() ? std::get<Ty>(v.get()) : Ty{};
}
template <class Ty, class... MyTypes>
[[nodiscard]]
inline constexpr auto get_if(Variant<MyTypes...> *v) noexcept {
  return v->is_valid() ? std::get_if<Ty>(&v->get()) : nullptr;
}
} // namespace accat::auxilia
