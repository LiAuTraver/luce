#include <fmt/base.h>
#include <cstdint>
#include <iostream>
#include "include/MainMemory.hpp"

#include "Task.hpp"
#include "accat/auxilia/file_reader.hpp"
#include "exec.hpp"
namespace accat::luce {
int luce_main(const std::span<const std::string_view> args) {
  int callback = 0;

  auto &context =
      ExecutionContext<isa::instruction_set::riscv32>::InitializeContext(args);

  auto task = Task<isa::instruction_set::riscv32>{};

  return callback;
}
} // namespace accat::luce
