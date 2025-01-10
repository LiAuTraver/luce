#include "deps.hh"

#include <cstdlib>
#include <functional>
#include <luce/config.hpp>
#include <luce/isa/architecture.hpp>
#include <luce/MainMemory.hpp>
#include <luce/Task.hpp>
#include "exec.hpp"
#include "luce/Image.hpp"
#include "luce/Monitor.hpp"
namespace accat::luce {
LUCE_API int luce_main(const std::span<const std::string_view> args) {

  auto imageData =
      auxilia::async(auxilia::read_raw_bytes<>, R"(Z:/luce/data/image.bin)");

  auto context =
      auxilia::async(std::bind(&ExecutionContext::InitializeContext, args));

  auto maybeBytes = imageData.get();
  if (!maybeBytes) {
    spdlog::error("Failed to read image: {}", maybeBytes.message());
    return EXIT_FAILURE;
  }

  auto image = Image{*std::move(maybeBytes), std::endian::little};

  auto monitor = Monitor{};
  if (auto res = monitor.run_new_task(
          image.bytes_view(), isa::physical_base_address, 0x100);
      !res) {
    spdlog::error("Failed to load program: {}", res.message());
    return EXIT_FAILURE;
  }

  return monitor.REPL().raw_code();
}
} // namespace accat::luce
