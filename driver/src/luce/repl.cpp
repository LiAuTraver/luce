#include "deps.hh"

#include <fmt/ostream.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <charconv>
#include <iterator>
#include <ranges>
#include <string>

#include "luce/config.hpp"
#include "luce/repl/repl_fwd.hpp"
#include "luce/repl/evaluation.hpp"
#include "luce/repl/ExprVisitor.hpp"
#include "luce/Monitor.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"
#include "luce/Support/utils/Pattern.hpp"

namespace accat::luce {
using fmt::fg;
using enum fmt::color;
using auxilia::InvalidArgumentError;
using auxilia::match;
using auxilia::Monostate;
using auxilia::Status;
using auxilia::StatusOr;
} // namespace accat::luce
namespace accat::luce::repl {
auto repl(Monitor *) -> auxilia::Generator<Status>;
}
namespace accat::luce::repl::command {
namespace {
auto read(Monitor *) -> StatusOr<std::string>;

StatusOr<std::string> read(Monitor *) {
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
      dbg_break
      continue;
    }
    // clang-format off
    auto trimmed_input = auxilia::ranges::trim(raw_input);
      ;
    // clang-format on
    if (trimmed_input.empty() && input.empty())
      continue;
    else if (trimmed_input.starts_with('#'))
      continue;
    else if (trimmed_input.ends_with('\\')) {
      trimmed_input.remove_suffix(1);
      input += std::move(trimmed_input);
      continue;
    }
    input += std::move(trimmed_input);
    return {std::move(input)};
  }
}
} // namespace
struct ICommand {
  virtual Status execute(Monitor *monitor) const = 0;

protected:
  virtual ~ICommand() = default;
};

struct Unknown : /* extends */ auxilia::Monostate, /* implements */ ICommand {
  Unknown() = default;
  explicit Unknown(string_type cmd) : command(std::move(cmd)) {}
  virtual ~Unknown() = default;

  string_type command;
  virtual Status execute(Monitor *) const override final {
    if (command.empty())
      auxilia::println(
          stderr, fg(crimson), "luce: no command or subcommand provided");
    else
      auxilia::println(stderr,
                       fg(crimson),
                       "luce: unknown command '{cmd}'. Type "
                       "'help' for more information.",
                       "cmd"_a = command);
    return {};
  }
};
struct Help final : ICommand {
  virtual Status execute(Monitor *) const override final {
    fmt::print(message::repl::Help);
    return {};
  }
};
struct Exit final : ICommand {
  virtual Status execute(Monitor *) const override final {
    return auxilia::ReturnMe("exit!");
  }
};
struct Restart final : ICommand {
  virtual Status execute(Monitor *monitor) const override final {
    return monitor->notify(nullptr, Event::kRestartOrResumeTask);
  }
};
struct Continue final : ICommand {
  virtual Status execute(Monitor *monitor) const override final {
    monitor->execute_n(1);
    return {};
  }
};
struct Step final : ICommand {
  Step() = default;
  explicit Step(size_t steps) : steps(steps) {}
  size_t steps = 1;
  virtual Status execute(Monitor *monitor) const override final {
    monitor->execute_n(steps);
    return {};
  }
};
struct AddWatchPoint final : ICommand {
  AddWatchPoint() = default;
  explicit AddWatchPoint(std::string expr) : expression(std::move(expr)) {}
  virtual Status execute(Monitor *monitor) const override final {
    monitor->debugger().add_watchpoint(expression);
    return {};
  }
  std::string expression;
};
struct DeleteWatchPoint final : ICommand {
  DeleteWatchPoint() = default;
  explicit DeleteWatchPoint(size_t id) : watchpointId(id) {}
  size_t watchpointId = 0;
  virtual Status execute(Monitor *monitor) const override final {
    if (auto res = monitor->debugger().delete_watchpoint(watchpointId)) {
      fmt::println("Deleted watchpoint {}", watchpointId);
    } else {
      spdlog::error("Error: {}", res.message());
    }
    return {};
  }
};
struct Info final : ICommand {
private:
  struct Registers final : ICommand {
    virtual Status execute(Monitor *monitor) const override final {
      auxilia::println(stdout, "{regs}", "regs"_a = *monitor->registers());
      return {};
    }
  };
  struct WatchPoints final : ICommand {
    virtual Status execute(Monitor *monitor) const override final {
      return monitor->notify(nullptr, Event::kPrintWatchPoint);
    }
  };

public:
  Info() = default;
  explicit Info(const std::string &subCommand) {
    auto trimmed = auxilia::ranges::trim(subCommand);
    if (trimmed.empty()) {
      infoType.emplace(Unknown{subCommand});
      return;
    } else if (trimmed == "r" or trimmed == "registers") {
      infoType.emplace(Registers{});
    } else if (trimmed == "w" or trimmed == "watchpoints") {
      infoType.emplace(WatchPoints{});
    } else {
      infoType.emplace(Unknown{{trimmed.begin(), trimmed.end()}});
    }
  }

public:
  using InfoType = auxilia::Variant<Unknown, Registers, WatchPoints>;
  InfoType infoType;
  virtual Status execute(Monitor *monitor) const override final {
    return infoType.visit([&](const auto &subCommand) -> Status {
      return subCommand.execute(monitor);
    });
  }
};

