#include "config.hpp"

#include <fmt/base.h>
#include <cstdint>
#include <span>
#include <iostream>
#include "MainMemory.hpp"

#include "Task.hpp"
#include <accat/auxilia/file_reader.hpp>
#include "exec.hpp"
namespace accat::luce {
LUCE_DRIVER_API int luce_main(const std::span<const std::string_view> args) {
  int callback = 0;

  auto &context =
      ExecutionContext<isa::instruction_set::riscv32>::InitializeContext(args);

  auto task = Task<isa::instruction_set::riscv32>{};

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
