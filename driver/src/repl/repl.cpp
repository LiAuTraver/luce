#include "deps.hh"

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
using auxilia::Monostate;
using auxilia::Status;
using auxilia::StatusOr;
using auxilia::views::trim;
} // namespace accat::luce
namespace accat::luce::repl::command {
namespace {
std::string read(Monitor *) {
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
        std::exit(EXIT_SUCCESS);
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
    auto trimmed_input = trim(raw_input);
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
    return input;
  }
}
} // namespace
struct ICommand {
  virtual void execute(Monitor *monitor) const = 0;

protected:
  virtual ~ICommand() = default;
};

struct Unknown : /* extends */ auxilia::Monostate, /* implements */ ICommand {
  Unknown() = default;
  explicit Unknown(string_type cmd) : command(std::move(cmd)) {}
  virtual ~Unknown() = default;

  string_type command;
  virtual void execute(Monitor *) const override final {
    if (command.empty())
      auxilia::println(
          stderr, fg(crimson), "luce: no command or subcommand provided");
    else
      auxilia::println(stderr,
                       fg(crimson),
                       "luce: unknown command '{cmd}'. Type "
                       "'help' for more information.",
                       "cmd"_a = command);
  }
};
struct Help final : ICommand {
  virtual void execute(Monitor *) const override final {
    fmt::print(message::repl::Help);
  }
};
struct Exit final : ICommand {
  virtual void execute(Monitor *) const override final {
    std::cout << "Goodbye!" << std::endl;
    std::exit(EXIT_SUCCESS);
  }
};
struct Restart final : ICommand {
  virtual void execute(Monitor *monitor) const override final {
    monitor->notify(nullptr, Event::kRestartOrResumeTask);
  }
};
struct Continue final : ICommand {
  virtual void execute(Monitor *monitor) const override final {
    monitor->execute_n(1);
  }
};
struct Step final : ICommand {
  Step() = default;
  explicit Step(size_t steps) : steps(steps) {}
  size_t steps = 1;
  virtual void execute(Monitor *monitor) const override final {
    monitor->execute_n(steps);
  }
};
struct AddWatchPoint final : ICommand {
  AddWatchPoint() = default;
  explicit AddWatchPoint(std::string expr) : expression(std::move(expr)) {}
  virtual void execute(Monitor *monitor) const override final {
    monitor->debugger().add_watchpoint(expression);
  }
  std::string expression;
};
struct DeleteWatchPoint final : ICommand {
  DeleteWatchPoint() = default;
  explicit DeleteWatchPoint(size_t id) : watchpointId(id) {}
  size_t watchpointId = 0;
  virtual void execute(Monitor *monitor) const override final {
    if (auto res = monitor->debugger().delete_watchpoint(watchpointId)) {
      fmt::println("Deleted watchpoint {}", watchpointId);
    } else {
      spdlog::error("Error: {}", res.message());
    }
  }
};
struct Info final : ICommand {
private:
  struct Registers final : ICommand {
    virtual void execute(Monitor *monitor) const override final {
      auxilia::println(stdout, "{regs}", "regs"_a = monitor->registers());
    }
  };
  struct WatchPoints final : ICommand {
    virtual void execute(Monitor *monitor) const override final {
      fmt::println("{}", monitor->debugger().watchpoints());
    }
  };

public:
  Info() = default;
  explicit Info(const std::string &subCommand) {
    auto trimmed = trim(subCommand);
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
  virtual void execute(Monitor *monitor) const override final {
    infoType.visit(
        [&](const auto &subCommand) -> void { subCommand.execute(monitor); });
  }
};

struct Print final : ICommand {
  Print() = default;
  Print(std::string expr) : expression(std::move(expr)) {}
  virtual void execute(Monitor *monitor) const override final {
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
  const auto it = std::ranges::find_if(input, auxilia::isspacelike);
  std::string_view mainCommand;
  const auto itAtEnd = it == input.end();
  if (itAtEnd) {
    // input itself is the command with no arguments
    mainCommand = input;
  } else {
    mainCommand = input.substr(0, it - input.begin());
  }
  if (mainCommand == "exit" or mainCommand == "q") {
    if (itAtEnd) {
      return {Exit{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "exit: command does not take arguments")};
  }
  if (mainCommand == "help") {
    if (itAtEnd) {
      return {Help{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "help: command does not take arguments")};
  }
  if (mainCommand == "r") {
    if (itAtEnd) {
      return {Restart{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "r: command does not take arguments")};
  }
  if (mainCommand == "c") {
    if (itAtEnd) {
      return {Continue{}};
    }
    return {InvalidArgumentError(fg(crimson),
                                 "c: command does not take arguments")};
  }
  if (mainCommand == "si") {
    if (auto args = trim(input.substr(it - input.begin()));
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
    if (auto maybe_exprStr = trim(input.substr(it - input.begin()));
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

  return {InvalidArgumentError(fg(crimson),
                               "luce: unknown command '{}'. Type 'help' for "
                               "more information.",
                               mainCommand)};
}
} // namespace accat::luce::repl::command
namespace accat::luce::repl {
namespace {
void replImpl(Monitor *monitor) {
  auto input = command::read(monitor);
  if (auto res = command::inspect(input)) {
    res->visit([&](const auto &cmd) -> void { cmd.execute(monitor); });
  } else { // no command found or invalid command
    auxilia::println(stderr,
                     "{error} {msg}",
                     "error"_a = format(fg(crimson), "error:"),
                     "msg"_a = res.message());
  }
}
} // namespace
// coroutine was the previous way to implement the REPL,
//    but now it's not necessary.
// TODO: remove the coroutine
auto repl(Monitor *monitor) -> auxilia::Generator<Status> {
  precondition(monitor, "Monitor must not be nullptr")
  std::exception_ptr eptr;
  fmt::println("{}", message::repl::Welcome);

  for (;;) {
    try {
      replImpl(monitor);
      co_yield {};
    } catch (const std::exception &res) {
      auxilia::println(stderr,
                       "{error} {msg}",
                       "error"_a = fmt::format(fg(crimson), "error:"),
                       "msg"_a = res.what());
      eptr = std::current_exception();
    }
    if (eptr)
      co_yield auxilia::InternalError("REPL exited unexpectedly");
  }
  co_return;
}
} // namespace accat::luce::repl
