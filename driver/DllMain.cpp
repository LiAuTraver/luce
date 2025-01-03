#include "deps.hh"

#if defined(_WIN32) && defined(LUCE_DRIVER_BUILD_SHARED) &&                    \
    defined(DRIVER_EXPORTS)
#  define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
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
