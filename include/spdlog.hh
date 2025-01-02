#pragma once
#ifdef __clang__
#  pragma clang system_header
#elifdef __GNUC__
#  pragma GCC system_header
#elifdef _MSC_VER
#  pragma system_header
#else
#  pragma system_header
#endif
#include <spdlog/spdlog.h>