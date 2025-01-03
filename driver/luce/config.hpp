#pragma once

#if _WIN32
#  if defined(LUCE_DRIVER_BUILD_SHARED)
#    if defined(DRIVER_EXPORTS) || defined (driver_EXPORTS)
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
