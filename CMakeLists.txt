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

if(DEFINED ENV{AC_CPP_DEBUG})
  if($ENV{AC_CPP_DEBUG} STREQUAL "ON")
    message(STATUS "Debug mode is ON. Corresponding macro features will be enabled: AC_CPP_DEBUG")
    set(AC_CPP_DEBUG ON CACHE BOOL "Debug mode is ON" FORCE)
    set(VCPKG_MANIFEST_MODE OFF)
    set(VCPKG_MANIFEST_INSTALL OFF)
  endif()
endif()

if(AC_CPP_DEBUG)
  include(cmake/flags.cmake)
endif()

include(cmake/dependencies.cmake)
include(cmake/llvm.cmake)

include_directories(driver)
include_directories(include)

add_subdirectory(driver)
add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(demo)
