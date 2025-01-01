#pragma once
#include <accat/auxilia/config.hpp>
#if _WIN32
#  if defined(LUCE_DRIVER_BUILD_SHARED)
#    if defined(LUCE_DRIVER_EXPORTS) || defined (luce_driver_EXPORTS)
#      define LUCE_DRIVER_API __declspec(dllexport)
#    else
#      define LUCE_DRIVER_API __declspec(dllimport)
#    endif
#  else
#    define LUCE_DRIVER_API
#  endif
#else
#  define LUCE_DRIVER_API
#endif
