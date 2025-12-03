#include "deps.hh"

#include "luce/config.hpp"
#include "luce/repl/repl_fwd.hpp"
#include "luce/repl/evaluation.hpp"
#include "luce/repl/ExprVisitor.hpp"
#include "luce/Monitor.hpp"
#include "luce/repl/Lexer.hpp"
#include "luce/repl/Parser.hpp"
#include "luce/Support/utils/Pattern.hpp"

#include <accat/auxilia/defines.hpp>

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

template <typename T>
  requires std::is_arithmetic_v<T>
auto to_number(const std::string_view sv) -> StatusOr<T> {

  constexpr auto makeErr = [](auto &&ec) {
    return InvalidArgumentError(fg(crimson),
                                "Error converting string to number: {}",
                                std::make_error_code(ec).message());
  };

  if constexpr (std::is_floating_point_v<T>) {
    T F;
    const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), F);
    if (ec != std::errc())
      return makeErr(ec);

    return F;
  } else {
    T I;
    const auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), I);
    if (ec != std::errc())
      return makeErr(ec);

    return I;
  }
}

std::string read(Monitor *) {
  std::string input;
  for (;;) {
    std::string raw_input;
    if (input.empty())
      fmt::print(stdout, fg(cyan), "(luce) ");
    else
      fmt::print(stdout, fg(cyan), ">>> ");

// just a workaround for wsl,
// for some reason it does not flush the output stream
#ifdef __linux__
    fflush(stdout);
#endif

    if (!std::getline(std::cin, raw_input)) {
      if (std::cin.eof()) {
        fmt::println("\nGoodbye!");
        std::exit(EXIT_SUCCESS);
      }

      if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        auxilia::Println(stderr, fg(crimson), "Input error, please try again");
        continue;
      }
      AC_UNREACHABLE("Unexpected error reading input");
      continue;
    }
    auto trimmed_input = trim(raw_input);
    if (trimmed_input.empty() && input.empty())
      continue;
    else if (trimmed_input.starts_with('#'))
      continue;
    else if (trimmed_input.ends_with('\\')) {
      trimmed_input.remove_suffix(1);
      input += trimmed_input;
      continue;
    }
    input += trimmed_input;
    return input;
  }
}

auto unknown_cmd(Monitor *, const std::string &command = "") {
  if (command.empty())
    return InvalidArgumentError("luce: no command or subcommand provided");
  else
    return InvalidArgumentError("luce: unknown command '{cmd}'. Type "
                                "'help' for more information.",
                                "cmd"_a = command);
}

auto help_cmd(Monitor *) {
  fmt::print(message::repl::Help);
  return auxilia::OkStatus();
}

[[noreturn]] Status exit_cmd(Monitor *) {
  std::cout << "Goodbye!" << std::endl;
  std::exit(EXIT_SUCCESS);
  // return auxilia::OkStatus(); // to satisfy the compiler
}

auto restart_cmd(Monitor *monitor) {
  return monitor->notify(nullptr, Event::kRestartOrResumeTask);
}

auto continue_cmd(Monitor *monitor) {
  return monitor->resume();
}

auto step_cmd(Monitor *monitor, size_t steps = 1) {
  return monitor->execute_n(steps);
}

auto add_watchpoint_cmd(Monitor *monitor, const std::string &expression) {
  return monitor->debugger().add_watchpoint(expression);
}

auto delete_watchpoint_cmd(Monitor *monitor, size_t watchpointId) {
  if (auto res = monitor->debugger().delete_watchpoint(watchpointId)) {
    fmt::println("Deleted watchpoint {}", watchpointId);
    return auxilia::OkStatus();
  } else {
    return res;
  }
}

auto info_registers_cmd(Monitor *monitor) {
  auxilia::Println(stdout, "{regs}", "regs"_a = monitor->registers());
  return auxilia::OkStatus();
}

auto info_watchpoints_cmd(Monitor *monitor) {
  fmt::println("{}", monitor->debugger().watchpoints());
  return auxilia::OkStatus();
}

auto info_cmd(Monitor *monitor, const std::string &subCommand) {
  auto trimmed = trim(subCommand);
  if (trimmed.empty()) {
    return unknown_cmd(monitor, subCommand);

  } else if (trimmed == "r" || trimmed == "registers") {
    return info_registers_cmd(monitor);
  } else if (trimmed == "w" || trimmed == "watchpoints") {
    return info_watchpoints_cmd(monitor);
  } else {
    return unknown_cmd(monitor, std::string{trimmed.begin(), trimmed.end()});
  }
}

auto print_cmd(Monitor *monitor, const std::string &expression) {
  Status status;
  auto lexer = Lexer{};
  auto parser = Parser{lexer.load_string(expression).lex()};
  auto eval = expression::Evaluator{monitor};
  parser.next_expression()
      .transform([&](auto &&res1) {
        res1->accept(eval)
            .transform([](auto &&res2) {
              auxilia::Println(
                  stdout,
                  "{result}",
                  "result"_a = res2.to_string(auxilia::FormatPolicy::kBrief));
            })
            .transform_error([&](auto &&res2) { status = res2; });
      })
      .transform_error([&](auto &&err) { status = err; });
  return status;
}

