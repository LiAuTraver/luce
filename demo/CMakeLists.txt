project(demo
  LANGUAGES C CXX # C needed for LLVM
  DESCRIPTION "playground, for testing purposes"
)
find_package(concurrencpp CONFIG REQUIRED)
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

add_executable(demo2
  demo2.cpp
)
target_link_libraries(demo2 PUBLIC
  external_deps
  $<TARGET_OBJECTS:driver>
  Support.isa
)
target_precompile_headers(demo2 PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)
add_llvm_deps_for(demo2)

add_executable(demo3
  demo3.cpp
)
target_link_libraries(demo3 PUBLIC
  external_deps
  concurrencpp::concurrencpp
)
target_precompile_headers(demo3 PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)


add_folder(Demo)