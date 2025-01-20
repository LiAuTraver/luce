#pragma once

#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/xchar.h>
#include <scn/scan.h>
#include <spdlog/spdlog.h>
#include "MainMemory.hpp"
#include "repl/Debugger.hpp"
#include "utils/Pattern.hpp"
#include "luce/utils/Timer.hpp"
#include "config.hpp"
#include "SystemBus.hpp"
#include "Task.hpp"
#include "cpu/cpu.hpp"
#include "isa/architecture.hpp"
#include "Disassembler.hpp"

#include <accat/auxilia/auxilia.hpp>
#include <cstddef>
#include <iostream>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
namespace accat::luce {
namespace message::repl {
using namespace std::literals;
using namespace fmt::literals;
using auxilia::operator""_raw;
using enum fmt::color;
using fmt::bg;
using fmt::fg;
using fmt::format;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
static constexpr inline auto Help = R"(
Available commands:
    - help: show this message
    - exit: exit the program
    - c: continue execution
    - si [steps]: step n instructions
    - r: restart the program
    - info r: show registers
    - info w: show watchpoints
)"_raw;
static const inline auto Welcome =
    format(fg(dark_cyan), "Welcome to luce emulator!\n").append(R"(
    Type 'help' for help
    Type 'exit' to exit
)"_raw);
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
  Disassembler disassembler;
  repl::Debugger debugger;

public:
  Monitor()
      : memory(this), bus(this), cpus(this), disassembler(this),
        debugger(this) {
    if (auto res = disassembler.set_target(isa::instruction_set::riscv32))
      return;
    else {
      spdlog::critical("Failed to initialize disassembler: {}", res.message());
      dbg_break
    }
  }

public:
  virtual auxilia::Status notify(Component *sender, Event event) override;
  auxilia::Status run();
  auxilia::Status REPL();
  auxilia::Status execute_n(size_t);
  auto register_task(const std::ranges::range auto &, paddr_t, paddr_t)
      -> auxilia::Status;

public:
  auto fetch_from_main_memory(const vaddr_t addr, const size_t size)
      -> auxilia::StatusOr<std::span<const std::byte>> {
    return memory.read_n(addr, size);
  }
  auto decode_from_disassembler(const std::span<const std::byte> bytes)
      -> auxilia::StatusOr<auxilia::string> {
    // return disassembler.decode(bytes);
    TODO(...)
  }

public:
private:
  auto _do_register_task_unchecked(std::span<const std::byte>, paddr_t, paddr_t)
      -> auxilia::Status;
  auto _do_execute_n_unchecked(size_t) -> auxilia::Status;
};
auxilia::Status Monitor::register_task(const std::ranges::range auto &program,
                                       const paddr_t start_addr,
                                       paddr_t block_size) {
  std::span<const std::byte> bytes;

  using myType = std::remove_cvref_t<decltype(program)>;
  using myValueType = std::ranges::range_value_t<myType>;
  if constexpr (std::same_as<myType, std::span<std::byte>> ||
                std::same_as<myType, std::span<const std::byte>>) {
    bytes = program;
  } else if constexpr (std::same_as<myValueType, const std::byte> ||
                       std::same_as<myValueType, std::byte>) {
    bytes = std::span{program};
  } else {
    auto dataSpan = std::span{program};
    bytes = std::as_bytes(dataSpan);
  }
  block_size = (std::min)(block_size, isa::physical_memory_size);

  contract_assert(bytes.size() > 0 && bytes.size() <= block_size,
                  "Invalid block size")
  contract_assert(start_addr + bytes.size() <= isa::physical_memory_end,
                  "Out of memory bounds")
  // add `this` for intellisenese (template intellisense was too poor)
  return this->_do_register_task_unchecked(bytes, start_addr, block_size);
}
#pragma clang diagnostic pop
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
