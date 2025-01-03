#pragma once

#include <fmt/compile.h>
#include <fmt/xchar.h>
#include <scn/scan.h>
#include "MainMemory.hpp"
#include "Pattern.hpp"
#include "Timer.hpp"
#include "accat/auxilia/details/Status.hpp"
#include "accat/auxilia/details/config.hpp"
#include "accat/auxilia/details/macros.hpp"
#include "config.hpp"
#include "SystemBus.hpp"
#include "Task.hpp"
#include "cpu.hpp"

#include <accat/auxilia/auxilia.hpp>
#include <iostream>
#include <utility>
namespace accat::luce {
namespace repl {
using namespace std::literals;
static constexpr inline auto HelpMessage = auxilia::raw(R"(
Available commands:
    - help: show this message
    - exit: exit the program
    - c: continue execution
)");
static const inline auto WelcomeMessage =
    fmt::format(fmt::fg(fmt::color::dark_cyan), "Welcome to luce emulator!\n")
        .append(auxilia::raw(R"(
    Type 'help' for help
    Type 'exit' to exit
)"));
} // namespace repl
class Monitor : public Mediator {
  MainMemory memory;
  SystemBus bus;
  // std::vector<Task> processes; // currently just one process
  Task process;
  CPUs cpus;
  Timer timer;

public:
  virtual auxilia::Status notify(Component *sender, Event event) override {
    // todo: implement
    return {};
  }
  auxilia::Status REPL();
  auxilia::Status shuttle();
  auxilia::Status inspect(std::string_view);
  auxilia::StatusOr<auxilia::string> read();
  auxilia::Status execute_n(size_t);
};
} // namespace accat::luce
/*utils::Status Monitor::initDisassembler(CtxRef) {
  constexpr auto cs_arch = CS_ARCH_RISCV;
  constexpr auto cs_mode =
      static_cast<::cs_mode>(CS_MODE_RISCV64 | CS_MODE_RISCVC);
  return disassembler_.init(cs_arch, cs_mode);
}*/
// inline utils::Status Monitor::initDevice(CtxRef ctx) {
//   // io_space = utils::alloc<uint8_t>(ctx.io_space_max);
//   // p_space = io_space;
//   return {};
//   TODO(init other devices);
// }
// utils::Status Monitor::initDebugSystem(CtxRef) {
//   // TODO: init regex
//   // TODO: init watchpoint pool
//   return {};
//   TODO(not implemented);
// }
