include_guard()

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(auxilia QUIET)

if(NOT auxilia_FOUND)
  message(AUTHOR_WARNING "auxilia not found, using local version")
  add_subdirectory(${LUCE_PROJECT_ROOT_DIR}/source/auxilia)
endif()

add_library(external_deps INTERFACE)
target_link_libraries(external_deps INTERFACE
  fmt::fmt
  spdlog::spdlog
  argparse::argparse
  auxilia::auxilia
)
target_compile_features(external_deps INTERFACE cxx_std_23)

set(LUCE_EXTERNAL_DEPS_PCH "${LUCE_PROJECT_ROOT_DIR}/include/deps.hh")
