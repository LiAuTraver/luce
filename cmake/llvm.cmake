include_guard()

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})

add_link_options(/INCREMENTAL:NO)
include(HandleLLVMOptions)
add_definitions(${LLVM_DEFINITIONS})

macro(add_llvm_deps_for PENDING_TARGET)
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
target_include_directories(${PENDING_TARGET} PUBLIC
  ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(${PENDING_TARGET}
  PUBLIC
  ${llvm_libs}
)
endmacro()
add_folder(Utility) 