if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Bad user. Bad code.")
endif()

if(NOT DEFINED PROJECT_IS_TOP_LEVEL)
  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(PROJECT_IS_TOP_LEVEL ON)
  else()
    set(PROJECT_IS_TOP_LEVEL OFF)
  endif()
endif()

macro(is_undefined var)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "${var} is not defined")
  endif()
endmacro()

# Assert that variable is defined and non-empty
macro(nonnull MAYBE_NULL_VARIABLE)
  if((NOT DEFINED ${MAYBE_NULL_VARIABLE}) OR("${${MAYBE_NULL_VARIABLE}}" STREQUAL ""))
    message(FATAL_ERROR "${MAYBE_NULL_VARIABLE} is not defined or empty")
  endif()
endmacro()

nonnull(CMAKE_CXX_COMPILER)
message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Simulate ID: ${CMAKE_CXX_SIMULATE_ID}")

set(CXX_FLAGS_STYLES_GNU OFF CACHE BOOL "Enable GNU style C++ flags")
set(CXX_FLAGS_STYLES_CLANG OFF CACHE BOOL "Enable Clang style C++ flags")
set(CXX_FLAGS_STYLES_CLANGCL OFF CACHE BOOL "Enable Clang-CL style C++ flags")
set(CXX_FLAGS_STYLES_MSVC OFF CACHE BOOL "Enable MSVC style C++ flags")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CXX_FLAGS_STYLES_GNU ON)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if(CMAKE_CXX_COMPILER MATCHES "clang-cl")
    set(CXX_FLAGS_STYLES_CLANGCL ON)
  else()
    set(CXX_FLAGS_STYLES_CLANG ON)
  endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CXX_FLAGS_STYLES_MSVC ON)
else()
  message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

find_package(Python3 COMPONENTS Interpreter)

if(NOT Python3_Interpreter_FOUND)
  message(WARNING "Python3 interpreter was not found. dll may not be copied to the target directory.")
elseif(Python3_FOUND)
  set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
  message(STATUS "Python3 interpreter found at ${Python3_EXECUTABLE}")
else()
  message(WARNING "Python3 interpreter was found, but Python3 was not found. This may caused by venv or conda environment.")
endif()

macro(copy_dlls target deps)
  add_custom_target(copy_dlls_${target} ALL
    COMMENT "copy necessary dynamic libraries to the test directory..."
    COMMAND ${CMAKE_COMMAND} -E env ${PYTHON_EXECUTABLE} ${LUCE_PROJECT_ROOT_DIR}/scripts/copy_dlls.py $<TARGET_FILE_DIR:${deps}> $<TARGET_FILE_DIR:${target}>
    DEPENDS ${deps}
  )
  add_dependencies(${target} copy_dlls_${target})
endmacro()

set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")