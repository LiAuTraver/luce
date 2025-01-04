#include "deps.hh"

#include "luce/cpu.hpp"
#include <spdlog/spdlog.h>
#include <type_traits>
#include "luce/Monitor.hpp"
#include "luce/Pattern.hpp"
namespace accat::luce {
CentralProcessingUnit::CentralProcessingUnit(Mediator *parent)
    : Component(parent) {
  if (parent) {
    contract_assert(dynamic_cast<Monitor *>(parent), "Parent must be a Monitor")
  }
  // else do nothing(allowed to be null)
}
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
    defer { state_ = State::kVacant; };
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
  spdlog::info("Fetched instruction: {:#04x}",
               fmt::join(context_->instruction_register, " "));
  // TODO()
  if (context_->instruction_register == isa::signal::trap) {
    this->send(Event::kTaskFinished);
  }
  return {};
}
auxilia::StatusOr<std::span<const std::byte>> CentralProcessingUnit::fetch() {
  return static_cast<Monitor *>(mediator)->fetch_from_main_memory(
      context_->program_counter,
      sizeof(instruction_t) / sizeof(isa::minimal_addressable_unit_t));
}
} // namespace accat::luce
