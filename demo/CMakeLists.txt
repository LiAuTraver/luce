project(demo
  LANGUAGES C CXX # C needed for LLVM
  DESCRIPTION "playground, for testing purposes"
)
find_package(concurrencpp CONFIG REQUIRED)
include(folders)
include(dependencies)
include(llvm)
add_executable(demo
  demo.cpp
)
target_link_libraries(demo PUBLIC
  external_deps
)
target_precompile_headers(demo PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)
add_executable(demo2
  demo2.cpp
)
target_link_libraries(demo2 PUBLIC
  external_deps
  $<TARGET_OBJECTS:driver>
  # Support.isa
)
target_precompile_headers(demo2 PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)
# add_executable(demo3
#   demo3.cpp
# )
# target_link_libraries(demo3 PUBLIC
#   external_deps
# )
# target_precompile_headers(demo3 PUBLIC
#   ${LUCE_EXTERNAL_DEPS_PCH}
# )

add_folder(Demo)