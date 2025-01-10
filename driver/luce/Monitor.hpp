#pragma once

#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/xchar.h>
#include <scn/scan.h>
#include "MainMemory.hpp"
#include "Pattern.hpp"
#include "Timer.hpp"
#include "config.hpp"
#include "SystemBus.hpp"
#include "Task.hpp"
#include "cpu.hpp"
#include "isa/architecture.hpp"
#include "Disassembler.hpp"

#include <accat/auxilia/auxilia.hpp>
#include <iostream>
#include <ranges>
#include <utility>
namespace accat::luce {
namespace message::repl {
using namespace std::literals;
using auxilia::raw;
using enum fmt::color;
using fmt::bg;
using fmt::fg;

// NOLINTNEXTLINE
static constexpr inline auto Help = raw(R"(
Available commands:
    - help: show this message
    - exit: exit the program
    - c: continue execution
)");
static const inline auto Welcome =
    fmt::format(fg(dark_cyan), "Welcome to luce emulator!\n").append(raw(R"(
    Type 'help' for help
    Type 'exit' to exit
)"));
} // namespace message::repl
class Monitor : public Mediator {
  using paddr_t = isa::physical_address_t;
  using vaddr_t = isa::virtual_address_t;

  MainMemory memory;
  SystemBus bus;
  // std::vector<Task> processes; // currently just one process
  Task process;
  CPUs cpus;
  Timer timer;

public:
  Monitor() : memory(this), bus(this), cpus(this) {}

public:
  virtual auxilia::Status notify(Component *sender, Event event) override;
  auxilia::Status REPL();
  auxilia::Status shuttle();
  auxilia::Status inspect(std::string_view);
  auxilia::StatusOr<auxilia::string> read();
  auxilia::Status execute_n(size_t);
  auto run_new_task(const std::ranges::range auto &, paddr_t, paddr_t)
      -> auxilia::Status;

public:
  auto fetch_from_main_memory(vaddr_t addr, size_t size)
      -> auxilia::StatusOr<std::span<const std::byte>> {
    return memory.read_n(addr, size);
  }

public:
  // auxilia::Status register_process(Task &&task) {
  //   process = std::move(task);
  //   return this->set_as_parent(&process);
  // }
private:
  auxilia::Status
      _do_run_new_task_unchecked(std::span<const std::byte>, paddr_t, paddr_t);
  auxilia::Status _do_execute_n_unchecked(size_t);
};
auxilia::Status Monitor::run_new_task(const std::ranges::range auto &program,
                                      const paddr_t start_addr,
                                      paddr_t block_size) {
  auto dataSpan = std::span{program};
  auto bytes = std::as_bytes(dataSpan);
  block_size = std::min(block_size, isa::physical_memory_size);

  contract_assert(bytes.size() > 0 && bytes.size() <= block_size,
                  "Invalid block size")
  contract_assert(start_addr + bytes.size() <= isa::physical_memory_end,
                  "Out of memory bounds")
  // add `this` for intellisenese (template intellisense was too poor)
  return this->_do_run_new_task_unchecked(bytes, start_addr, block_size);
}
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