struct Print final : ICommand {
  Print() = default;
  Print(std::string expr) : expression(std::move(expr)) {}
  virtual Status execute(Monitor *monitor) const override final {
    auto lexer = Lexer{};
    auto parser = Parser{lexer.load_string(expression).lex()};
    auto eval = expression::Evaluator{monitor};
    parser.next_expression()
        .transform([&](auto &&res1) {
          res1->accept(eval)
              .transform([](auto &&res2) {
                auxilia::println(stdout,
                                 "{result}",
                                 "result"_a = res2.underlying_string(
                                     auxilia::FormatPolicy::kBrief));
              })
              .transform_error([](auto &&res2) {
                auxilia::println(stderr,
                                 fg(crimson),
                                 "luce: error: {msg}",
                                 "msg"_a = res2.message());
              });
        })
        .transform_error([](auto &&res1) {
          auxilia::println(stderr,
                           fg(crimson),
                           "luce: error: {msg}",
                           "msg"_a = res1.message());
        });
    return {};
  }
  std::string expression;
};

using command_t = auxilia::Variant<Unknown,
                                   Help,
                                   Exit,
                                   Restart,
                                   Continue,
                                   Step,
                                   AddWatchPoint,
                                   DeleteWatchPoint,
                                   Info,
                                   Print>;

StatusOr<command_t> inspect(std::string_view input) {
  // extract out the first command
  auto it = std::ranges::find_if(input, auxilia::isspacelike);
  std::string_view mainCommand;
  if (it == input.end()) {
    // input itself is the command with no arguments
    mainCommand = input;
  } else {
    mainCommand = input.substr(0, it - input.begin());
  }
  if (mainCommand == "exit" or mainCommand == "q") {
    if (it == input.end()) {
      return {Exit{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "exit: command does not take arguments")};
  }
  if (mainCommand == "help") {
    if (it == input.end()) {
      return {Help{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "help: command does not take arguments")};
  }
  if (mainCommand == "r") {
    if (it == input.end()) {
      return {Restart{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "r: command does not take arguments")};
  }
  if (mainCommand == "c") {
    if (it == input.end()) {
      return {Continue{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "c: command does not take arguments")};
  }
  if (mainCommand == "si") {
    if (auto args = auxilia::ranges::trim(input.substr(it - input.begin()));
        !args.empty() && args.front() == '[' && args.back() == ']') {
      if (auto maybe_steps =
              scn::scan_int<size_t>(args.substr(1, args.size() - 2))) {
        auto [steps] = std::move(maybe_steps)->values();
        return {Step{steps}};
      } else {
        return {InvalidArgumentError(fg(crimson),
                                     "si: error parsing number: {}",
                                     maybe_steps.error().msg())};
      }
    }

    return {
        InvalidArgumentError(fg(crimson), "si: requires [number] as argument")};
  }
  if (mainCommand == "p") {
    return {Print{input.substr(it - input.begin()).data()}};
  }

  if (mainCommand == "info") {
    return {Info{input.substr(it - input.begin()).data()}};
  }

  if (mainCommand == "w") {
    if (auto maybe_exprStr =
            auxilia::ranges::trim(input.substr(it - input.begin()));
        !maybe_exprStr.empty()) {
      return {AddWatchPoint{{maybe_exprStr.begin(), maybe_exprStr.end()}}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "w: requires 'expression' as argument")};
  }

  if (mainCommand == "d") {
    if (auto maybe_watchpoint =
            scn::scan_int<size_t>(input.substr(it - input.begin()))) {
      auto [watchpoint] = std::move(maybe_watchpoint)->values();
      return {DeleteWatchPoint{watchpoint}};
    }
    return {
        InvalidArgumentError(fg(crimson), "d: requires 'number' as argument")};
  }

  return {Unknown{std::string(input)}};
}
} // namespace accat::luce::repl::command
namespace accat::luce::repl {
auto repl(Monitor *monitor) -> auxilia::Generator<Status> {
  precondition(monitor, "Monitor must not be nullptr")

  fmt::println("{}", message::repl::Welcome);

  for (;;) {
    auto maybe_input = command::read(monitor);
    if (!maybe_input) {
      co_yield {maybe_input.as_status()};
    }
    auto input = *std::move(maybe_input);
    if (auto res = command::inspect(input)) {
      co_yield res->visit(match{
          [&](const auto &cmd) -> Status { return cmd.execute(monitor); },
      });
    } else { // no command found or invalid command
      auxilia::println(stderr,
                       "{error} {msg}",
                       "error"_a = fmt::format(fg(crimson), "error:"),
                       "msg"_a = res.message());
      co_yield {};
    }
  }
  co_return;
}
} // namespace accat::luce::repl
