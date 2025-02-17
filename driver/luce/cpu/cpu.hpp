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
#include "mmu.hpp"
#include <accat/auxilia/auxilia.hpp>
#include <accat/auxilia/details/macros.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <fmt/ranges.h>

namespace accat::luce {
class Monitor;
}
namespace accat::luce::isa::riscv32 {
class Instruction;
}
namespace accat::luce {
class CentralProcessingUnit : public Component {
  std::shared_ptr<Context> context_;
  std::optional<pid_t> task_id_;
  MemoryManagementUnit mmu_;
  using vaddr_t = MemoryManagementUnit::vaddr_t;
  using paddr_t = MemoryManagementUnit::paddr_t;

  Timer cpu_timer_;

public:
  enum class State : uint8_t {
    /// no program is running
    kVacant = 0,
    /// cpu is running and executing instructions
    kRunning,
    /// finished running the program
  };

private:
  State state_ = State::kVacant;
  using instruction_t = isa::instruction_size_t;
  using instruction_bytes_t = std::array<std::byte, sizeof(instruction_t)>;

public:
  CentralProcessingUnit(Mediator * = nullptr);
  CentralProcessingUnit(const CentralProcessingUnit &) = delete;
  auto operator=(const CentralProcessingUnit &) = delete;
  CentralProcessingUnit(CentralProcessingUnit &&) noexcept = default;
  CentralProcessingUnit &operator=(CentralProcessingUnit &&) noexcept = default;

public:
  auto &switch_context(std::shared_ptr<Context> context,
                       const pid_t taskid) noexcept {
    precondition(state_ == State::kVacant && task_id_ == std::nullopt,
                 "CPU is already running a program")
    context_ = context;
    task_id_ = taskid;
    return *this;
  }
  constexpr auto is_vacant() const noexcept {
    return state_ == State::kVacant;
  }
  auto detach_context() noexcept -> CentralProcessingUnit &;
  auxilia::Status execute_shuttle();
  auto context() noexcept -> decltype(auto) {
    return context_;
  }
  auto fetch(vaddr_t) const -> auxilia::StatusOr<std::span<const std::byte>>;
  auto write(vaddr_t, const std::span<const std::byte>) -> auxilia::Status;

private:
  auxilia::Status shuttle();
  auxilia::Status decode_and_execute();
  auxilia::Status execute(isa::Instruction *);

public:
  auto task_id() const noexcept {
    precondition(task_id_, "Task id is not set or invalid")
    return *task_id_;
  }
};

/// @implements Component
class CPUs : public Component {
  // for debug and easy to understand, we use 1 cpus
  // std::array<CentralProcessingUnit, 1> cpus{};
  CentralProcessingUnit cpu;

public:
  CPUs() = default;
  CPUs(const CPUs &) = delete;
  auto operator=(const CPUs &) = delete;
  CPUs(CPUs &&) noexcept = default;
  CPUs &operator=(CPUs &&) noexcept = default;
  ~CPUs() = default;

  CPUs(Mediator *parent = nullptr) : Component(parent), cpu(parent) {}

public:
  auxilia::Status execute_shuttle() {
    if (cpu.is_vacant()) {
      if (auto res = cpu.execute_shuttle(); !res) {
        return res;
      }
    }

    return {};
    // spdlog::warn(
    //     "No CPU available. \n{}",
    //     fmt::format(
    //         fmt::fg(fmt::color::cyan),
    //         "Note: currently not implemented for multi-core and parallel "
    //         "execution"));
  }
  auxilia::Status attach_context(std::shared_ptr<Context> context,
                                 const pid_t taskid) {
    cpu.switch_context(context, taskid);
    return {};
  }
};
} // namespace accat::luce
