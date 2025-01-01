include_guard()

find_package(gtest CONFIG REQUIRED)
find_package(scn CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)


enable_testing()
add_compile_definitions(GTEST_CREATE_SHARED_LIBRARY=1)
include(CTest)
include(GoogleTest)


function(create_test_executable test_name)
  add_executable(${test_name} ${ARGN})
  target_link_libraries(${test_name} PRIVATE
          spdlog::spdlog
          fmt::fmt
          GTest::gtest
          GTest::gtest_main
          magic_enum::magic_enum
          argparse::argparse
  )
  gtest_discover_tests(${test_name})
endfunction(create_test_executable)

create_test_executable(endian_test test/endian_test.cpp)
create_test_executable(memory_load_test test/memory_load_test.cpp)