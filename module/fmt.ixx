module;

// avoid unresolved symbol (reason unknown)
#define FMT_HEADER_ONLY 1

#ifdef __clang__
#  pragma clang system_header
#elifdef __GNUC__
#  pragma GCC system_header
#elifdef _MSC_VER
#  pragma system_header
#else
// nothing
#endif

// NOTE:
// The contents of this header are derived in part from libfmt under the
// following license:

//////////////////////// BEGIN OF FMT COPYRIGHT NOTICE ////////////////////////
// Copyright (c) 2012 - present, Victor Zverovich
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// --- Optional exception to the license ---
//
// As an exception, if, as a result of your compiling your source code, portions
// of this Software are embedded into a machine-executable object form of such
// source code, you may redistribute such embedded portions in such object form
// without including the above copyright and permission notices.

///////////////////////// END OF FMT COPYRIGHT NOTICE //////////////////////////

#ifdef _MSVC_LANG
#  define FMT_CPLUSPLUS _MSVC_LANG
#else
#  define FMT_CPLUSPLUS __cplusplus
#endif

// Put all implementation-provided headers into the global module fragment
// to prevent attachment to this module.
#ifndef FMT_IMPORT_STD
#  include <algorithm>
#  include <bitset>
#  include <chrono>
#  include <cmath>
#  include <complex>
#  include <cstddef>
#  include <cstdint>
#  include <cstdio>
#  include <cstdlib>
#  include <cstring>
#  include <ctime>
#  include <exception>
#  if FMT_CPLUSPLUS > 202002L
#    include <expected>
#  endif
#  include <filesystem>
#  include <fstream>
#  include <functional>
#  include <iterator>
#  include <limits>
#  include <locale>
#  include <memory>
#  include <optional>
#  include <ostream>
#  include <source_location>
#  include <stdexcept>
#  include <string>
#  include <string_view>
#  include <system_error>
#  include <thread>
#  include <type_traits>
#  include <typeinfo>
#  include <utility>
#  include <variant>
#  include <vector>
#else
#  include <limits.h>
#  include <stdint.h>
#  include <stdio.h>
#  include <time.h>
#endif
#include <cerrno>
#include <climits>
#include <version>

#if __has_include(<cxxabi.h>)
#  include <cxxabi.h>
#endif
#if defined(_MSC_VER) || defined(__MINGW32__)
#  include <intrin.h>
#endif
#if defined __APPLE__ || defined(__FreeBSD__)
#  include <xlocale.h>
#endif
#if __has_include(<winapifamily.h>)
#  include <winapifamily.h>
#endif
#if (__has_include(<fcntl.h>) || defined(__APPLE__) || \
     defined(__linux__)) &&                            \
    (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  ifndef _WIN32
#    include <unistd.h>
#  else
#    include <io.h>
#  endif
#endif
#ifdef _WIN32
#  if defined(__GLIBCXX__)
#    include <ext/stdio_filebuf.h>
#    include <ext/stdio_sync_filebuf.h>
#  endif
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

export module fmt;

#ifdef FMT_IMPORT_STD
import std;
#endif

#define FMT_EXPORT export
#define FMT_BEGIN_EXPORT export {
#define FMT_END_EXPORT                                                         \
  ;                                                                            \
  } // unknown error if not adding a semicolon here

// If you define FMT_ATTACH_TO_GLOBAL_MODULE
//  - all declarations are detached from module 'fmt'
//  - the module behaves like a traditional static library, too
//  - all library symbols are mangled traditionally
//  - you can mix TUs with either importing or #including the {fmt} API
#ifdef FMT_ATTACH_TO_GLOBAL_MODULE
extern "C++" {
#endif

#ifndef FMT_OS
#  define FMT_OS 1
#endif

// All library-provided declarations and definitions must be in the module
// purview to be exported.
#include "fmt/args.h"
#include "fmt/chrono.h"
#include "fmt/color.h"
#include "fmt/compile.h"
#include "fmt/format.h"
#if FMT_OS
#  include "fmt/os.h"
#endif
#include "fmt/ostream.h"
#include "fmt/printf.h"
#include "fmt/ranges.h"
#include "fmt/std.h"
#include "fmt/xchar.h"

#ifdef FMT_ATTACH_TO_GLOBAL_MODULE
}
#endif

// gcc doesn't yet implement private module fragments
#if !FMT_GCC_VERSION
module :private;
#endif

#ifdef FMT_ATTACH_TO_GLOBAL_MODULE
extern "C++" {
#endif

#if FMT_HAS_INCLUDE("format.cc")
#  include "format.cc"
#endif
#if FMT_OS && FMT_HAS_INCLUDE("os.cc")
#  include "os.cc"
#endif

#ifdef FMT_ATTACH_TO_GLOBAL_MODULE
}
#endif
