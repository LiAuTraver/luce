#include "deps.hh"

#include <capstone/capstone.h>
#include <accat/auxilia/details/macros.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/MC/MCDisassembler/MCDisassembler.h>
#include <llvm/MC/MCObjectFileInfo.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/MC/MCDisassembler/MCDisassembler.h>
#include <llvm/MC/MCInst.h>
#include <llvm/MC/MCInstPrinter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/MC/MCTargetOptions.h>
#include <llvm/MC/MCTargetOptionsCommandFlags.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCInstrInfo.h>

void captsone_demo() {
  csh handle;
  cs_insn *insn; // Array to hold disassembled instructions
  size_t count;  // instructions count

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
    std::cerr << "Failed to initialize Capstone" << '\n';
    return;
  }
  defer { cs_close(&handle); };
  cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON); // Enable instruction details

  // Disassemble the code buffer
  count = cs_disasm(handle,
                    code.data(),
                    code.size(),
                    0x80000000,
                    0,
                    &insn); // 0x1000: Base address
  if (count > 0) {
    for (size_t i = 0; i < count; i++) {
      std::cout << "Address: 0x" << std::hex << insn[i].address
                << " | Mnemonic: " << insn[i].mnemonic
                << " | Operands: " << insn[i].op_str << '\n';
    }
    cs_free(insn, count); // Free memory allocated by Capstone
  } else {
    cs_err error = cs_errno(handle);
    std::cerr << "Disassembly failed:\n"
              << "  Error code: " << static_cast<int>(error) << "\n"
              << "  Error message: " << cs_strerror(error) << "\n"
              << "  Input size: " << code.size() << '\n';
  }
  std::cout << std::endl;
  return;
}

void llvm_mc_demo() {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllDisassemblers();
  std::string error;
  auto triple = llvm::Triple("riscv32");
  auto *target = llvm::TargetRegistry::lookupTarget(triple.getTriple(), error);
  if (!target) {
    std::cerr << "Failed to lookup target: " << error << '\n';
    return;
  }
  auto cpu = "generic";
  auto features = "";
  auto options = llvm::MCTargetOptions{};
  auto sti = target->createMCSubtargetInfo(triple.getTriple(), cpu, features);
  auto mri = target->createMCRegInfo(triple.getTriple());
  auto asm_info = target->createMCAsmInfo(*mri, triple.getTriple(), options);
  auto ctx = llvm::MCContext(triple, asm_info, mri, nullptr);
  auto disassembler = target->createMCDisassembler(*sti, ctx);
  auto instruction_info = target->createMCInstrInfo();
  auto instruction_printer = target->createMCInstPrinter(
      triple, 0, *asm_info, *instruction_info, *mri);
  // clang-format off
  llvm::ArrayRef<uint8_t> code = {
                                0x97, 0x02, 0x00, 0x00,
                                0x23, 0x88, 0x02, 0x00,
                                0x01, 0xC5, 0x02, 0x03,
                                0x73, 0x01, 0x10, 0x00,
                                0xef, 0xbe, 0xad, 0xde,
                                };
  // clang-format on
  uint64_t addr = 0x80000000; // Base address
  while (addr < 0x80000000 + code.size()) {
    auto inst = llvm::MCInst{};
    uint64_t size;

    auto result = disassembler->getInstruction(
        inst, size, code.slice(addr - 0x80000000), addr, llvm::nulls());
    if (result == llvm::MCDisassembler::Success) {
      llvm::outs() << "Address: " << llvm::format_hex(addr, 10) << " | ";
      instruction_printer->printInst(&inst, addr, "", *sti, llvm::outs());
      llvm::outs() << "\n";
      addr += size;
    } else {
      llvm::errs() << "Failed to disassemble instruction at "
                   << llvm::format_hex(addr, 10) << "\n"
                   << "Raw bytes: ";
      fmt::println("{:#04x}",
                   fmt::join(code.slice(addr - 0x80000000, 4), ", "));
      break;
    }
  }
}
int main() {
  std::cout << "Capstone demo" << '\n';
  captsone_demo();
  std::cout << "LLVM MC demo" << '\n';
  llvm_mc_demo();
  return 0;
}
