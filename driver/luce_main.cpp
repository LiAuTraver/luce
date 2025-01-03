#include "deps.hh"
#include <luce/config.hpp>
#include <luce/isa/architecture.hpp>
#include <luce/MainMemory.hpp>
#include <luce/Task.hpp>
#include "exec.hpp"
#include "luce/Monitor.hpp"
#include <accat/auxilia/auxilia.hpp>
namespace accat::luce {
LUCE_DRIVER_API int luce_main(const std::span<const std::string_view> args) {
  int callback = 0;

  auto image = std::async(std::launch::async, []() {
    return accat::auxilia::read_as_bytes<uint32_t>(R"(Z:/luce/data/image.bin)");
  });

  auto &context =
      ExecutionContext::InitializeContext(args);

  auto task = Task{};

  auto littleEndianData = image.get().value_or("\xcc\xcc\xcc\xcc");
  auto little_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
                littleEndianData.size()};
                
  auto mainMemory = MainMemory{};
  mainMemory.load_program(little_endian_byte_span);
  
  // test, print image data
  dbg(info,"Data : {:#04x}\n", fmt::join(little_endian_byte_span, " "))

  auto monitor = Monitor{};
  monitor.REPL();

  return callback;
}
} // namespace accat::luce