project(driver
  LANGUAGES C CXX
  DESCRIPTION "shared items between luce project"
)

include(folders)
include(dependencies)
include(llvm)

add_library(driver OBJECT)
file(GLOB_RECURSE LUCE_DRIVER_SOURCES "src/*.cpp")
file(GLOB_RECURSE LUCE_HEADER_SOURCES "luce/*.hpp")

target_compile_features(driver PUBLIC cxx_std_23)
target_sources(driver PUBLIC
  luce_main.cpp
  exec.hpp
  DllMain.cpp
  ${LUCE_DRIVER_SOURCES}
  ${LUCE_HEADER_SOURCES}
)
target_link_libraries(driver PUBLIC
  external_deps
)
target_precompile_headers(driver PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)

add_folder(Support)
