#pragma once

#include <fmt/color.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "luce/Support/utils/Pattern.hpp"
#include "luce/Support/utils/Timer.hpp"
#include "accat/auxilia/details/Status.hpp"
#include "luce/config.hpp"
#include "luce/Task.hpp"
#include "luce/Support/isa/architecture.hpp"
#include "luce/Support/isa/Icpu.hpp"
#include "luce/cpu/mmu.hpp"
#include <accat/auxilia/auxilia.hpp>
#include <accat/auxilia/details/macros.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
namespace accat::luce {
namespace isa {
class IDisassembler;
}
class Monitor;
} // namespace accat::luce
namespace accat::luce::isa {
class IInstruction;
}
namespace accat::luce {
class CentralProcessingUnit : public isa::Icpu {
  Task *task_;
  MemoryManagementUnit mmu_;
  Timer cpu_timer_;
  std::optional<vaddr_t> atomic_address_;

public:
  CentralProcessingUnit(Mediator * = nullptr);
  virtual ~CentralProcessingUnit() override;

public:
  virtual auto switch_task(Task *task) noexcept -> Icpu & override {
    precondition(state_ == State::kVacant, "CPU is already running a program")
    atomic_address_.reset();
    task_ = task;
    return *this;
  }

public:
  virtual auto execute_shuttle() -> auxilia::Status override;
  virtual auto fetch(vaddr_t) const
      -> auxilia::StatusOr<std::span<const std::byte>> override;
  virtual auto write(vaddr_t, const std::span<const std::byte>)
      -> auxilia::Status override;
  virtual auto pc() noexcept -> isa::Word & override {
    return task_->context().program_counter;
  }
  virtual auto gpr() noexcept -> isa::GeneralPurposeRegisters & override {
    return *task_->context().general_purpose_registers();
  }
  virtual auto atomic_address() noexcept -> std::optional<vaddr_t> & override {
    return atomic_address_;
  }
  virtual auto handle_syscall() -> auxilia::Status override;

private:
  auto detach_task() noexcept -> CentralProcessingUnit &;
  auto shuttle() -> auxilia::Status;
  auto decode_and_execute() -> auxilia::Status;
  auto execute(isa::IInstruction *) -> auxilia::Status;
  auto monitor() const noexcept -> Monitor *;
  /// used to handle generic exceptions,subject to change
  auto trap() -> auxilia::Status;
};

} // namespace accat::luce
