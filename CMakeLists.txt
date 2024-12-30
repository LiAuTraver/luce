cmake_minimum_required(VERSION 3.15)

project(luce LANGUAGES CXX)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(capstone CONFIG REQUIRED)
find_package(scn CONFIG REQUIRED)

option(AC_CPP_DEBUG "set the environment variable AC_CPP_DEBUG to enable debug mode" OFF)

if (DEFINED ENV{AC_CPP_DEBUG})
  if ($ENV{AC_CPP_DEBUG} STREQUAL "ON")
    message(STATUS "Debug mode is ON. Corresponding macro features will be enabled: DEBUG, _DEBUG, DEBUG_, _DEBUG_, AC_CPP_DEBUG")
    set(AC_CPP_DEBUG ON CACHE BOOL "Debug mode is ON" FORCE)
    set(VCPKG_MANIFEST_MODE OFF)
    set(VCPKG_MANIFEST_INSTALL OFF)
  endif ()
endif ()

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

# MUST include at the TOP level CMakelists.txt otherwise Ctest will not work!!!
# Why cmake makes it so hard to use?
enable_testing()
include(CTest)

if (AC_CPP_DEBUG)
  add_compile_definitions(
      _DEBUG DEBUG __DEBUG__ AC_CPP_DEBUG
  )
  message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(
        # MinGW does can't get through the `source_location` in file `spdlog/logger.h:334`
        SPDLOG_NO_EXCEPTIONS SPDLOG_NO_SOURCE_LOC
    )
    add_compile_options(-Wno-reserved-module-identifier
        -Wno-include-angled-in-module-purview
        -Wimplicit-fallthrough
        -Wno-deprecated-declarations
        -Wno-deprecated
        -g3
        -fvar-tracking
        -fno-inline
        -Wno-c23-extensions
    )
    add_link_options()
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
   add_compile_options(-Wno-reserved-module-identifier
        -Wno-include-angled-in-module-purview
        -Wimplicit-fallthrough
        -Wno-deprecated-declarations
        -Wno-deprecated
        -g3
        -fstandalone-debug
        -fno-inline
        -Wno-c23-extensions
    )
    add_link_options()
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/EHsc /wd4716 /wd4530 /wd4244 /Zi /Od)
    add_link_options(/DEBUG:FULL)
  else ()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
  endif ()
endif ()

file(GLOB_RECURSE SOURCES "driver/*.cpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shared)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/driver/include)

add_executable(luce)
target_sources(luce PUBLIC
    main.cpp
    ${SOURCES})
target_link_libraries(luce PUBLIC
    fmt::fmt
    spdlog::spdlog
    argparse::argparse
    capstone::capstone
    scn::scn
)

target_compile_features(luce PUBLIC cxx_std_23)
