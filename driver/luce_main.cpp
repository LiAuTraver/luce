#include <fmt/base.h>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <span>
#include <string_view>
#include <accat/auxilia/file_reader.hpp>
#include <fmt/ranges.h>
#include "exec.hpp"
namespace accat::luce {
[[nodiscard]] int
    luce_main([[maybe_unused]] std::span<const std::string_view>);
}

namespace accat::luce {
int luce_main(const std::span<const std::string_view> args) {
  int callback = 0;

  auto &context = ExecutionContext<
      isa::instruction_set::riscv32>::InitializeContext(args);


  return callback;
}
} // namespace accat::luce
