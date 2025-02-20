#include "deps.hh"

#include <accat/auxilia/auxilia.hpp>

#include "luce/Monitor.hpp"
#include "luce/Support/isa/riscv32/Decoder.hpp"
#include "luce/repl/evaluation.hpp"
#include "luce/Task.hpp"
#include "luce/Support/utils/Pattern.hpp"

namespace accat::luce::repl {
extern auto repl(Monitor *)
    -> ::accat::auxilia::Generator<::accat::auxilia::Status>;
}

namespace accat::luce {
using namespace std::literals;
using namespace fmt::literals;
using enum fmt::color;
using enum Event;
using auxilia::Status;
using auxilia::StatusOr;
using fmt::fg;
namespace {
[[gnu::cold]] auto Die() {
  spdlog::error("REPL exited unexpectedly. The program may be unresponsive.");
  // flush the output(use osyncstream to avoid interleaving since the program
  // may be unresponsive)
  std::osyncstream(std::cout) << std::flush;
  dbg_break
  return auxilia::InternalError("REPL exited unexpectedly");
}
} // namespace
Monitor::Monitor() : memory_(this), cpus(this), debugger_(this) {
  // default just use riscv32
  disassembler_ = std::make_shared<isa::riscv32::Disassembler>();
  disassembler_->initializeDefault();
}
Monitor::~Monitor() = default;
auto Monitor::notify(Component *,
                     Event event,
                     std::function<void(void)> callback) -> Status {
  defer {
    if (callback)
      callback();
  };
  switch (event) {
  case kRestartOrResumeTask: {
    if (process.state == Task::State::kPaused) {
      process.resume();
    } else {
      process.restart();
      cpus.attach_task(&process);
    }
    break;
  }
  case kPauseTask:
    // pause all tasks
    process.pause();
    break;
  default:
    spdlog::warn("Unhandled event: {}", static_cast<uint8_t>(event));
    dbg_break
  }
  return {};
}
Status Monitor::run() {
  process.start();
  cpus.attach_task(&process);
  while (process.state != Task::State::kTerminated) {
    if (auto res = cpus.execute_shuttle(); !res) {
      spdlog::error("Error: {}", res.message());
      dbg(error, AC_UTILS_STACKTRACE);
      return res;
    }
  }
  return {};
}
Status Monitor::REPL() {
  process.start();
  cpus.attach_task(&process);

  for (auto res : repl::repl(this) | std::views::common) {
    if (res)
      continue;

    // error, return as is
    spdlog::error("Error: {}", res.message());
    dbg(error, AC_UTILS_STACKTRACE);
    return res;
  }
  [[unlikely]] return Die();
}

Status Monitor::_do_execute_n_unchecked(const size_t steps) {
  for ([[maybe_unused]] auto _ : std::views::iota(0ull, steps)) {
    if (process.state == Task::State::kTerminated) {
      spdlog::info("Program has terminated.");
      return {};
    }
    if (process.state == Task::State::kPaused) {
      spdlog::info("Program is paused. Press `r` to resume.");
      return {};
    }
    if (auto res = cpus.execute_shuttle(); !res)
      return res;
    this->debugger_.update_watchpoints(true, true);
  }
  return {};
}
Status Monitor::execute_n(const size_t steps) {
  if (process.state == Task::State::kReady) {
    process.state = Task::State::kRunning;
  } else if (process.state == Task::State::kTerminated) {
    spdlog::info("Program has terminated. Press `r` to restart.");
    return {};
  }
  return _do_execute_n_unchecked(steps);
}
auto Monitor::_do_register_task_unchecked(
    const std::span<const std::byte> bytes,
    const paddr_t start_addr,
    const paddr_t block_size) -> Status {
  if (auto res = memory_.load_program(bytes, start_addr, block_size); !res)
    return res;

  process = Task(this);
  const auto startOfDynamicMemory =
      static_cast<vaddr_t>(start_addr + bytes.size());

  process.address_space = {
      .static_regions = {.text_segment = {.start = start_addr,
                                          .end = startOfDynamicMemory,
                                          .permissions = Permission::kRead |
                                                         Permission::kExecute},
                         .data_segment = {}}, // currently ignore data segment
      .dynamic_regions =
          {.stack = {.start = startOfDynamicMemory,
                     .end = (startOfDynamicMemory + 0x1000), // 4KB stack
                     .permissions = Permission::kRead | Permission::kWrite},
           .heap_break = start_addr + block_size,
           .heap = {}},
      .mapped_regions = {}}; // currently ignore mapped regions
  return {};
}
} // namespace accat::luce
