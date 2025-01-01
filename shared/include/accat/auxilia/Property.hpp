#pragma once

#include "Status.hpp"

AUXILIA_EXPORT
namespace accat::auxilia {
/// @brief fancy wrapper around the getter and setter functions
/// @tparam Instance the parent class
/// @tparam Field the field type
/// @tparam ReturnType the return type of the getter function
/// @tparam getter the getter function
template <typename Instance,
          typename Field,
          typename ReturnType,
          Field (Instance::*getter)() const,
          ReturnType (Instance::*setter)(const Field &)>
struct Property {
  Instance *instance;
  /// DANGER: do not use this constructor
  constexpr Property() : instance(nullptr) {}
  constexpr Property(Instance *instance) : instance(instance) {}
  constexpr operator ReturnType() const
      noexcept(noexcept((instance->*getter)())) {
    return (instance->*getter)();
  }
  constexpr Property &
  operator=(const Field &value) noexcept(noexcept((instance->*setter)(value))) {
    (instance->*setter)(value);
    return *this;
  }

  template <typename Parent2,
            typename Field2,
            typename ReturnType2,
            Field2 (Parent2::*getter2)() const,
            ReturnType2 (Parent2::*setter2)(const Field2 &)>
  constexpr Property &
  operator=(const Property<Parent2, Field2, ReturnType2, getter2, setter2>
                &that) noexcept(noexcept((that.instance->*getter2)())) {
    return *this = (that.instance->*getter2)();
  }

  constexpr Property &operator=(const Property &that) noexcept(
      noexcept((that.instance->*getter)())) {
    return *this = (that.instance->*getter)();
  }

  constexpr Property &operator=(const ReturnType &value) noexcept(
      noexcept((instance->*setter)(value))) {
    return *this = (instance->*setter)(value);
  }
};

} // namespace accat::auxilia
