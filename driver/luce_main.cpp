
#include <luce/config.hpp>

#include <luce/MainMemory.hpp>
#include "luce/Task.hpp"
#include "luce/isa/architecture.hpp"
#include "exec.hpp"

#include <accat/auxilia/auxilia.hpp>
#ifndef AC_USE_MODULE
#else
import accat.auxilia;
import fmt;
import std;
#endif
namespace accat::luce {
LUCE_DRIVER_API int luce_main(const std::span<const std::string_view> args) {
  int callback = 0;

  auto image = std::async(std::launch::async, []() {
    return accat::auxilia::read_as_bytes<uint32_t>(R"(Z:/luce/data/image.bin)");
  });

  auto &context =
      ExecutionContext<isa::instruction_set::riscv32>::InitializeContext(args);

  auto task = Task<isa::instruction_set::riscv32>{};

  auto littleEndianData = image.get().value_or("\0xcc\0xcc\0xcc\0xcc");
  auto little_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
                littleEndianData.size()};
  auto mainMemory = MainMemory<isa::instruction_set::riscv32>{};
  mainMemory.load_program(little_endian_byte_span);
  
  // test, print image data
  dbg(info,"Data : {:#04x}\n", fmt::join(little_endian_byte_span, " "));

  return callback;
}
} // namespace accat::luce

#if defined(_WIN32) && defined(LUCE_DRIVER_BUILD_SHARED) &&                    \
    defined(LUCE_DRIVER_EXPORTS)
#  include <Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    spdlog::debug("luce driver loaded by process");
    break;
  case DLL_THREAD_ATTACH:
    spdlog::debug("luce driver loaded by thread");
    break;
  case DLL_THREAD_DETACH:
    spdlog::debug("luce driver unloaded by thread");
    break;
  case DLL_PROCESS_DETACH:
    spdlog::debug("luce driver unloaded by process");
    break;
  default:
    spdlog::error("Error: unknown reason for luce driver load/unload");
    return FALSE;
  }
  return TRUE;
}
#endif
