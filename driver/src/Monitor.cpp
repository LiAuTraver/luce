#include "deps.hh"

#include <luce/Monitor.hpp>

namespace accat::luce {
auxilia::Status Monitor::REPL() {
  std::cout << repl::WelcomeMessage << '\n';
  precondition(process.state == Task::State::kNew,
               "No program loaded or the program has already running")

  auto executeShuttle = [&]() { return shuttle(); };

  for (;;) {
    if (auto [res, elapsed] = timer.measure(executeShuttle); !res) {
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
  auto input = std::move(maybe_input).value();

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
    fmt::print(repl::HelpMessage);
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
    fmt::print(stderr,
               fg(fmt::color::red),
               "luce: error: {}\n",
               maybe_steps.error().msg());
    return {};
  }
  fmt::print(
      stderr, fg(fmt::color::red), "luce: unknown command '{}'\n", input);
  return {};
}
auxilia::StatusOr<auxilia::string> Monitor::read() {
  for (;;) {
    std::string input;
    fmt::print(stdout, fmt::fg(fmt::color::dark_cyan), "(luce) ");

    if (!std::getline(std::cin, input)) {
      if (std::cin.eof()) {
        fmt::println("\nGoodbye!");
        return {auxilia::ReturnMe("End of input")};
      }

      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        fmt::print(stderr,
                   fmt::fg(fmt::color::red),
                   "Input error, please try again\n");
        continue;
      }
      contract_assert(0, "unreachable")
      continue;
    }
    // clang-format off
    // trim input
    auto input_view = input 
        | std::ranges::views::drop_while(auxilia::isspacelike) 
        | std::ranges::views::reverse 
        | std::ranges::views::drop_while(auxilia::isspacelike) 
        | std::ranges::views::reverse 
        | std::ranges::to<std::string>()
    ;
    // clang-format on
    if (input_view.empty())
      continue;
    return input_view;
  }
}
auxilia::Status Monitor::execute_n(const size_t steps) {
  if (process.state == Task::State::kPaused ||
      process.state == Task::State::kNew) {
    process.state = Task::State::kRunning;
  } else if (process.state == Task::State::kTerminated) {
    spdlog::info("Program has terminated.");
    return {};
  }
  // TODO: implement this
  for (const auto step : std::views::iota(0ull, steps)) {
    if (auto res = cpus.execute_shuttle(); !res)
      return res;
  }
  return {};
}
} // namespace accat::luce
