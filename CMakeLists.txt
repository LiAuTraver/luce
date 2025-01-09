cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(luce-project
  LANGUAGES C CXX
  VERSION 0.1.0
  DESCRIPTION "A simple riscv64 emulator and its assets"
)

include(cmake/prelude.cmake)
include(cmake/folders.cmake)

set(LUCE_PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# if source dir is the same as project root dir,enable testing
if(PROJECT_IS_TOP_LEVEL)
  # MUST include at the TOP level CMakelists.txt otherwise Ctest will not work
  enable_testing()

  # set this BEFORE include CTest to disable those annoying CTest build targets
  set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
  include(CTest)
  include(GoogleTest)
endif()

option(AC_CPP_DEBUG "set the environment variable AC_CPP_DEBUG to enable debug mode" OFF)
is_undefined(ENV{AC_CPP_DEBUG})

if(DEFINED ENV{AC_CPP_DEBUG})
  if($ENV{AC_CPP_DEBUG} STREQUAL "ON")
    message(STATUS "Debug mode is ON. Corresponding macro features will be enabled: DEBUG, _DEBUG, DEBUG_, _DEBUG_, AC_CPP_DEBUG")
    set(AC_CPP_DEBUG ON CACHE BOOL "Debug mode is ON" FORCE)
    set(VCPKG_MANIFEST_MODE OFF)
    set(VCPKG_MANIFEST_INSTALL OFF)
  endif()
endif()

if(AC_CPP_DEBUG)
  add_compile_definitions(
    _DEBUG DEBUG __DEBUG__ AC_CPP_DEBUG
    SPDLOG_FMT_EXTERNAL
    _CRT_SECURE_NO_WARNINGS
  )

  if(CXX_FLAGS_STYLES_MSVC)
    add_compile_options(
      /D_DISABLE_VECTOR_ANNOTATION /D_DISABLE_STRING_ANNOTATION
      /Zi /Od /Ob0 /JMC /Oy-
      /GR # <- rtti
      /fsanitize=address /fsanitize=fuzzer
      /guard:cf /guard:ehcont
      /RTCsu /sdl
      /Qpar
      /Zc:wchar_t /Zc:forScope /Zc:rvalueCast
      /fp:precise
      /WX- /external:W0 /W3
      /external:anglebrackets /errorReport:prompt /diagnostics:caret /openmp
      /MP /Gd /FC
      /GS
      # this has already appeared in CMakePresets.json, but just in case
      /permissive- /volatile:iso /Zc:inline /Zc:preprocessor /Zc:enumTypes /Zc:lambda /Zc:__cplusplus /Zc:externConstexpr /Zc:throwingNew /EHsc
    )

    add_link_options(
      /DEBUG:FULL /INCREMENTAL
      /SUBSYSTEM:CONSOLE /MACHINE:X64
      /fsanitize=address /fsanitize=fuzzer
    )
  elseif(CXX_FLAGS_STYLES_CLANGCL)
    # TODO
    message(FATAL_ERROR "Clang-CL TODO")
  elseif(CXX_FLAGS_STYLES_CLANG)
  # see below, this is not `todo`.
  elseif(CXX_FLAGS_STYLES_GNU)
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
      -fsanitize=address -fsanitize=fuzzer -fsanitize=undefined -fsanitize=leak
      -fcf-protection=full -fstack-protector-strong -fstack-clash-protection # control flow protection
      -fopenmp # openmp/qpar/simd
      -Wno-system-headers -Wno-include-angled-in-module-purview
      -Wno-deprecated-declarations -Wno-deprecated -Wno-c23-extensions
      -Wimplicit-fallthrough
    )
    add_link_options(
      -g -fsanitize=address -fsanitize=fuzzer -fsanitize=undefined -fsanitize=leak
    )
  endif()
endif()

include(cmake/dependencies.cmake)

include_directories(driver)
include_directories(include)

add_subdirectory(driver)
add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(demo)
#[[=== Asan dll and lib(since we altered the scnlib's build to include ASAN, no need to use this snippet)
  # Get MSVC tools directory from environment or VS installation
  if(DEFINED ENV{VCToolsInstallDir})
    set(VC_TOOLS_DIR $ENV{VCToolsInstallDir})
  else()
    # Fallback to typical VS2022 path
    set(VC_TOOLS_DIR "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.42.34433")
  endif()
  set(ASAN_RT_DLL_DEBUG "${VC_TOOLS_DIR}/bin/Hostx64/x64/clang_rt.asan_dbg_dynamic-x86_64.dll")
  set(ASAN_RT_DLL_RELEASE "${VC_TOOLS_DIR}/bin/Hostx64/x64/clang_rt.asan_dynamic-x86_64.dll")

  # Copy ASAN DLLs to output directory post-build
  add_custom_command(TARGET luce POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  $<$<CONFIG:Debug>:${ASAN_RT_DLL_DEBUG}>$<$<CONFIG:Release>:${ASAN_RT_DLL_RELEASE}>
  $<TARGET_FILE_DIR:luce>
  )

  # Link against ASAN runtime and thunk
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_libraries(luce PRIVATE
  "${VC_TOOLS_DIR}/lib/x64/clang_rt.asan_dbg_dynamic-x86_64.lib"
  # "${VC_TOOLS_DIR}/lib/x64/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
  )
  else()
  target_link_libraries(luce PRIVATE
  "${VC_TOOLS_DIR}/lib/x64/clang_rt.asan_dynamic-x86_64.lib"
  # "${VC_TOOLS_DIR}/lib/x64/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
  )
  endif()
===]]