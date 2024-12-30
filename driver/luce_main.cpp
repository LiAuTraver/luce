#include <iostream>
#include <span>
#include <string_view>

#include "exec.hpp"
namespace accat::luce {
[[nodiscard]] int
    luce_main([[maybe_unused]] std::span<const std::string_view>);
}

namespace accat::luce {
[[nodiscard]] int
luce_main([[maybe_unused]]const std::span<const std::string_view> args) {
  int callback = 0;

  auto &context = ExecutionContext<
      isa::instruction_set::riscv32>::InitializeContext(args);

      
  return callback;
}
} // namespace accat::luce
