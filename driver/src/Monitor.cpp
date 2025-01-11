#include "accat/auxilia/details/format.hpp"
#include "deps.hh"

#include <fmt/color.h>
#include <fmt/xchar.h>
#include <spdlog/spdlog.h>
#include <accat/auxilia/details/Status.hpp>
#include <accat/auxilia/details/macros.hpp>
#include <functional>
#include <luce/Monitor.hpp>
#include <utility>

namespace accat::luce {
using namespace fmt::literals;
using enum fmt::color;
using fmt::fg;
auxilia::Status Monitor::notify(Component *sender, Event event) {
  // todo: implement
  switch (event) {
  case Event::kNone:
    spdlog::warn("nothing to do");
    break;
  case Event::kTaskFinished: {
    spdlog::info("Hit good ol' {:x}, program finished!",
                 fmt::join(isa::signal::trap, ""));
    // currently the sender here was surely a CPU, so we cast it
    auto cpu = static_cast<CentralProcessingUnit *>(sender);
    defer { cpu->detach_context(); };
    // find the task and mark it as terminated
    if (process.id() != cpu->task_id()) {
      spdlog::error(
          "Task id mismatch: {lpid} != {rpid}; should not happen; currently "
          "we just have excatly one task",
          "lpid"_a = process.id(),
          "rpid"_a = cpu->task_id());
      dbg_break
      return auxilia::InternalError("Task id mismatch");
    }
    process.state = Task::State::kTerminated;
    break;
  }
  default:
    spdlog::warn("Unhandled event: {}", static_cast<uint8_t>(event));
    dbg_break
  }
  return {};
}
auxilia::Status Monitor::REPL() {
  precondition(process.state == Task::State::kNew,
               "No program loaded or the program has already running")

  fmt::println("{}", message::repl::Welcome);

  for (;;) {
    if (auto [res, elapsed] = timer.measure(std::bind(&Monitor::shuttle, this));
        !res) {
      if (res.code() == auxilia::Status::Code::kReturning) {
        return {};
      }
      // error, return as is
      return res;
    }
  }
}
auxilia::Status Monitor::shuttle() {
  auto maybe_input = read();
  if (!maybe_input) {
    return maybe_input.as_status();
  }
  auto input = *std::move(maybe_input);

  if (auto res = inspect(input); !res) {
    return res;
  }

  return {};
}
auxilia::Status Monitor::inspect(const std::string_view input) {
  if (input == "exit") {
    fmt::println("Goodbye!");
    return auxilia::ReturnMe("exit!");
  }
  if (input == "help") {
    fmt::print(message::repl::Help);
    return {};
  }
  if (input == "c") {
    return execute_n(1);
  }
  if (input.starts_with("si")) {
    auto maybe_steps = scn::scan<size_t>(input, "si [{}]");
    if (maybe_steps.has_value()) {
      auto [steps] = std::move(maybe_steps)->values();
      return execute_n(steps);
    }
    auxilia::println(stderr,
                     fg(crimson),
                     "luce: error: {err_msg}",
                     "err_msg"_a = maybe_steps.error().msg());
    return {};
  }
  auxilia::println(stderr, fg(crimson), "luce: unknown command '{}'", input);
  return {};
}
auxilia::StatusOr<auxilia::string> Monitor::read() {
  for (;;) {
    std::string input;
    fmt::print(stdout, fg(cyan), "(luce) ");

    if (!std::getline(std::cin, input)) {
      if (std::cin.eof()) {
        fmt::println("\nGoodbye!");
        return {auxilia::ReturnMe("End of input")};
      }

      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auxilia::println(stderr, fg(crimson), "Input error, please try again");
        continue;
      }
      contract_assert(0, "unreachable")
      continue;
    }
    // clang-format off
    // trim input
    auto trimmed_input = input 
        | std::ranges::views::drop_while(auxilia::isspacelike) 
        | std::ranges::views::reverse 
        | std::ranges::views::drop_while(auxilia::isspacelike) 
        | std::ranges::views::reverse 
        | std::ranges::to<std::string>()
    ;
    // clang-format on
    if (trimmed_input.empty())
      continue;
    return {std::move(trimmed_input)};
  }
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
Monitor::_do_run_new_task_unchecked(const std::span<const std::byte> bytes,
                                    paddr_t start_addr,
                                    paddr_t block_size) {
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
  cpus.attach_context(process.context(), process.id());
  return {};
}
} // namespace accat::luce
