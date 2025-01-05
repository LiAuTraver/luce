project(demo
  LANGUAGES CXX
  DESCRIPTION "playground, for testing purposes"
)

include(${LUCE_PROJECT_ROOT_DIR}/cmake/dependencies.cmake)

add_executable(demo
  demo.cpp
)

target_link_libraries(demo PUBLIC
  external_deps
)

target_precompile_headers(demo PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)

add_folder(Demo)