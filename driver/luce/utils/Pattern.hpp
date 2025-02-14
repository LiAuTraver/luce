#pragma once

// Loosely coupled components -> Mediator pattern

#include <spdlog/spdlog.h>
#include <accat/auxilia/auxilia.hpp>
#include <functional>
#include <utility>

#include "accat/auxilia/details/Status.hpp"
#include "luce/config.hpp"

namespace accat::luce {
/// @interface Mediator
struct Mediator;

/// @struct Component
struct Component;
/// @note for magic_enum to perform cast, the enum value must be explicitly
/// defined in the enum class
/// @todo `procedual programming(switch enum) makes it clear, but also makes the
/// program unmaintainable` -- Scott Meyers, More Effective C++.
///       but currently I have no time to refactor this to use
/// polymorphism/visitor pattern.
enum class Event : uint8_t {
  kNone = 0,
  kLoadProgram = 1,
  kRunTask = 2,
  kTaskFinished = 3,
  kRestartOrResumeTask = 4,
  kPrintWatchPoint = 5,
  kPauseTask = 6,
  // todo: add more events
};
namespace event {
constexpr auto to_string_view(const Event event) noexcept {
  using namespace std::string_view_literals;
  switch (event) {
  case Event::kNone:
    return "kNone"sv;
  case Event::kLoadProgram:
    return "kLoadProgram"sv;
  case Event::kRunTask:
    return "kRunTask"sv;
  case Event::kTaskFinished:
    return "kTaskFinished"sv;
  case Event::kRestartOrResumeTask:
    return "kRestartTask"sv;
  case Event::kPrintWatchPoint:
    return "kPrintWatchPoint"sv;
  default:
    return "kUnknown"sv;
  }
  std::unreachable();
}
} // namespace event
struct Mediator {
public:
  constexpr Mediator() = default;

public:
  virtual auxilia::Status notify(Component *, Event, std::function<void(void)> = [](){}) = 0;
  // virtual auxilia::StatusOr<std::any> query(Component*, Event,
  auxilia::Status set_as_parent(Component *);

protected:
  virtual ~Mediator() = default;
};

struct LUCE_API Component {
  friend struct Mediator;

protected:
  using pid_t = uint32_t;

public:
  Mediator *mediator = nullptr;
  auxilia::Status send(const Event event, std::function<void(void)>&& callback = []() {}) {
    return mediator->notify(this, event , std::move(callback));
  }

public:
  explicit Component(Mediator *mediator = nullptr) : mediator(mediator) {
    spdlog::debug("Component with id {} created.", id_);
  }
  Component(const Component &) = delete;
  Component &operator=(const Component &) = delete;
  constexpr pid_t id() const noexcept /* final */ {
    return id_;
  }
  Component(Component &&that) noexcept
      : mediator(that.mediator), id_(that.id_) {
    spdlog::debug("Component with id {} moved.", id_);
    that.mediator = nullptr;
    that.id_ = (std::numeric_limits<pid_t>::max)();
  }
  Component &operator=(Component &&that) noexcept {
    if (this != &that) {
      mediator = that.mediator;
      id_ = that.id_;
      that.mediator = nullptr;
      that.id_ = (std::numeric_limits<pid_t>::max)();
    }
    return *this;
  }

protected:
  virtual ~Component() noexcept {
    auxilia::id::release(id_);
  }

private:
  pid_t id_ = auxilia::id::get();
};
inline auxilia::Status Mediator::set_as_parent(Component *child) {
  if (child->mediator && child->mediator != this) {
    return auxilia::AlreadyExistsError(
        "Component already has a parent. Check your code, set the parent to "
        "nullptr first.");
  }
  child->mediator = this;
  return {};
}

} // namespace accat::luce
