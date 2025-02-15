#pragma once

#include <accat/auxilia/auxilia.hpp>

#include "utils/Pattern.hpp"
#include "config.hpp"
#include "luce/MainMemory.hpp"

namespace accat::luce {
class SystemBus : public Component {
public:
  SystemBus() = default;
  SystemBus(Mediator *parent) : Component(parent) {}
  SystemBus(const SystemBus &) = delete;
  SystemBus &operator=(const SystemBus &) = delete;
  SystemBus(SystemBus &&) noexcept = default;
  SystemBus &operator=(SystemBus &&) noexcept = default;
  ~SystemBus() = default;
};
} // namespace accat::luce
