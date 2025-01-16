#include "accat/auxilia/details/macros.hpp"
#include "deps.hh"

#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <luce/config.hpp>
#include <luce/isa/architecture.hpp>
#include <luce/MainMemory.hpp>
#include <luce/Task.hpp>
#include <string_view>
#include "exec.hpp"
#include "luce/Argument.hpp"
#include "luce/ArgumentLoader.hpp"
#include "luce/Image.hpp"
#include "luce/Monitor.hpp"
#include "luce/isa/riscv32/isa.hpp"
namespace accat::luce {
LUCE_API int luce_main(const std::span<const std::string_view> args) {
  auto program_arguments = argument::program::args();
  auto program_options = ArgumentLoader("luce", "1.0");
  program_options.load_arguments(program_arguments);

  if (auto res = program_options.parse_arguments(args); !res) {
    fmt::print(stderr, fmt::fg(fmt::color::fire_brick), "Error: ");
    fmt::println(stderr, "{err_msg}", "err_msg"_a = res.message());
    return res.raw_code();
  }

  auto imageData =
      auxilia::async(auxilia::read_raw_bytes<>, R"(Z:/luce/data/image.bin)");

  auto context = auxilia::async(std::bind(&ExecutionContext::InitializeContext,
                                          std::ref(program_options)));

  auto maybeBytes = imageData.get();
  if (!maybeBytes) {
    spdlog::error("Failed to read image: {}", maybeBytes.message());
    return EXIT_FAILURE;
  }

  auto image = Image{*std::move(maybeBytes), std::endian::little};

  auto monitor = Monitor{};
  if (auto res = monitor.register_task(
          image.bytes_view(), isa::virtual_base_address, 0x100);
      !res) {
    spdlog::error("Failed to load program: {}", res.message());
    return EXIT_FAILURE;
  }

  if (argument::program::batch.value == true)
    return monitor.run().raw_code();

  return monitor.REPL().raw_code();
}
} // namespace accat::luce
