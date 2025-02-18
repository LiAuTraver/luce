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
#include "luce/Support/isa/architecture.hpp"

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
  auto maybe_bytes = fetch(context_->program_counter);
  if (!maybe_bytes) {
    return maybe_bytes.as_status();
  }
  context_->program_counter += sizeof(instruction_t);
  auto bytes = std::move(maybe_bytes).value();
  auto &orig_bytes = context_->instruction_register.bytes();
  for (const auto i : std::views::iota(0ull, sizeof(instruction_t))) {
    orig_bytes[i] = bytes[i];
  }
  // convert little-endian to big-endian for more human-readable output
  spdlog::info("Fetched instruction: {:#04x} (in big-endian: 0x{:02x})",
               fmt::join(context_->instruction_register.bytes(), " "),
               fmt::join(context_->instruction_register.bytes() |
                             auxilia::ranges::views::swap_endian,
                         ""));
  return decode_and_execute();
}
Status CPU::decode_and_execute() {
  // TODO(todo: decode the instruction and perform the operation)
  if (std::ranges::equal(context_->instruction_register.bytes(),
                         isa::signal::trap)) {
    this->send(Event::kTaskFinished, [this]() { this->detach_context(); });
  }
  auto inst = isa::instruction::Factory::createInstruction(
      context_->instruction_register.num());
  contract_assert(inst,
                  "Failed to decode the instruction."
                  " This assert is designed for debugging purposes. "
                  "Currently, we are not handling this case.");
  return execute(inst.get());
}
auxilia::Status CentralProcessingUnit::execute(isa::Instruction *inst) {
  precondition(inst, "Instruction is nullptr");
  inst->execute(this);
  return {};
}
auto CPU::fetch(const vaddr_t addr) const
    -> StatusOr<std::span<const std::byte>> {
  return static_cast<Monitor *>(this->mediator)
      ->memory()
      .read_n(mmu_.virtual_to_physical(addr),
              isa::instruction_size_bytes)
      .and_then([&](auto &&res) { // convert back
        return mmu_.physical_to_virtual(res);
      });
}
auto CPU::write(const vaddr_t addr, const std::span<const std::byte> bytes)
    -> auxilia::Status {
  return static_cast<Monitor *>(this->mediator)
      ->memory()
      .write_n(
          mmu_.virtual_to_physical(addr), isa::instruction_size_bytes, bytes);
}

} // namespace accat::luce
