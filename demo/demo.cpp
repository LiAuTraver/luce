#include "deps.hh"
#include <capstone/capstone.h>
#include <accat/auxilia/details/macros.hpp>
#include <iostream>
#include <vector>

int main() {
  csh handle;    // Capstone handle
  cs_insn *insn; // Array to hold disassembled instructions
  size_t count;  // Number of instructions

  // Fetched instructions as raw machine code (RISC-V)
  // NOTE: little-endian byte order!
  std::vector<uint8_t> code = {
      0x97, 0x02, 0x00, 0x00, // Instruction 1
      0x23, 0x88, 0x02, 0x00, // Instruction 2
      0x01, 0xC5, 0x02, 0x03, // Instruction 3
      0x73, 0x01, 0x10, 0x00, // Instruction 4
      0xef, 0xbe, 0xad, 0xde  // Invalid instruction
  };

  // Set the architecture to RISC-V and 32-bit mode
  if (cs_open(CS_ARCH_RISCV, CS_MODE_RISCV32, &handle) != CS_ERR_OK) {
    std::cerr << "Failed to initialize Capstone" << std::endl;
    return -1;
  }
  defer {
    cs_close(&handle); // Close Capstone handle
  };
  cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON); // Enable instruction details

  // Disassemble the code buffer
  count = cs_disasm(handle,
                    code.data(),
                    code.size(),
                    0x1000,
                    0,
                    &insn); // 0x1000: Base address
  if (count > 0) {
    for (size_t i = 0; i < count; i++) {
      std::cout << "[info] Address: 0x" << std::hex << insn[i].address
                << " | Mnemonic: " << insn[i].mnemonic
                << " | Operands: " << insn[i].op_str << std::endl;
    }
    cs_free(insn, count); // Free memory allocated by Capstone
  } else {
    cs_err error = cs_errno(handle);
    std::cerr << "Disassembly failed:\n"
              << "  Error code: " << static_cast<int>(error) << "\n"
              << "  Error message: " << cs_strerror(error)
              << "\n"
              // << "  Handle valid: " << (handle != nullptr) << "\n"
              << "  Input size: " << code.size() << std::endl;
  }

  return 0;
}
