#include "deps.hh"

#include "luce/cpu/cpu.hpp"
#include "luce/Support/isa/IInstruction.hpp"
#include "luce/Support/isa/IDisassembler.hpp"
#include "luce/Monitor.hpp"
#include "luce/Support/isa/architecture.hpp"

namespace accat::luce {
using auxilia::Status;
using auxilia::StatusOr;
using CPU = CentralProcessingUnit;
using enum CPU::State;

CPU::CentralProcessingUnit(Mediator *parent) : Icpu(parent), mmu_(this) {}

CPU::~CentralProcessingUnit() = default;
auto CPU::detach_task() noexcept -> CPU & {
  // if (task_.use_count() == 1) {
  //   spdlog::warn("Seems like the context is in a broken state,
  //   destroying...");
  // }
  // task_.reset();
  state_ = kVacant;
  return *this;
}
Status CPU::execute_shuttle() {
  precondition(task_, "No program to execute")

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
  auto &ctx = task_->context();
  auto maybe_bytes = fetch(ctx.program_counter.num());
  if (!maybe_bytes) {
    return maybe_bytes.as_status();
  }
  ctx.program_counter.num() += sizeof(isa::instruction_size_t);
  auto bytes = std::move(maybe_bytes).value();
  auto &orig_bytes = ctx.instruction_register.bytes();
  for (const auto i : std::views::iota(0ull, sizeof(isa::instruction_size_t))) {
    orig_bytes[i] = bytes[i];
  }
  // convert little-endian to big-endian for more human-readable output
  spdlog::info("Fetched instruction: {:#04x} (in big-endian: 0x{:02x})",
               fmt::join(ctx.instruction_register.bytes(), " "),
               fmt::join(ctx.instruction_register.bytes() |
                             auxilia::ranges::views::swap_endian,
                         ""));
  return decode_and_execute();
}
Status CPU::decode_and_execute() {
  if (std::ranges::equal(task_->context().instruction_register.bytes(),
                         isa::signal::trap)) {
    task_->finish();
    this->detach_task();
    return {};
  }
  auto inst = monitor()->disassembler()->disassemble(
      task_->context().instruction_register.num());
  contract_assert(inst,
                  "Failed to decode the instruction."
                  " This assert is designed for debugging purposes. "
                  "Currently, we are not handling this case.");
  spdlog::info("Decoded instruction: {}", *inst);
  return execute(inst.get());
}
auxilia::Status CentralProcessingUnit::execute(isa::IInstruction *inst) {
  precondition(inst, "Instruction is nullptr");
  auto exec = inst->execute(this);
  using enum isa::IInstruction::ExecutionStatus;
  switch (exec) {
  case kSuccess:
    return {};
  case kMemoryViolation:
    spdlog::error("Memory violation detected. Pausing the task.");
    goto ebreak;
  case kInvalidInstruction:
    spdlog::error("Invalid instruction detected. Pausing the task.");
    goto ebreak;
  case kEnvBreak:
    goto ebreak;
  case kEnvCall:
    // TODO(...)
    spdlog::warn("Environment call detected, currently does nothing but resume "
                 "the task.");
    break;
  case kUnknown:
    spdlog::error("Unknown error occurred. Pausing the task.");
    goto ebreak;
  }
  return {};
ebreak:
  spdlog::info("received an environment break signal; pausing the task");
  task_->pause();
  return {};
}
auto CPU::fetch(const vaddr_t addr) const
    -> StatusOr<std::span<const std::byte>> {
  return monitor()
      ->memory()
      .read_n(mmu_.virtual_to_physical(addr),
              isa::instruction_size_bytes)
      .and_then([&](auto &&res) { // convert back
        return mmu_.physical_to_virtual(res);
      });
}
auto CPU::write(const vaddr_t addr, const std::span<const std::byte> bytes)
    -> auxilia::Status {
  return monitor()->memory().write_n(
      mmu_.virtual_to_physical(addr), isa::instruction_size_bytes, bytes);
}
auto CPU::monitor() const noexcept -> Monitor * {
  return static_cast<Monitor *>(this->mediator);
}

} // namespace accat::luce
