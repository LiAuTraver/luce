#include "deps.hh"

#include "luce/cpu/cpu.hpp"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>
#include "luce/Monitor.hpp"
#include "Support/isa/riscv32/isa.hpp"
namespace accat::luce {
CentralProcessingUnit::CentralProcessingUnit(Mediator *parent)
    : Component(parent), mmu_(this) {}
auto CentralProcessingUnit::detach_context() noexcept
    -> CentralProcessingUnit & {
  if (context_.use_count() == 1) {
    spdlog::warn("Seems like the context is in a broken state, destroying...");
  }
  context_.reset();
  task_id_.reset();
  state_ = State::kVacant;
  return *this;
}
auxilia::Status CentralProcessingUnit::execute() {
  precondition(context_, "No program to execute")

  auto executeShuttle = [&]() {
    state_ = State::kRunning;
    defer {
      state_ = State::kVacant;
    };
    return shuttle();
  };
  auto [res, elapsed] = cpu_timer_.measure(executeShuttle);
  spdlog::info("CPU execution time: {} ms", elapsed);
  return res;
}
auxilia::Status CentralProcessingUnit::shuttle() {
  auto maybe_bytes = fetch();
  if (!maybe_bytes) {
    return maybe_bytes.as_status();
  }
  context_->program_counter += sizeof(instruction_t);
  auto bytes = std::move(maybe_bytes).value();
  for (const auto i : std::views::iota(0ull, sizeof(instruction_t))) {
    context_->instruction_register[i] = bytes[i];
  }
  // convert little-endian to big-endian for more human-readable output
  spdlog::info("Fetched instruction: {:#04x} (in big-endian: 0x{:02x})",
               fmt::join(context_->instruction_register, " "),
               fmt::join(context_->instruction_register |
                             auxilia::ranges::views::invert_endianness,
                         ""));
  decode();
  return {};
}
auxilia::Status CentralProcessingUnit::decode() {
  // TODO()
  if (std::ranges::equal(context_->instruction_register, isa::signal::trap)) {
    this->send(Event::kTaskFinished);
  }

  return {};
}
auxilia::StatusOr<std::span<const std::byte>> CentralProcessingUnit::fetch() {
  auto translated = mmu_.virtual_to_physical(context_->program_counter);
  // if (auto res = this->monitor()->fetch_from_main_memory(translated, num)) {
  //   return {mmu_.physical_to_virtual(*res)};
  // } else {
  //   return {res.as_status()};
  // }
  return this->monitor()
      ->fetch_from_main_memory(translated, isa::instruction_size_bytes)
      .and_then([&](auto &&res) { return mmu_.physical_to_virtual(res); });
}
Monitor *CentralProcessingUnit::monitor() const noexcept {
  precondition(dynamic_cast<Monitor *>(mediator), "Parent must be a Monitor")

  return static_cast<Monitor *>(mediator);
}
} // namespace accat::luce
