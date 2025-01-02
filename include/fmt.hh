/// @note fmt precompiled header
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

#include <fmt/base.h>

#include <fmt/format.h>

// #include <fmt/core.h> // compatibility header
// #include <fmt/printf.h> // legacy implementation
#include <fmt/args.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/os.h>
#include <fmt/ranges.h>

#include <fmt/ostream.h>
#include <fmt/std.h>

#include <fmt/xchar.h>
