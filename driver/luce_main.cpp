#include "luce/config.hpp"
#include "exec.hpp"
#include "luce/Support/isa/IDisassembler.hpp"
#include "luce/Support/isa/riscv32/instruction/Multiply.hpp"
#include "luce/Support/isa/riscv32/instruction/Atomic.hpp"
#include "luce/args.hpp"
#include "luce/Image.hpp"
#include "luce/Monitor.hpp"

LUCE_API int accat::luce::main(const std::span<const std::string_view> args) {
  auto callback = 0;

  auto parser = auxilia::program_options::Global("luce", "1.1");

  if (auto res = load_arguments() && parse_arguments(args); !res) {
    auxilia::println(stderr,
                     fg(fmt::color::fire_brick),
                     "Error: {err_msg}",
                     "err_msg"_a = res.message());
    callback = EXIT_FAILURE;
    return callback;
  }

  auto contextFut = auxilia::async(ExecutionContext::InitializeContext);

  auto maybe_imagePath = parser->get_option("image")->value();
  if (!maybe_imagePath) {
    spdlog::error("Please provide an image path.");
    return EXIT_FAILURE;
  }

  auto imageFut = auxilia::async(Image::FromPath<>, *maybe_imagePath);
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
  spdlog::info("Image loaded from {}",
               std::filesystem::absolute(*maybe_imagePath));

  if (isBatchMode())
    callback = monitor.run().raw_code();
  else
    callback = monitor.REPL().raw_code();

  if (callback == 0)
    spdlog::info("Goodbye!");
  else
    spdlog::error("Program exited abnormally. Check the code.");

  return callback;
}
