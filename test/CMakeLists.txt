project(tests
  LANGUAGES CXX
  DESCRIPTION "unit tests for luce project"
)

enable_testing()
include(CTest)
include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

include(folders)
include(dependencies)
include(llvm)

add_library(test_deps INTERFACE)

target_link_libraries(test_deps INTERFACE
  GTest::gtest
  GTest::gtest_main
  external_deps
)

target_compile_definitions(test_deps INTERFACE GTEST_CREATE_SHARED_LIBRARY=1)

function(create_test_executable test_name)
  add_executable(${test_name} ${ARGN})
  target_link_libraries(${test_name} PRIVATE
    $<TARGET_OBJECTS:driver>
    test_deps
  )
  gtest_discover_tests(${test_name})
endfunction(create_test_executable)

create_test_executable(
  luce.test
  endian.test.cpp
  memory.load.test.cpp
  expr.test.cpp
  decoder.test.cpp
  rawbin.test.cpp
)
add_folder(Test)