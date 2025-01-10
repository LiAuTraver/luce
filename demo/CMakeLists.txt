project(demo
  LANGUAGES C CXX # C needed for LLVM
  DESCRIPTION "playground, for testing purposes"
)

include(${LUCE_PROJECT_ROOT_DIR}/cmake/folders.cmake)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/dependencies.cmake)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/llvm.cmake)
add_executable(demo
  demo.cpp
)
target_link_libraries(demo PUBLIC
  external_deps
)
target_precompile_headers(demo PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)
add_llvm_deps_for(demo)

add_folder(Demo)