#include "deps.hh"

#include <fmt/color.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>
#include <functional>
#include <iostream>
#include <ostream>
#include <ranges>
#include <stacktrace>
#include <string>
#include <syncstream>
#include <utility>

#include <accat/auxilia/auxilia.hpp>

#include "luce/Monitor.hpp"
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

Status
Monitor::notify(Component *, Event event, std::function<void(void)> callback) {
  switch (event) {
  case kNone:
    spdlog::warn("nothing to do");
    break;
  case kTaskFinished: {
    defer {
      callback(); // cpu detaches context
    };
    spdlog::info(
        "Hit good ol' {trapBytes:x}, program finished!",
        "trapBytes"_a = fmt::join(
            isa::signal::trap | auxilia::ranges::views::swap_endian, ""));
    // find the task and mark it as terminated
    process.state = Task::State::kTerminated;
    break;
  }
  case kRestartOrResumeTask: {
    if (process.state == Task::State::kPaused) {
      spdlog::info("Resuming task");
      process.resume();
    } else {
      spdlog::info("Restarting task");
      process.restart();
      cpus.attach_context(process.context(), process.id());
    }
    break;
  }
  case kPrintWatchPoint:
    fmt::println("{}", debugger_.watchpoints());
    break;
  case kPauseTask:
    spdlog::info("Pausing task");
    process.pause();
    break;
  default:
    spdlog::warn("Unhandled event: {}", event::to_string_view(event));
    dbg_break
  }
  return {};
}
Status Monitor::run() {
  precondition(process.state == Task::State::kNew,
               "No program loaded or the program has already running")
  cpus.attach_context(process.context(), process.id());
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
  precondition(process.state == Task::State::kNew,
               "No program loaded or the program has already running")

  cpus.attach_context(process.context(), process.id());

  for (auto res : repl::repl(this) | std::views::common) {
    if (!res) {
      if (res.code() == Status::Code::kReturning) {
        return {};
      }
      // error, return as is
      spdlog::error("Error: {}", res.message());
      dbg(error, AC_UTILS_STACKTRACE);
      return res;
    }
  }

  spdlog::error("REPL exited unexpectedly. The program may be unresponsive.");
  std::osyncstream(std::cout) << std::flush;
  dbg_break
  return auxilia::InternalError("REPL exited unexpectedly");
}

Status Monitor::_do_execute_n_unchecked(const size_t steps) {
  // TODO: implement this
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
  if (process.state == Task::State::kPaused ||
      process.state == Task::State::kNew) {
    process.state = Task::State::kRunning;
  } else if (process.state == Task::State::kTerminated) {
    spdlog::info("Program has terminated. Press `r` to restart.");
    return {};
  }
  return _do_execute_n_unchecked(steps);
}
Status
Monitor::_do_register_task_unchecked(const std::span<const std::byte> bytes,
                                     const paddr_t start_addr,
                                     const paddr_t block_size) {
  return_if_not(memory_.load_program(bytes, start_addr, block_size))

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
