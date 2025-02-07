#include "deps.hh"

#include "luce/config.hpp"
#include "luce/isa/architecture.hpp"
#include "luce/MainMemory.hpp"
#include "luce/Task.hpp"
#include "exec.hpp"
#include "luce/argument/Argument.hpp"
#include "luce/argument/ArgumentLoader.hpp"
#include "luce/Image.hpp"
#include "luce/Monitor.hpp"
#include "luce/isa/riscv32/isa.hpp"

LUCE_API int accat::luce::main(const std::span<const std::string_view> args) {
  auto callback = 0;
  constexpr auto defaultImagePath = R"(Z:/luce/data/image.bin)";

  auto program_arguments = argument::program::args();
  auto program_options = ArgumentLoader("luce", "1.0");
  program_options.load_arguments(program_arguments);

  if (auto res = program_options.parse_arguments(args); !res) {
    fmt::print(stderr, fmt::fg(fmt::color::fire_brick), "Error: ");
    fmt::println(stderr, "{err_msg}", "err_msg"_a = res.message());
    callback = res.raw_code();
    return callback;
  }
  auto context = auxilia::async(std::bind(&ExecutionContext::InitializeContext,
                                          std::ref(program_options)));

  auto imageData = auxilia::async(auxilia::read_raw_bytes<>,
                                  argument::program::image.value.empty()
                                      ? defaultImagePath
                                      : argument::program::image.value);

  auto maybeBytes = imageData.get();
  if (!maybeBytes) {
    spdlog::error("Failed to read image: {}", maybeBytes.message());
    callback = EXIT_FAILURE;
    return callback;
  }

  auto image = Image{*std::move(maybeBytes), std::endian::little};

  auto monitor = Monitor{};
  if (auto res = monitor.register_task(
          image.bytes_view(), isa::virtual_base_address, 0x100);
      !res) {
    spdlog::error("Failed to load program: {}", res.message());
    callback = EXIT_FAILURE;
    return callback;
  }

  if (argument::program::batch.value == true)
    callback = monitor.run().raw_code();
  else
    callback = monitor.REPL().raw_code();

  if (callback == 0)
    spdlog::info("Goodbye!");
  else
    spdlog::error("Program exited abnormally. Check the code.");

  return callback;
}