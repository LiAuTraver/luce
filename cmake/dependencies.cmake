include_guard()

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(capstone CONFIG REQUIRED)
find_package(scn CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(auxilia QUIET)
# find_package(zstd CONFIG REQUIRED)
# find_package(ZLIB #[[CONFIG]] REQUIRED) # zlib does not provide a config file

if(NOT auxilia_FOUND)
  message(AUTHOR_WARNING "auxilia not found, using local version")
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source/auxilia/include")
endif()

add_library(external_deps INTERFACE)
target_link_libraries(external_deps INTERFACE
  fmt::fmt
  spdlog::spdlog
  argparse::argparse
  capstone::capstone
  scn::scn
  magic_enum::magic_enum
  # ZLIB::ZLIB
  # zstd::libzstd
  $<$<BOOL:${auxilia_FOUND}>:auxilia::auxilia>
)
target_compile_features(external_deps INTERFACE cxx_std_23)

set(LUCE_EXTERNAL_DEPS_PCH "${LUCE_PROJECT_ROOT_DIR}/include/deps.hh")
