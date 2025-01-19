#pragma once

#include "luce/utils/Pattern.hpp"

namespace accat::luce {
class Debugger : public Component {
public:
  explicit Debugger(Mediator *parent = nullptr) : Component(parent) {}
  Debugger(const Debugger &) = delete;
  Debugger &operator=(const Debugger &) = delete;
  Debugger(Debugger &&) = default;
  Debugger &operator=(Debugger &&) = default;
  virtual ~Debugger() override = default;

public:
private:
  
};

} // namespace accat::luce
