#pragma once

#if _WIN32
#  if defined(LUCE_DRIVER_BUILD_SHARED)
#    if defined(DRIVER_EXPORTS) || defined(driver_EXPORTS)
#      define LUCE_API __declspec(dllexport)
#    else
#      define LUCE_API __declspec(dllimport)
#    endif
#  else
#    define LUCE_API
#  endif
#else
#  define LUCE_API
#endif

/// @brief minimize includes, meanwhile provide a way to use fmt::literals
namespace fmt::inline v11::inline literals {}
namespace std::inline literals {}
namespace accat::luce {
using namespace fmt::v11::literals;
using namespace std::literals;
} // namespace accat::luce
