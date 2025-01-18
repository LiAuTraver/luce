#pragma once
#include <string>
#include "accat/auxilia/details/Status.hpp"
#include "luce/utils/Pattern.hpp"

namespace accat::luce {
class Monitor;
}

namespace accat::luce {
class replWatcher : public Component {
public:
  replWatcher() = default;
  explicit replWatcher(const replWatcher &) = delete;
  explicit replWatcher(replWatcher &&) = delete;
  auto operator=(const replWatcher &) -> replWatcher & = delete;
  auto operator=(replWatcher &&) -> replWatcher & = delete;
  virtual ~replWatcher() override = default;

public:
  explicit replWatcher(Mediator *parent) : Component(parent) {}

public:
  auxilia::StatusOr<std::string> read();
  auxilia::Status inspect(std::string_view);
  Monitor *monitor() const noexcept;
};
} // namespace accat::luce
