﻿#pragma once

#include <fmt/color.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "Pattern.hpp"
#include "Timer.hpp"
#include "accat/auxilia/details/Status.hpp"
#include "config.hpp"
#include "Task.hpp"
#include "isa/architecture.hpp"
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
namespace accat::luce {
class CentralProcessingUnit : public Component {
  std::shared_ptr<Context> context_;
  std::optional<pid_t> task_id_;

  Timer cpu_timer_;
  enum class State : uint8_t {
    /// no program is running
    kVacant = 0,
    /// cpu is running and executing instructions
    kRunning,
    /// finished running the program
  };
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
  constexpr auto is_vacant() const noexcept { return state_ == State::kVacant; }
  auto detach_context() noexcept -> CentralProcessingUnit &;
  auxilia::Status execute();
  auxilia::Status shuttle();
  auxilia::StatusOr<std::span<const std::byte>> fetch();

public:
  auto task_id() const noexcept {
    precondition(task_id_, "Task id is not set or invalid")
    return *task_id_;
    // return task_id_.value_or(std::numeric_limits<uint32_t>::max());
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
      if (auto res = cpu.execute(); !res) {
        // if (res.code() == auxilia::Status::Code::kReturning) {
        //   if (event::to_string_view(Event::kTaskFinished) == res.message()) {
        //     spdlog::info("Hit good ol' deadbeef, program finished!");
        //     return {};
        //   }
        //   spdlog::error("Unknown event: {}", res.message());
        //   return res;
        // }
        // error, return as is
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
