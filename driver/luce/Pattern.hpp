#pragma once

// Loosely coupled components -> Mediator pattern

#include <accat/auxilia/auxilia.hpp>

namespace accat::luce {
/// @interface Mediator
AC_INTERFACE Mediator;
/// @class Component
class Component;
enum class Event : uint8_t {
  kNone = 0,
  // todo: add more events
};
AC_INTERFACE Mediator {
public:
  virtual auxilia::Status notify(Component *, Event) = 0;
};

class Component {
protected:
  Mediator *mediator = nullptr;
  auxilia::Status send(Event event) {
    precondition(mediator,
                 "Fatal: Component has no mediator. "
                 "Check your code.");
    return mediator->notify(this, event);
  }

public:
  /// @brief Default constructor, just make it default-constructible.
  constexpr Component() = default;
  constexpr Component(Mediator *mediator) : mediator(mediator) {}
  virtual ~Component() = default;
};
} // namespace accat::luce
