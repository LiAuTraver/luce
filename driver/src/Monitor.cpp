#include "deps.hh"

#include <fmt/color.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>
#include "accat/auxilia/details/views.hpp"
#include <accat/auxilia/details/Status.hpp>
#include <accat/auxilia/details/macros.hpp>
#include <functional>
#include "luce/Monitor.hpp"
#include <iostream>
#include <ostream>
#include <ranges>
#include <string>
#include <syncstream>
#include <utility>

namespace accat::luce::inline tr2::repl{
extern auto repl(Monitor *)
    -> ::accat::auxilia::Generator<::accat::auxilia::Status>;
}

namespace accat::luce {
using namespace std::literals;
using namespace fmt::literals;
using enum fmt::color;
using fmt::fg;
auxilia::Status Monitor::notify(Component *sender, Event event) {
  switch (event) {
  case Event::kNone:
    spdlog::warn("nothing to do");
    break;
  case Event::kTaskFinished: {
    spdlog::info(
        "Hit good ol' {trapBytes:x}, program finished!",
        "trapBytes"_a = fmt::join(
            isa::signal::trap | auxilia::ranges::views::invert_endianness, ""));
    // currently the sender here was surely a CPU, so we cast it
    auto cpu = static_cast<CentralProcessingUnit *>(sender);
    defer {
      cpu->detach_context();
    };
    dbg_block
    {
      if (process.id() != cpu->task_id()) {
        spdlog::error(
            "Task id mismatch: {lpid} != {rpid}; should not happen; currently "
            "we just have excatly one task",
            "lpid"_a = process.id(),
            "rpid"_a = cpu->task_id());
        dbg_break
        // return auxilia::InternalError("Task id mismatch");
      }
    };
    // find the task and mark it as terminated
    process.state = Task::State::kTerminated;
    break;
  }
  case Event::kRestartTask: {
    precondition(sender == nullptr,
                 "currently no sender is expected to send "
                 "this event, except from the REPL")
    spdlog::info("Restarting task");
    process.restart();
    cpus.attach_context(process.context(), process.id());
    break;
  }
  default:
    spdlog::warn("Unhandled event: {}", static_cast<uint8_t>(event));
    dbg_break
  }
  return {};
}
auxilia::Status Monitor::run() {
  precondition(process.state == Task::State::kNew,
               "No program loaded or the program has already running")
  cpus.attach_context(process.context(), process.id());
  while (process.state != Task::State::kTerminated) {
    if (auto res = cpus.execute_shuttle(); !res) {
      spdlog::error("Error: {}", res.message());
      dbg(error, res.stacktrace());
      return res;
    }
  }
  return {};
}
auxilia::Status Monitor::REPL() {
  precondition(process.state == Task::State::kNew,
               "No program loaded or the program has already running")

  cpus.attach_context(process.context(), process.id());

  auto replCoro = tr2::repl::repl(this);
  for (auto res : replCoro | std::views::common) {
    if (!res) {
      if (res.code() == auxilia::Status::Code::kReturning) {
        return {};
      }
      // error, return as is
      spdlog::error("Error: {}", res.message());
      dbg(error, res.stacktrace());
      return res;
    }
  }

  spdlog::error("REPL exited unexpectedly. The program may be unresponsive.");
  std::osyncstream(std::cout) << std::flush;
  dbg_break
  return auxilia::InternalError("REPL exited unexpectedly");
}

auxilia::Status Monitor::_do_execute_n_unchecked(const size_t steps) {
  // TODO: implement this
  for ([[maybe_unused]] const auto _ : std::views::iota(0ull, steps)) {
    if (process.state == Task::State::kTerminated) {
      spdlog::info("Program has terminated.");
      return {};
    }
    if (auto res = cpus.execute_shuttle(); !res)
      return res;
  }
  return {};
}
auxilia::Status Monitor::execute_n(const size_t steps) {
  if (process.state == Task::State::kPaused ||
      process.state == Task::State::kNew) {
    process.state = Task::State::kRunning;
  } else if (process.state == Task::State::kTerminated) {
    spdlog::info("Program has terminated. Press `r` to restart.");
    return {};
  }
  return _do_execute_n_unchecked(steps);
}
auxilia::Status
Monitor::_do_register_task_unchecked(const std::span<const std::byte> bytes,
                                     const paddr_t start_addr,
                                     const paddr_t block_size) {
  return_if_not(memory.load_program(bytes, start_addr, block_size))

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
