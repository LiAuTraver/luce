#pragma once

#include <cstddef>
#include <functional>
#include <iostream>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <spdlog/spdlog.h>
#include <accat/auxilia/auxilia.hpp>

#include "MainMemory.hpp"
#include "repl/Debugger.hpp"
#include "luce/Support/utils/Pattern.hpp"
#include "luce/Support/utils/Timer.hpp"
#include "config.hpp"
#include "SystemBus.hpp"
#include "Task.hpp"
#include "cpu/cpu.hpp"

#include "luce/Support/isa/architecture.hpp"

namespace accat::luce::isa {
class IDisassembler;
}
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
    Type 'exit' to  exit.
)"_raw);
} // namespace message::repl
class Monitor : public Mediator {
  using paddr_t = isa::physical_address_t;
  using vaddr_t = isa::virtual_address_t;

  MainMemory memory_;
  // SystemBus bus;
  // std::vector<Task> processes; // currently just one process
  Task process;
  CPUs cpus;
  Timer timer;
  std::shared_ptr<isa::IDisassembler> disassembler_;
  repl::Debugger debugger_;

public:
  Monitor();
  virtual ~Monitor() override;
  auto &debugger(this auto &&self) noexcept {
    return self.debugger_;
  }
  auto &memory(this auto &&self) noexcept {
    return self.memory_;
  }
  auto &disassembler(this auto &&self) noexcept {
    return self.disassembler_;
  }
  auto &registers(this auto &&self) noexcept {
    // currently only one task
    return *self.process.context().general_purpose_registers();
  }

public:
  virtual auto notify(
      Component *sender,
      Event event,
      std::function<void(void)> callback = []() {}) -> auxilia::Status override;

  auxilia::Status run();
  auxilia::Status REPL();
  auxilia::Status resume();
  auxilia::Status execute_n(size_t);
  auto register_task(const std::ranges::range auto &, paddr_t, paddr_t)
      -> auxilia::Status;

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
                std::same_as<myType, std::span<const std::byte>>)
    bytes = program;
  else if constexpr (std::same_as<myValueType, const std::byte> ||
                     std::same_as<myValueType, std::byte>)
    bytes = std::span{program};
  else
    bytes = std::as_bytes(std::span{program});

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
