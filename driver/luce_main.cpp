#include <filesystem>
#include <functional>
#include <memory>
#include <utility>
#include "luce/Support/isa/riscv32/instruction/Atomic.hpp"
#include "luce/config.hpp"
#include "exec.hpp"
#include "luce/Support/isa/IDisassembler.hpp"
#include "luce/Support/isa/riscv32/instruction/Multiply.hpp"
#include "luce/argument/Argument.hpp"
#include "luce/argument/ArgumentLoader.hpp"
#include "luce/Image.hpp"
#include "luce/Monitor.hpp"

LUCE_API int accat::luce::main(const std::span<const std::string_view> args) {
  auto callback = 0;
  constexpr auto defaultImagePath = R"(Z:/luce/data/image.bin)";

  auto program_arguments = argument::program::args();
  auto program_options = ArgumentLoader("luce", "1.0");
  program_options.load_arguments(program_arguments);

  if (auto res = program_options.parse_arguments(args); !res) {
    auxilia::println(stderr,
                     fg(fmt::color::fire_brick),
                     "Error: {err_msg}",
                     "err_msg"_a = res.message());
    callback = res.raw_code();
    return callback;
  }
  auto contextFut = auxilia::async(ExecutionContext::InitializeContext,
                                   std::ref(program_options));

  auto imagePath = argument::program::image.value.empty()
                       ? defaultImagePath
                       : argument::program::image.value;

  auto imageFut = auxilia::async(Image::FromPath<>, imagePath);
  auto &context = contextFut.get();

  auto monitor = Monitor{std::move(context.disassembler)};

  auto image = imageFut.get();
  if (!image) {
    spdlog::error("Failed to read image: {}", image.message());
    callback = EXIT_FAILURE;
    return callback;
  }

  if (auto res = monitor.register_task(
          image->bytes_view(), isa::virtual_base_address, 0x10000);
      !res) {
    spdlog::error("Failed to load program: {}", res.message());
    callback = EXIT_FAILURE;
    return callback;
  }
  spdlog::info("Image loaded from {}", std::filesystem::absolute(imagePath));
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
