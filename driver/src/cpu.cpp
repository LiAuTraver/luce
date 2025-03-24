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
  spdlog::trace("CPU execution time: {} ms", elapsed);
  return res;
}
Status CPU::shuttle() {
  auto &ctx = task_->context();
  auto maybe_bytes = fetch(ctx.program_counter.num());
  if (!maybe_bytes) {
    return maybe_bytes.as_status();
  }
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
  auto inst = monitor()->disassembler()->disassemble(
      task_->context().instruction_register.num());
  if (!inst) {
    spdlog::error("Failed to decode the instruction.");
    return trap();
  }
  spdlog::info("Decoded instruction: {}", *inst);
  return execute(inst.get());
}
auxilia::Status CentralProcessingUnit::execute(isa::IInstruction *inst) {
  auto exec = inst->execute(this);
  using enum isa::IInstruction::ExecutionStatus;
  switch (exec) {
  case kSuccess:
    task_->context().advance_pc();
    [[fallthrough]];
  case kSuccessAndNotAdvancePC:
    return {};
  case kMemoryViolation:
    spdlog::error("Memory violation detected. Pausing the task.");
    break;
  case kInvalidInstruction:
    spdlog::error("Invalid instruction detected. Pausing the task.");
    break;
  case kEnvBreak:
    // TODO: pc.
    spdlog::info("received an environment break signal; pausing the task");
    // temporary solution
    task_->context().advance_pc();
    break;
  case kEnvCall:
    // TODO(...)
    // advancing the PC after the call, todo.
    spdlog::warn("Environment call detected, currently does nothing but resume "
                 "the task. this is a TODO.");
    // temporary solution
    handle_syscall();
    task_->context().advance_pc();
    return {};
  case kUnknown:
    spdlog::error("Unknown error occurred. Pausing the task.");
    break;
  }
  return trap();
}
auto CentralProcessingUnit::trap() -> Status {
  if (std::ranges::equal(task_->context().instruction_register.bytes(),
                         isa::signal::deadbeef))
    spdlog::info("Hit good ol' deadbeef, pausing the task.");

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
auto CPU::handle_syscall() -> auxilia::Status {
  auto &gpr = *task_->context().general_purpose_registers();

  // Get syscall number from a7, which is the 18th register
  const auto syscall_num = gpr[17];
  std::array<uint32_t, 7> args;
  for (const auto i : std::views::iota(0, 7)) {
    args[i] = gpr[i + 10];
  }

  dbg(info, "Syscall number: {}", syscall_num);
  dbg(info, "Arguments: {}", fmt::join(args, ", "));

  // Handle different syscall numbers
  switch (syscall_num) {
  case 1:               // SYS_write
    if (args[0] == 1) { // stdout
      // Write to stdout, args[1] is buffer pointer, args[2] is length
      monitor()
          ->memory()
          .read_n(mmu_.virtual_to_physical(args[1]), args[2])
          .transform([&](auto &&res) {
            fmt::println("[stdout]{}", fmt::join(res, " "));
            gpr.write_at(10) = args[2];
          })
          .transform_error([&](auto &&err) {
            dbg(error, "Failed to read from memory: {}", err.message());
            gpr.write_at(10) = -1;
          });
    }
    break;

  case 2: // SYS_open
    // args[0]: pathname pointer, args[1]: flags, args[2]: mode
    spdlog::warn("SYS_open not fully implemented");
    gpr.write_at(10) = -1; // Return error for now
    break;

  case 3: // SYS_close
    // args[0]: file descriptor
    spdlog::warn("SYS_close not fully implemented"); 
    gpr.write_at(10) = 0; // Pretend success
    break;

  case 4: // SYS_stat
    // args[0]: pathname pointer, args[1]: stat buffer pointer
    spdlog::warn("SYS_stat not fully implemented");
    gpr.write_at(10) = -1;
    break;

  case 63: // SYS_read
    // args[0]: fd, args[1]: buffer pointer, args[2]: count
    if (args[0] == 0) { // stdin
      spdlog::warn("stdin read not implemented");
      gpr.write_at(10) = -1;
    }
    break;

  case 64: // SYS_write 
    if (args[0] == 1 || args[0] == 2) { // stdout/stderr
      monitor()
          ->memory()
          .read_n(mmu_.virtual_to_physical(args[1]), args[2])
          .transform([&](auto &&res) {
            fmt::println("[{}]{}", args[0] == 1 ? "stdout" : "stderr", 
                       fmt::join(res, " "));
            gpr.write_at(10) = args[2];
          })
          .transform_error([&](auto &&err) {
            dbg(error, "Failed to read from memory: {}", err.message());
            gpr.write_at(10) = -1;
          });
    }
    break;

  case 80: // SYS_fstat
    // args[0]: fd, args[1]: stat buffer pointer
    spdlog::warn("SYS_fstat not fully implemented");
    gpr.write_at(10) = -1;
    break;

  case 93: // SYS_exit
    spdlog::info("Program exited with code: {}", args[0]);
    task_->finish();
    break;

  case 214: // SYS_brk
    // args[0]: new program break
    spdlog::warn("SYS_brk not fully implemented");
    gpr.write_at(10) = args[0]; // Pretend success by returning requested address
    break;

  default:
    spdlog::warn("Unhandled syscall number: {}", syscall_num);
    gpr.write_at(10) = -1; // return error
  }

  return {};
}
} // namespace accat::luce
