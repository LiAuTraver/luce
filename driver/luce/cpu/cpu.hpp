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
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <fmt/ranges.h>

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

public:
  CentralProcessingUnit(Mediator * = nullptr);
  virtual ~CentralProcessingUnit() override;

public:
  virtual auto switch_task(Task *task) noexcept -> Icpu & override {
    precondition(state_ == State::kVacant, "CPU is already running a program")
    task_ = task;
    return *this;
  }

public:
  virtual auxilia::Status execute_shuttle() override;
  virtual auto fetch(vaddr_t) const
      -> auxilia::StatusOr<std::span<const std::byte>> override;
  virtual auto write(vaddr_t, const std::span<const std::byte>)
      -> auxilia::Status override;
  auto pc() noexcept -> isa::Word & override {
    return task_->context().program_counter;
  }
  auto gpr() noexcept -> isa::GeneralPurposeRegisters & override {
    return *task_->context().general_purpose_registers();
  }

private:
  auto detach_task() noexcept -> CentralProcessingUnit &;
  auxilia::Status shuttle();
  auxilia::Status decode_and_execute();
  auxilia::Status execute(isa::IInstruction *);
  auto monitor() const noexcept -> Monitor *;
};

/// @implements Component
class CPUs : public Component {
  // for debug and easy to understand, we use 1 cpus
  // std::array<CentralProcessingUnit, 1> cpus{};
  std::unique_ptr<isa::Icpu> cpu;

public:
  CPUs() = default;
  CPUs(const CPUs &) = delete;
  auto operator=(const CPUs &) = delete;
  CPUs(CPUs &&) noexcept = default;
  CPUs &operator=(CPUs &&) noexcept = default;
  virtual ~CPUs() override = default;

  CPUs(Mediator *parent = nullptr) : Component(parent) {
    cpu = std::make_unique<CentralProcessingUnit>(parent);
  }

public:
  auxilia::Status execute_shuttle() {
    if (cpu->is_vacant()) {
      if (auto res = cpu->execute_shuttle(); !res) {
        return res;
      }
      return {};
    }

    spdlog::warn(
        "No CPU available. \n{}",
        fmt::format(
            fmt::fg(fmt::color::cyan),
            "Note: currently not implemented for multi-core and parallel "
            "execution"));
    return {};
  }
  auto attach_task(Task *task) noexcept -> CPUs & {
    cpu->switch_task(task);
    return *this;
  }
};
} // namespace accat::luce
