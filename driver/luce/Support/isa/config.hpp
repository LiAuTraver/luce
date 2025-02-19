#pragma once

#if _WIN32
#  if defined(Support_isa_EXPORTS) || defined(SUPPORT_ISA_EXPORTS)
#    define LUCE_SUPPORT_ISA_API __declspec(dllexport)
#  else
#    define LUCE_SUPPORT_ISA_API __declspec(dllimport)
#  endif
#else
#  define LUCE_SUPPORT_ISA_API
#endif