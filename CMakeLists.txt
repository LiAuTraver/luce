cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/ports")
project(luce LANGUAGES CXX VERSION 0.1.0 DESCRIPTION "A simple riscv64 emulator" )

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(capstone CONFIG REQUIRED)
find_package(scn CONFIG REQUIRED)
find_package(auxilia CONFIG REQUIRED)
option(AC_CPP_DEBUG "set the environment variable AC_CPP_DEBUG to enable debug mode" OFF)

if(DEFINED ENV{AC_CPP_DEBUG})
  if($ENV{AC_CPP_DEBUG} STREQUAL "ON")
    message(STATUS "Debug mode is ON. Corresponding macro features will be enabled: DEBUG, _DEBUG, DEBUG_, _DEBUG_, AC_CPP_DEBUG")
    set(AC_CPP_DEBUG ON CACHE BOOL "Debug mode is ON" FORCE)
    set(VCPKG_MANIFEST_MODE OFF)
    set(VCPKG_MANIFEST_INSTALL OFF)
  endif()
endif()

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

# MUST include at the TOP level CMakelists.txt otherwise Ctest will not work
enable_testing()
include(CTest)

add_executable(luce)

if(AC_CPP_DEBUG)
  add_compile_definitions(
    _DEBUG DEBUG __DEBUG__ AC_CPP_DEBUG
  )
  message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-reserved-module-identifier
      -Wno-include-angled-in-module-purview
      -Wimplicit-fallthrough
      -Wno-deprecated-declarations
      -Wno-deprecated
      -g3
      -fstandalone-debug
      -fno-inline
      -Wno-c23-extensions

      -fsanitize=address
      -fsanitize=undefined
    )
    add_link_options(
      -fsanitize=address
      -fno-omit-frame-pointer
    )

  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      message(FATAL_ERROR "GCC is not supported on Windows")
    endif()
  else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif()
endif()

file(GLOB_RECURSE LUCE_DRIVER_SOURCES "driver/src/*.cpp")
file(GLOB_RECURSE LUCE_HEADER_SOURCES "driver/luce/*.hpp")
list(APPEND LUCE_DRIVER_SOURCES "driver/luce_main.cpp")
list(APPEND LUCE_HEADER_SOURCES "driver/exec.hpp")

include_directories(driver)
include_directories(include)

target_sources(luce PUBLIC
  tools/luce/alter_args-inl.h
  tools/luce/init-inl.hpp
  tools/luce/main.cpp
  ${LUCE_DRIVER_SOURCES}
  ${LUCE_HEADER_SOURCES}
)
target_link_libraries(luce PUBLIC
  fmt::fmt
  spdlog::spdlog
  argparse::argparse
  capstone::capstone
  scn::scn
  auxilia::auxilia
)
target_precompile_headers(luce PUBLIC
  include/deps.hh
)
target_compile_features(luce PUBLIC cxx_std_23)

include(cmake/test.cmake)

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