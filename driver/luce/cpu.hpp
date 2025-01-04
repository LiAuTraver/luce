#pragma once

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
#include <array>
#include <cstddef>

namespace accat::luce {
class CentralProcessingUnit {
  std::shared_ptr<Context> context_;
  Timer cpu_timer_;
  enum class State : uint8_t {
    /// no program is running
    kVacant = 0,
    /// cpu is running and executing instructions
    kRunning,
  };
  State state_ = State::kVacant;
  using instruction_t = isa::instruction_size_t;
  using instruction_bytes_t = std::array<std::byte, sizeof(instruction_t)>;

public:
  CentralProcessingUnit() = default;
  CentralProcessingUnit(const CentralProcessingUnit &) = delete;
  auto operator=(const CentralProcessingUnit &) = delete;
  CentralProcessingUnit(CentralProcessingUnit &&) noexcept = default;
  CentralProcessingUnit &operator=(CentralProcessingUnit &&) noexcept = default;

public:
  auto &switch_context(std::shared_ptr<Context> context) noexcept {
    precondition(state_ == State::kVacant, "CPU is already running a program")
    context_ = context;
    return *this;
  }
  constexpr auto is_vacant() const noexcept { return state_ == State::kVacant; }
  auto &detach_context() noexcept {
    if (context_.use_count() == 1) {
      spdlog::info("The program has finished running");
    }
    context_.reset();
    return *this;
  }
  auxilia::Status execute() {
    precondition(context_, "No program to execute")

    auto executeShuttle = [&]() {
      state_ = State::kRunning;
      defer { state_ = State::kVacant; };
      return shuttle();
    };
    auto [res, elapsed] = cpu_timer_.measure(executeShuttle);
    spdlog::info("CPU execution time: {} ms", elapsed);
    return res;
  }
  auxilia::Status shuttle() {
    auto maybe_instruction = fetch();
    return {};
    // TODO();
  }
  auxilia::StatusOr<instruction_t> fetch() {
    TODO();
    return {};
  }
};
/// @implements Component
class CPUs : public Component {
  // for debug and easy to understand, we use 1 cpus
  std::array<CentralProcessingUnit, 1> cpus{};

public:
  constexpr CPUs() = default;
  CPUs(const CPUs &) = delete;
  auto operator=(const CPUs &) = delete;
  CPUs(CPUs &&) noexcept = default;
  CPUs &operator=(CPUs &&) noexcept = default;
  ~CPUs() = default;

  CPUs(Mediator *mediator) : Component(mediator) {}

public:
  auxilia::Status execute_shuttle() {
    for (auto &cpu : cpus) {
      if (cpu.is_vacant()) {
        if (auto res = cpu.execute(); !res) {
          spdlog::error("CPU execution failed: {}", res.message());
          return res;
        }
      }
    }
    spdlog::warn(
        "No CPU available. \n{}",
        fmt::format(
            fmt::fg(fmt::color::cyan),
            "Note: currently not implemented for multi-core and parallel "
            "execution"));
    return {};
  }
};
} // namespace accat::luce
