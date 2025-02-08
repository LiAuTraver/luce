project(tests
    LANGUAGES CXX
    DESCRIPTION "unit tests for luce project"
)

enable_testing()
include(CTest)
include(GoogleTest)

find_package(GTest CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)

include(${LUCE_PROJECT_ROOT_DIR}/cmake/folders.cmake)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/dependencies.cmake)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/llvm.cmake)

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
      Support.isa
      test_deps
  )
  gtest_discover_tests(${test_name})
  #  add_llvm_deps_for(${test_name})
endfunction(create_test_executable)

create_test_executable(endian.test endian.test.cpp)
create_test_executable(memory.load.test memory.load.test.cpp)
create_test_executable(expr.test expr.test.cpp)

add_folder(Test)