#pragma once

// Loosely coupled components -> Mediator pattern

#include <spdlog/spdlog.h>
#include <accat/auxilia/auxilia.hpp>
#include <accat/auxilia/details/id.hpp>
#include <utility>

namespace accat::luce {
/// @interface Mediator
class Mediator;

/// @class Component
class Component;
/// @note for magic_enum to perform cast, the enum value must be explicitly
/// defined in the enum class
enum class Event : uint8_t {
  kNone = 0,
  kLoadProgram = 1,
  kRunTask = 2,
  kTaskFinished = 3,
  // todo: add more events
};
namespace event {
consteval auto to_string_view(const Event event) noexcept {
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
  default:
    return "kUnknown"sv;
  }
  std::unreachable();
}
} // namespace event
class Mediator {
public:
  constexpr Mediator() = default;

public:
  virtual auxilia::Status notify(Component *, Event) = 0;
  auxilia::Status set_as_parent(Component *);

protected:
  virtual ~Mediator() = default;
};

class Component {
  friend class Mediator;

protected:
  using pid_t = uint32_t;

public:
  Mediator *mediator = nullptr;
  auxilia::Status send(const Event event) {
    precondition(mediator,
                 "Fatal: Component has no mediator. "
                 "Check your code.")
    return mediator->notify(this, event);
  }

public:
  explicit Component(Mediator *mediator = nullptr) : mediator(mediator) {
    spdlog::debug("Component with id {} created.", id_);
  }
  Component(const Component &) = delete;
  Component &operator=(const Component &) = delete;
  constexpr pid_t id() const noexcept { return id_; }
  Component(Component &&that) noexcept
      : mediator(that.mediator), id_(that.id_) {
    spdlog::debug("Component with id {} moved.", id_);
    that.mediator = nullptr;
    that.id_ = std::numeric_limits<pid_t>::max();
  }
  Component &operator=(Component &&that) noexcept {
    if (this != &that) {
      mediator = that.mediator;
      id_ = that.id_;
      that.mediator = nullptr;
      that.id_ = std::numeric_limits<pid_t>::max();
    }
    return *this;
  }

protected:
  virtual ~Component() noexcept { auxilia::id::release(id_); }

private:
  pid_t id_ = auxilia::id::get();
};
inline auxilia::Status Mediator::set_as_parent(Component *child) {
  precondition(child->mediator == nullptr || child->mediator == this,
               "Component already has a parent. Check your code, set the "
               "parent to nullptr first.")
  child->mediator = this;
  return auxilia::OkStatus();
}

} // namespace accat::luce