auto scan_cmd(Monitor *monitor, size_t count, const std::string &expression) {
  Status status;
  auto lexer = Lexer{};
  auto parser = Parser{lexer.load_string(expression).lex()};
  auto eval = expression::Evaluator{monitor};
  parser.next_expression().transform([&](auto &&value) {
    value->accept(eval)
        .transform([&](auto &&value) {
          auto ptr = value.template get_if<evaluation::Number>();
          if (!ptr) {
            // does not hold a number
            status = InvalidArgumentError(
                "expression does not evaluate to a number");
            return;
          }
          if (auto num = ptr->integer()) {
            monitor->memory()
                .read_n(*num, count * isa::instruction_size_bytes)
                .transform([num](auto &&byteSpan) {
                  // print the result as hex
                  fmt::println(
                      "Bytes start at address {addr:#10x}: {bytes:#04x}",
                      "addr"_a = num,
                      "bytes"_a = fmt::join(byteSpan, " "));
                })
                .transform_error([&](auto &&err) { status = err; });
            return;
          }
          // holds a floating point number
          status = InvalidArgumentError(
              "expression does not evaluate to an integer");
        })
        // evaluation error
        .transform_error([&](auto &&err) { status = err; });
  });
  return status;
}

// Simplified command dispatch - returns a status
Status dispatch_command(Monitor *monitor, std::string_view input) {
  // extract out the first command
  const auto it = std::ranges::find_if(input, auxilia::isspacelike);
  std::string_view mainCommand;
  const auto itAtEnd = it == input.end();

  if (itAtEnd)
    // input itself is the command with no arguments
    mainCommand = input;
  else
    mainCommand = input.substr(0, it - input.begin());

  const auto C =
      [&mainCommand](const std::string_view name) constexpr noexcept {
        return mainCommand == name;
      };

  if (C("exit") || C("q")) {
    if (itAtEnd)
      return exit_cmd(monitor);

    return InvalidArgumentError("exit: command does not take arguments");
  }

  if (C("help")) {
    if (itAtEnd)
      return help_cmd(monitor);

    return InvalidArgumentError("help: command does not take arguments");
  }

  if (C("r")) {
    if (itAtEnd)
      return restart_cmd(monitor);

    return InvalidArgumentError("r: command does not take arguments");
  }

  if (C("c")) {
    if (itAtEnd)
      return continue_cmd(monitor);

    return InvalidArgumentError("c: command does not take arguments");
  }

  if (C("si")) {
    if (auto args = trim(input.substr(it - input.begin()));
        !args.empty() && args.front() == '[' && args.back() == ']') {
      if (auto maybe_steps = to_number<size_t>(args.substr(1, args.size() - 2)))
        return step_cmd(monitor, *maybe_steps);
      else
        return InvalidArgumentError("si: error parsing number: {}",
                                    maybe_steps.message());
    }

    return step_cmd(monitor, 1); // default to 1
  }

  if (C("p")) {
    return print_cmd(monitor, std::string{input.substr(it - input.begin())});
  }

  if (C("info")) {
    return info_cmd(monitor, std::string{input.substr(it - input.begin())});
  }

  if (C("w")) {
    if (auto maybe_exprStr = trim(input.substr(it - input.begin()));
        !maybe_exprStr.empty())
      return add_watchpoint_cmd(
          monitor, std::string{maybe_exprStr.begin(), maybe_exprStr.end()});

    return InvalidArgumentError("w: requires 'expression' as argument");
  }

  if (C("d")) {
    if (auto maybe_watchpoint =
            to_number<size_t>(input.substr(it - input.begin())))
      return delete_watchpoint_cmd(monitor, *maybe_watchpoint);

    return InvalidArgumentError("d: requires 'number' as argument");
  }

  if (C("x")) {
    // x N EXPR
    if (auto args = trim(input.substr(it - input.begin())); !args.empty()) {
      auto s = std::ranges::find_if(args, auxilia::isspacelike);
      auto maybe_num = to_number<size_t>(args.substr(0, s - args.begin()));
      if (!maybe_num)
        return InvalidArgumentError("x: error parsing number: {}",
                                    maybe_num.message());

      auto expr = trim(args.substr(s - args.begin()));
      if (expr.empty())
        return InvalidArgumentError("x: requires 'expression' as argument");

      return scan_cmd(
          monitor, *maybe_num, std::string{expr.begin(), expr.end()});
    }
    return InvalidArgumentError(
        "x: requires 'number' and 'expression' as arguments");
  }

  return unknown_cmd(monitor, std::string{mainCommand});
}

} // namespace
} // namespace accat::luce::repl::command

namespace accat::luce::repl {

// coroutine was the previous way to implement the REPL,
//    but now it's not necessary.
// TODO: remove the coroutine
auto repl(Monitor *monitor) -> auxilia::Generator<Status> {
  precondition(monitor, "Monitor must not be nullptr")
  std::exception_ptr eptr;
  fmt::println("{}", message::repl::Welcome);

  for (;;) {
    try {
      if (auto status =
              command::dispatch_command(monitor, command::read(monitor));
          !status) {
        auxilia::Println(stderr,
                         fg(crimson),
                         "{error} {msg}",
                         "error"_a = format(fg(crimson), "error:"),
                         "msg"_a = status.message());
      }
      co_yield {};
    } catch (const std::exception &res) {
      auxilia::Println(stderr,
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
