#pragma once

// Loosely coupled components -> Mediator pattern

#include <spdlog/spdlog.h>
#include <accat/auxilia/auxilia.hpp>
#include <functional>
#include <utility>

#include "luce/config.hpp"

namespace accat::luce {

/// @interface Mediator
struct Mediator;

/// @struct Component
struct Component;

enum class Event : uint8_t {
  kRestartOrResumeTask = 4,
  kPauseTask = 6,
};
namespace tr {
struct Command {
  virtual auto execute(Mediator *, std::function<void(void)> = nullptr)
      -> auxilia::Status = 0;

  virtual ~Command() = default;
};
} // namespace tr
struct Mediator {
public:
  constexpr Mediator() = default;

public:
  virtual auxilia::Status
  notify(Component *, Event, std::function<void(void)> = []() {}) = 0;
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
  auxilia::Status
  send(const Event event, std::function<void(void)> &&callback = []() {}) {
    return mediator->notify(this, event, std::move(callback));
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
