include_guard()
add_compile_definitions(
  _DEBUG DEBUG __DEBUG__ # common debug flags
  AC_CPP_DEBUG
  SPDLOG_FMT_EXTERNAL
  _CRT_SECURE_NO_WARNINGS

  # shut up the error of iterator debug level mismatch
  # _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
  # shut up the error of mismatched annotation level
  _DISABLE_VECTOR_ANNOTATION
  _DISABLE_STRING_ANNOTATION
)
nonnull(AC_CPP_DEBUG)

if(CXX_FLAGS_STYLES_MSVC)
  message(STATUS "Use MSVC debug flags")
  add_compile_options(
    /JMC # just my code debugging
    /wd4834 # discarding return value of function with 'nodiscard' attribute
    /analyze- # don't run analysis when compiling (too slow)
    /Zi /Od /Ob0 /Oy-
    /GR # <- rtti
    /fsanitize=address /fsanitize=fuzzer
    /guard:cf /guard:ehcont
    /RTCsu /sdl

    # /Qpar # parallel code generation
    /Zc:wchar_t /Zc:forScope /Zc:rvalueCast
    /fp:precise
    /WX- /external:W0 /W4
    /external:anglebrackets /errorReport:prompt /diagnostics:caret /openmp
    /MP /Gd /FC
    /GS

    # this has already appeared in CMakePresets.json, but just in case
    /permissive- /volatile:iso /Zc:inline /Zc:preprocessor
    /Zc:enumTypes /Zc:lambda /Zc:__cplusplus /Zc:externConstexpr
    /Zc:throwingNew /EHsc
  )

  add_link_options(
    /DEBUG:FULL

    # /incremental is not compatible with /Brepro (dunno where it's from, probably LLVM's config)
    /INCREMENTAL:NO
    /SUBSYSTEM:CONSOLE
    /MACHINE:X64
    /GUARD:CF # control flow protection
    /GUARD:EHCONT # exception handler control
    # /bigobj # big object files
  )

# failed. vvvvvv
elseif(CXX_FLAGS_STYLES_CLANGCL)
  message(STATUS "Use Clang-CL debug flags")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
  add_compile_options(
    -O0
    -fsanitize=address
    -fcf-protection=full
    -Wno-system-headers
    -Wno-deprecated-declarations
    /MD # <- runtime library
  )

  add_link_options(
    -fsanitize=address
    -shared-libasan
  )

elseif(CXX_FLAGS_STYLES_CLANG)
  # see below, this is not `todo`.
  message(STATUS "Use Clang debug flags")
elseif(CXX_FLAGS_STYLES_GNU)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(FATAL_ERROR "GNU style flags are not supported on Windows")
  endif()

  add_compile_options(-ftrapv

    # Arch for simd
    $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>:-m64>
  )
else()
  message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# add common gnu/clang flags
if(CXX_FLAGS_STYLES_GNU OR CXX_FLAGS_STYLES_CLANG)
  add_compile_options(
    -g3 -O0 -fno-inline -fstandalone-debug # Debug flags
    -frtti
    -fsanitize=address -fsanitize=fuzzer -fsanitize=undefined
    -fcf-protection=full -fstack-protector-strong # control flow protection

    # -fopenmp # openmp/qpar/simd
    -Wno-system-headers -Wno-include-angled-in-module-purview
    -Wno-deprecated-declarations -Wno-deprecated -Wno-c23-extensions
    -Wimplicit-fallthrough
  )
  add_link_options(
    -g -fsanitize=address -fsanitize=fuzzer -fsanitize=undefined
  )
endif()
