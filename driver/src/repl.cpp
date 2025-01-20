#include "accat/auxilia/details/Monostate.hpp"
#include "accat/auxilia/details/config.hpp"
#include "accat/auxilia/details/format.hpp"
#include "deps.hh"

#include "luce/config.hpp"
#include "luce/Monitor.hpp"
#include <fmt/color.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <accat/auxilia/details/macros.hpp>
#include <accat/auxilia/details/Status.hpp>
#include <accat/auxilia/details/views.hpp>
#include "luce/utils/Pattern.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <queue>
#include <string>
#include <string_view>
#include <iostream>
#include <ranges>
#include <utility>

namespace accat::luce {
using fmt::fg;
using enum fmt::color;
using namespace std::literals;
namespace {
auxilia::StatusOr<std::string> read(Monitor *) {
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
        | auxilia::ranges::views::trim
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
} // namespace

inline namespace tr2 {
using auxilia::match;
namespace repl::command {
namespace {
interface ICommand {
  virtual auxilia::Status execute(Monitor * monitor) const = 0;
};

struct Unknown : /* extends */ auxilia::Monostate, /* implements */ ICommand {
  Unknown() = default;
  explicit Unknown(string_type cmd) : command(std::move(cmd)) {}
  virtual ~Unknown() override = default;

  string_type command;
  virtual auxilia::Status execute(Monitor *) const override final {
    auxilia::println(stderr,
                     fg(crimson),
                     "luce: unknown command '{cmd}'. Type "
                     "'help' for more information.",
                     "cmd"_a = command);
    return {};
  }
};
struct Help final : ICommand {
  virtual auxilia::Status execute(Monitor *) const override final {
    fmt::print(message::repl::Help);
    return {};
  }
};
struct Exit final : ICommand {
  virtual auxilia::Status execute(Monitor *) const override final {
    return auxilia::ReturnMe("exit!");
  }
};
struct Restart final : ICommand {
  virtual auxilia::Status execute(Monitor *monitor) const override final {
    return monitor->notify(nullptr, Event::kRestartTask);
  }
};
struct Continue final : ICommand {
  virtual auxilia::Status execute(Monitor *monitor) const override final {
    return monitor->execute_n(1);
  }
};
struct Step final : ICommand {
  Step() = default;
  explicit Step(size_t steps) : steps(steps) {}
  size_t steps = 1;
  virtual auxilia::Status execute(Monitor *monitor) const override final {
    return monitor->execute_n(steps);
  }
};
struct AddWatchPoint final : ICommand {
  // TODO: implement expression AST and expr parser
  virtual auxilia::Status execute(Monitor *) const override final {
    TODO(...)
  }
};
struct Info final : ICommand {
private:
  struct Registers final : ICommand {
    virtual auxilia::Status execute(Monitor *monitor) const override final {
      TODO(...)
    }
  };
  struct WatchPoints final : ICommand {
    virtual auxilia::Status execute(Monitor *monitor) const override final {
      TODO(...)
    }
  };

public:
  using InfoType = auxilia::Variant<Unknown, Registers, WatchPoints>;
  InfoType infoType;
  virtual auxilia::Status execute(Monitor *monitor) const override final {
    return infoType.visit(match{
        [&](const auto &subCommand) -> auxilia::Status {
          return subCommand.execute(monitor);
        },
    });
  }
};

using command_t = auxilia::
    Variant<Unknown, Help, Exit, Restart, Continue, Step, AddWatchPoint, Info>;

auxilia::StatusOr<command_t> inspect(std::string_view input) {
  if (input == "exit" or input == "q") {
    return {Exit{}};
  }
  if (input == "help") {
    return {Help{}};
  }
  if (input == "r") {
    return {Restart{}};
  }

  if (input == "c") {
    return {Continue{}};
  }
  if (input.starts_with("si")) {
    if (auto begin = input.find_first_of('['); begin != std::string::npos) {
      if (auto end = input.find_last_of(']'); end != std::string::npos) {
        auto shouldBeSpaceOrNumber = input.substr(begin + 1, end - begin - 1) |
                                     auxilia::ranges::views::trim;
        if (shouldBeSpaceOrNumber.empty()) {
          return {Step{1}};
        }
        auto steps = size_t{};
        auto shouldBeNumberStr =
            shouldBeSpaceOrNumber | std::ranges::to<std::string>();
        if (auto [p, ec] = std::from_chars(shouldBeNumberStr.data(),
                                           shouldBeNumberStr.data() +
                                               shouldBeNumberStr.size(),
                                           steps,
                                           10);
            ec == std::errc{}) {
          return {Step{steps}};
        } else {
          return auxilia::InvalidArgumentError(
              auxilia::format(fg(crimson),
                              "{msg}.",
                              "msg"_a = std::make_error_code(ec).message()));
        }
      }
      // else, fallthrough(error)
    }
    return auxilia::InvalidArgumentError(auxilia::format(
        fg(crimson),
        "please provide a number of steps to execute(e.g., si [2] to execute 2 "
        "steps)."));
  }
  if (input.starts_with("info")) {
    // info r -> show registers
    // info w -> show watchpoints (not implemented)
    TODO(...)
  }
  if (input.starts_with("x")) {
    // x
    TODO()
  }
  if (input.starts_with("d")) {
    // delete watchpoint
    auto maybe_watchpoint = scn::scan<size_t>(input, "d {}");
    if (maybe_watchpoint.has_value()) {
      auto [watchpoint] = std::move(maybe_watchpoint)->values();
    }
    TODO();
  }
  if (input.starts_with("w")) {
    // watchpoint: w EXPRESSION
    TODO()
  }

  return {Unknown{std::string(input)}};
}
} // namespace
} // namespace repl::command
namespace repl {
auto repl(Monitor *monitor) -> auxilia::Generator<auxilia::Status> {
  precondition(monitor, "Monitor must not be nullptr")

  fmt::println("{}", message::repl::Welcome);

  for (;;) {
    auto maybe_input = read(monitor);
    if (!maybe_input) {
      co_yield {maybe_input.as_status()};
    }
    auto input = *std::move(maybe_input);
    // if (auto res = inspect(input, monitor); !res) {
    //   co_yield res;
    // }
    // nothing to do
    if (auto res = command::inspect(input); res) {
      co_yield res->visit(match{
          [&](const auto &cmd) -> auxilia::Status {
            return cmd.execute(monitor);
          },
      });
    }
    // else, no command found
    else {
      auxilia::println(stderr,
                       "{error} {msg}",
                       "error"_a = fmt::format(fg(crimson), "error:"),
                       "msg"_a = res.message());
      co_yield {};
    }
  }
  co_return;
}
} // namespace repl
} // namespace tr2
/// @deprecated use tr2::repl instead
namespace tr1 {
namespace {
/// @deprecated use tr2::repl::command::inspect instead
auxilia::Status inspect(std::string_view input, Monitor *monitor) {
  if (input == "exit" or input == "q") {
    return auxilia::ReturnMe("exit!");
  }
  if (input == "help") {
    fmt::print(message::repl::Help);
    return {};
  }
  if (input == "r") {
    return monitor->notify(nullptr, Event::kRestartTask);
  }

  if (input == "c") {
    return monitor->execute_n(1);
  }
  if (input.starts_with("si")) {
    if (auto begin = input.find_first_of('['); begin != std::string::npos) {
      if (auto end = input.find_last_of(']'); end != std::string::npos) {
        auto shouldBeSpaceOrNumber = input.substr(begin + 1, end - begin - 1) |
                                     auxilia::ranges::views::trim;
        if (shouldBeSpaceOrNumber.empty()) {
          return monitor->execute_n(1);
        }
        auto steps = size_t{};
        auto shouldBeNumberStr =
            shouldBeSpaceOrNumber | std::ranges::to<std::string>();
        if (auto [p, ec] = std::from_chars(shouldBeNumberStr.data(),
                                           shouldBeNumberStr.data() +
                                               shouldBeNumberStr.size(),
                                           steps,
                                           10);
            ec == std::errc{}) {
          return monitor->execute_n(steps);

        } else {
          auxilia::println(stderr,
                           fg(crimson),
                           "luce: error: {msg}.",
                           "msg"_a = std::make_error_code(ec).message());
        }
        return {};
      }
      // else, fallthrough(error)
    }
    auxilia::println(stderr,
                     fg(crimson),
                     "please provide a number of steps to execute(e.g., si "
                     "[2] to execute 2 steps).");
    return {};
  }
  if (input.starts_with("info")) {
    // info r -> show registers
    // info w -> show watchpoints (not implemented)
    TODO(...)
  }
  if (input.starts_with("x")) {
    // x
    TODO()
  }
  if (input.starts_with("d")) {
    // delete watchpoint
    auto maybe_watchpoint = scn::scan<size_t>(input, "d {}");
    if (maybe_watchpoint.has_value()) {
      auto [watchpoint] = std::move(maybe_watchpoint)->values();
    }
    TODO();
  }
  if (input.starts_with("w")) {
    // watchpoint: w EXPRESSION
    TODO()
  }

  auxilia::println(stderr,
                   fg(crimson),
                   "luce: unknown command '{inputCmd}'",
                   "inputCmd"_a = input);
  return {};
}
} // namespace
/// @deprecated use tr2::repl::repl instead
auto repl(Monitor *monitor) -> auxilia::Generator<auxilia::Status, void> {
  precondition(monitor, "Monitor must not be nullptr")

  fmt::println("{}", message::repl::Welcome);

  for (;;) {
    auto maybe_input = read(monitor);
    if (!maybe_input) {
      co_yield {maybe_input.as_status()};
    }
    auto input = *std::move(maybe_input);
    if (auto res = inspect(input, monitor); !res) {
      co_yield res;
    }
    // nothing to do
    co_yield {};
  }
  co_return;
}
} // namespace tr1
} // namespace accat::luce
