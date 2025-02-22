project(demo
  LANGUAGES C CXX
  DESCRIPTION "playground, for testing purposes"
)
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
)
target_precompile_headers(demo2 PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)

add_folder(Demo)