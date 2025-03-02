#pragma once

#include <accat/auxilia/auxilia.hpp>
#include "luce/Support/utils/Pattern.hpp"
#include "luce/Support/isa/Word.hpp"

// workaround.
// better to make an interface for this, but it's hard to do so
#include "luce/Support/isa/riscv32/Register.hpp"
namespace accat::luce {
class Context;
class Task;
} // namespace accat::luce
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
  virtual auto pc() noexcept [[clang::lifetimebound]] -> isa::Word & = 0;
  virtual auto gpr() noexcept [[clang::lifetimebound]]
  -> isa::GeneralPurposeRegisters & = 0;
  virtual auto switch_task(Task *) noexcept [[clang::lifetimebound]]
  -> Icpu & = 0;
  virtual auto atomic_address() noexcept [[clang::lifetimebound]]
  -> std::optional<vaddr_t> & = 0;
  virtual auxilia::Status execute_shuttle() = 0;

public:
  constexpr auto is_vacant() const noexcept {
    return state_ == State::kVacant;
  }
};
} // namespace accat::luce::isa
