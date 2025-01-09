project(demo
  LANGUAGES C CXX # C needed for LLVM
  DESCRIPTION "playground, for testing purposes"
)

include(${LUCE_PROJECT_ROOT_DIR}/cmake/dependencies.cmake)
find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
add_link_options(/INCREMENTAL:NO)
include(HandleLLVMOptions)
add_definitions(${LLVM_DEFINITIONS})
add_executable(demo
  demo.cpp
)
target_link_libraries(demo PUBLIC
  external_deps
)

target_precompile_headers(demo PUBLIC
  ${LUCE_EXTERNAL_DEPS_PCH}
)

target_include_directories(demo PUBLIC
  ${LLVM_INCLUDE_DIRS}
)
llvm_map_components_to_libnames(llvm_libs
  # RISCV Components
  RISCVInfo
  RISCVDesc
  RISCVCodeGen
  RISCVAsmParser
  RISCVDisassembler
  # X86 Components
  X86Info
  X86Desc
  X86CodeGen
  X86AsmParser
  X86Disassembler
  # Common Components
  IRReader
  MC
  MCParser
  MCDisassembler
  Object
  OrcJIT
  ExecutionEngine
  Analysis
  Passes
  TransformUtils
  BitWriter
  BitReader
  AsmParser
)
target_link_libraries(demo
  PUBLIC 
  ${llvm_libs}
)
add_folder(Demo)