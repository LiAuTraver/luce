#include "deps.hh"

#include "luce/config.hpp"
#include "luce/Monitor.hpp"
#include "luce/repl/ExprVisitor.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"
#include "luce/repl/evaluation.hpp"
#include "luce/utils/Pattern.hpp"

namespace accat::luce {
using fmt::fg;
using enum fmt::color;
using namespace std::literals;
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
interface ICommand {
  virtual Status execute(Monitor * monitor) const = 0;
};

struct Unknown : /* extends */ auxilia::Monostate, /* implements */ ICommand {
  Unknown() = default;
  explicit Unknown(string_type cmd) : command(std::move(cmd)) {}
  virtual ~Unknown() override = default;

  string_type command;
  virtual Status execute(Monitor *) const override final {
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
    return monitor->notify(nullptr, Event::kRestartTask);
  }
};
struct Continue final : ICommand {
  virtual Status execute(Monitor *monitor) const override final {
    return monitor->execute_n(1);
  }
};
struct Step final : ICommand {
  Step() = default;
  explicit Step(size_t steps) : steps(steps) {}
  size_t steps = 1;
  virtual Status execute(Monitor *monitor) const override final {
    return monitor->execute_n(steps);
  }
};
struct AddWatchPoint final : ICommand {
  // TODO: implement expression AST and expr parser
  virtual Status execute(Monitor *) const override final {
    TODO(...)
  }
};
struct Info final : ICommand {
private:
  struct Registers final : ICommand {
    virtual Status execute(Monitor *) const override final {
      TODO(...)
    }
  };
  struct WatchPoints final : ICommand {
    virtual Status execute(Monitor *) const override final {
      TODO(...)
    }
  };

public:
  using InfoType = auxilia::Variant<Unknown, Registers, WatchPoints>;
  InfoType infoType;
  virtual Status execute(Monitor *monitor) const override final {
    return infoType.visit(match{
        [&](const auto &subCommand) -> Status {
          return subCommand.execute(monitor);
        },
    });
  }
};

struct Print final : ICommand {
  Print() = default;
  Print(std::string expr) : expression(std::move(expr)) {}
  virtual Status execute(Monitor * monitor) const override final {
    auto lexer = Lexer{};
    auto parser = Parser{lexer.load_string(expression).lex()};
    auto eval = expression::Evaluator{monitor};
    parser.next_expression()
        ->accept(eval)
        .transform([](auto &&res) {
          auxilia::println(stdout,
                           "{result}",
                           "result"_a = res.underlying_string(
                               auxilia::FormatPolicy::kBrief));
          return std::monostate{};
        })
        .transform_error([](auto &&res) {
          auxilia::println(stderr,
                           fg(crimson),
                           "luce: error: {msg}",
                           "msg"_a = res.message());
          return std::monostate{};
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
    return {InvalidArgumentError(
        fmt::format(fg(crimson), "exit: command does not take arguments"))};
  }
  if (mainCommand == "help") {
    if (it == input.end()) {
      return {Help{}};
    }
    return {InvalidArgumentError(
        fmt::format(fg(crimson), "help: command does not take arguments"))};
  }
  if (mainCommand == "r") {
    if (it == input.end()) {
      return {Restart{}};
    }
    return {InvalidArgumentError(
        fmt::format(fg(crimson), "r: command does not take arguments"))};
  }
  if (mainCommand == "c") {
    if (it == input.end()) {
      return {Continue{}};
    }
    return {InvalidArgumentError(
        fmt::format(fg(crimson), "c: command does not take arguments"))};
  }
  if (mainCommand == "si") {
    if (auto maybe_number =
            scn::scan<size_t>(input.substr(it - input.begin()), "[{}]")) {
      auto [steps] = std::move(maybe_number)->values();
      return {Step{steps}};
    }
    return {InvalidArgumentError(
        fmt::format(fg(crimson), "si: requires [number] as argument"))};
  }
  if (mainCommand == "p") {
    return {Print{input.substr(it - input.begin()).data()}};
  }

  if (mainCommand == "info") {
    // if (it == input.end()) {
    //   // return {Info{Info::Registers{}}};
    // }
    // if (auto subCommand = input.substr(it - input.begin());
    //     subCommand == "r") {
    //   // return {Info{Info::Registers{}}};
    // }
    // if (subCommand == "w") {
    //   // return {Info{Info::WatchPoints{}}};
    TODO(...)
  }

  if (mainCommand == "w") {
    // watchpoint: w EXPRESSION
    return {AddWatchPoint{}};
  }

  if (mainCommand == "d") {
    // delete watchpoint
    if (auto maybe_watchpoint =
            scn::scan_int<size_t>(input.substr(it - input.begin()))) {
      auto [watchpoint] = std::move(maybe_watchpoint)->values();
      return {Unknown{std::string(input)}};
    }
    return {InvalidArgumentError(
        fmt::format(fg(crimson), "d: requires 'number' as argument"))};
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
    if (auto res = command::inspect(input); res) {
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
