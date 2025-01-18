#include "deps.hh"

#include "luce/config.hpp"
#include "luce/replWatcher.hpp"
#include "luce/Monitor.hpp"
#include <fmt/color.h>
#include "accat/auxilia/details/Status.hpp"
#include "luce/utils/Pattern.hpp"
#include <string>

namespace accat::luce {
using fmt::fg;
using enum fmt::color;
auxilia::StatusOr<std::string> replWatcher::read() {
  std::string input;
  for (;;) {
    std::string raw_input;
    if (input.empty())
      fmt::print(stdout, fg(cyan), "(luce) ");
    else
      fmt::print(stdout, fg(cyan), ">>> ");

    if (!std::getline(std::cin, raw_input)) {
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
    auto trimmed_input = raw_input 
        | std::ranges::views::drop_while(auxilia::isspacelike) 
        | std::ranges::views::reverse 
        | std::ranges::views::drop_while(auxilia::isspacelike) 
        | std::ranges::views::reverse 
        | std::ranges::to<std::string>()
    ;
    // clang-format on
    if (trimmed_input.empty() && input.empty())
      continue;
    else if (trimmed_input.starts_with('#'))
      continue;
    else if (trimmed_input.ends_with('\\')) {
      trimmed_input.pop_back();
      input += std::move(trimmed_input);
      continue;
    }
    input += std::move(trimmed_input);
    return {std::move(input)};
  }
}
auxilia::Status replWatcher::inspect(std::string_view input) {
  if (input == "exit" or input == "q") {
    return auxilia::ReturnMe("exit!");
  }
  if (input == "help") {
    fmt::print(message::repl::Help);
    return {};
  }
  if (input == "r") {
    return this->send(Event::kRestartTask);
  }

  if (input == "c") {
    return this->monitor()->execute_n(1);
  }
  if (input.starts_with("si")) {
    auto maybe_steps = scn::scan<size_t>(input, "si [{}]");
    if (maybe_steps.has_value()) {
      auto [steps] = std::move(maybe_steps)->values();
      return this->monitor()->execute_n(steps);
    }
    auxilia::println(stderr,
                     fg(crimson),
                     "luce: error: {msg}",
                     "msg"_a = maybe_steps.error().msg());
    return {};
  }
  auxilia::println(stderr,
                   fg(crimson),
                   "luce: unknown command '{inputCmd}'",
                   "inputCmd"_a = input);
  return {};
}
Monitor *replWatcher::monitor() const noexcept {
  return static_cast<Monitor *>(mediator);
}
} // namespace accat::luce
