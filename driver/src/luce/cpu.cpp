#include "deps.hh"


#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cstddef>
#include <ranges>
#include <span>
#include <type_traits>

#include "luce/cpu/cpu.hpp"
#include "luce/Monitor.hpp"
#include "Support/isa/riscv32/isa.hpp"

namespace accat::luce {
using auxilia::Status;
using auxilia::StatusOr;
using CPU = CentralProcessingUnit;
using enum CPU::State;

CPU::CentralProcessingUnit(Mediator *parent) : Component(parent), mmu_(this) {}

auto CPU::detach_context() noexcept -> CPU & {
  if (context_.use_count() == 1) {
    spdlog::warn("Seems like the context is in a broken state, destroying...");
  }
  context_.reset();
  task_id_.reset();
  state_ = kVacant;
  return *this;
}
Status CPU::execute_shuttle() {
  precondition(context_, "No program to execute")

  auto executeShuttle = [&]() {
    state_ = kRunning;
    defer {
      state_ = kVacant;
    };
    return shuttle();
  };
  auto [res, elapsed] = cpu_timer_.measure(executeShuttle);
  spdlog::info("CPU execution time: {} ms", elapsed);
  return res;
}
Status CPU::shuttle() {
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
                             auxilia::ranges::views::swap_endian,
                         ""));
  decode();
  execute();
  return {};
}
Status CPU::decode() {
  // TODO(todo: decode the instruction and perform the operation)
  if (std::ranges::equal(context_->instruction_register, isa::signal::trap))
  {
    this->send(Event::kTaskFinished, [this]() { this->detach_context(); });
  }

  return {};
}
auxilia::Status CentralProcessingUnit::execute() {
  // TODO()
  return {};
}
auto CPU::fetch() -> StatusOr<std::span<const std::byte>> {
  return static_cast<Monitor *>(this->mediator)
      ->memory()
      .read_n(mmu_.virtual_to_physical(context_->program_counter),
              isa::instruction_size_bytes)
      .and_then([&](auto &&res) { // convert back
        return mmu_.physical_to_virtual(res);
      });
}
} // namespace accat::luce
