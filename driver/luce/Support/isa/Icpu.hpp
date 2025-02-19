#pragma once

#include <accat/auxilia/auxilia.hpp>
#include "luce/Support/utils/Pattern.hpp"
#include "luce/Support/isa/Word.hpp"

// workaround.
// better to make an interface for this, but it's hard to do so
#include "luce/Support/isa/riscv32/Register.hpp"
namespace accat::luce {
class Context;
}
namespace accat::luce::isa {
class Icpu : public Component {
public:
  enum class State : uint8_t {
    /// no program is running
    kVacant = 0,
    /// cpu is running and executing instructions
    kRunning,
    /// finished running the program
  };

protected:
  using vaddr_t = isa::virtual_address_t;
  using paddr_t = isa::physical_address_t;
  std::optional<pid_t> task_id_;
  State state_ = State::kVacant;

public:
  Icpu(Mediator *parent = nullptr) : Component(parent) {}
  Icpu(const Icpu &other) = delete;
  Icpu(Icpu &&other) noexcept = default;
  Icpu &operator=(const Icpu &other) = delete;
  Icpu &operator=(Icpu &&other) noexcept = default;
  virtual ~Icpu() = default;

public:
  virtual auto fetch(vaddr_t) const
      -> auxilia::StatusOr<std::span<const std::byte>> = 0;
  virtual auto write(vaddr_t, const std::span<const std::byte>)
      -> auxilia::Status = 0;
  virtual auto pc() noexcept -> isa::Word & = 0;
  virtual auto gpr() noexcept -> isa::GeneralPurposeRegisters & = 0;
  virtual auxilia::Status execute_shuttle() = 0;
  virtual auto switch_context(std::shared_ptr<Context>, const pid_t) noexcept
      -> Icpu & = 0;

public:
  auto task_id() const noexcept -> pid_t {
    precondition(task_id_, "Task id is not set or invalid")
    return *task_id_;
  }
  constexpr auto is_vacant() const noexcept {
    return state_ == State::kVacant;
  }
};
} // namespace accat::luce::isa
