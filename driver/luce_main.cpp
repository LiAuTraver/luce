#include "deps.hh"
#include <algorithm>
#include <luce/config.hpp>
#include <luce/isa/architecture.hpp>
#include <luce/MainMemory.hpp>
#include <luce/Task.hpp>
#include "exec.hpp"
#include "luce/Monitor.hpp"
#include <accat/auxilia/auxilia.hpp>
namespace accat::luce {
LUCE_API int luce_main(const std::span<const std::string_view> args) {
  int callback = 0;

  auto image = std::async(std::launch::async, []() {
    return accat::auxilia::read_as_bytes<uint32_t>(R"(Z:/luce/data/image.bin)");
  });

  auto &context = ExecutionContext::InitializeContext(args);
  auto res = image.get();

  if (!res) {
    spdlog::error("Failed to read image: {}", res.message());
    return -1;
  }
  auto littleEndianData = *std::move(res);
  // auto little_endian_byte_span =
  //     std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
  //               littleEndianData.size()};

  //
  // // test, print image data
  // dbg(info,"Data : {:#04x}\n", fmt::join(little_endian_byte_span, " "))

  auto monitor = Monitor{};
  if (auto res = monitor.run_new_task(littleEndianData, isa::physical_base_address, 0x100); !res) {
    spdlog::error("Failed to load program: {}", res.message());
    return -1;
  }
  monitor.REPL();

  return callback;
}
} // namespace accat::luce
