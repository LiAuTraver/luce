#include "deps.hh"

#include <capstone/capstone.h>
#include <accat/auxilia/details/macros.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
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
#include <llvm/MC/MCSubtargetInfo.h>
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
  defer {
    cs_close(&handle);
  };
  cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON); // Enable instruction details

  // Disassemble the code buffer
  count = cs_disasm(handle,
                    code.data(),
                    code.size(),
                    0x80000000,
                    0,
                    &insn); // 0x1000: Base address
  if (count > 0) {
    #pragma unroll
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
AC_NO_SANITIZE_ADDRESS
void llvm_mc_demo() {
  // clang-format off
llvm::ArrayRef<uint8_t> code = 
    {
    0x97, 0x02, 0x00, 0x00,
    0x23, 0x88, 0x02, 0x00,
    0x01, 0xC5, 0x02, 0x03,
    0x73, 0x01, 0x10, 0x00,
    0xef, 0xbe, 0xad, 0xde,
    };
  // clang-format on
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllDisassemblers();
  auto error = std::string{};
  auto triple = llvm::Triple("riscv32");
  auto *target = llvm::TargetRegistry::lookupTarget(triple.getTriple(), error);
  if (!error.empty() or !target) {
    std::cerr << "Failed to lookup target: " << error << '\n';
    return;
  }

  auto mri = std::unique_ptr<llvm::MCRegisterInfo>(
      target->createMCRegInfo(triple.getTriple()));

  auto options = std::make_unique<llvm::MCTargetOptions>();

  auto asm_info = std::unique_ptr<llvm::MCAsmInfo>(
      target->createMCAsmInfo(*mri, triple.getTriple(), *options));

  auto sti = std::unique_ptr<llvm::MCSubtargetInfo>(
      target->createMCSubtargetInfo(triple.getTriple(), "generic", ""));

  auto instruction_info =
      std::unique_ptr<llvm::MCInstrInfo>(target->createMCInstrInfo());

  // Context needs to outlive the disassembler
  auto ctx = std::make_unique<llvm::MCContext>(
      triple, asm_info.get(), mri.get(), nullptr);

  auto disassembler = std::unique_ptr<llvm::MCDisassembler>(
      target->createMCDisassembler(*sti, *ctx));

  auto instruction_printer =
      std::unique_ptr<llvm::MCInstPrinter>(target->createMCInstPrinter(
          triple, 0, *asm_info, *instruction_info, *mri));

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

      auto opCode = inst.getOpcode();
      const auto& desc = instruction_info->get(opCode);
      auto name = instruction_info->getName(opCode);
      llvm::outs() << "  Opcode: " << opCode << "\n";
      llvm::outs() << "  Name: " << name << "\n";
      llvm::outs() << "  Description: " << desc.TSFlags << "\n";
      // here we can access more information about the instruction...
      // TODO
    } else {
      llvm::errs() << "Failed to disassemble instruction at "
                   << llvm::format_hex(addr, 10) << "\n"
                   << "Raw bytes: ";
      fmt::println("{:#04x}",
                   fmt::join(code.slice(addr - 0x80000000, 4), ", "));
      break;
    }
  }

  return;
}
int main() {
  std::cout << "Capstone demo" << '\n';
  captsone_demo();
  std::cout << "LLVM MC demo" << '\n';
  llvm_mc_demo();
  return 0;
}
