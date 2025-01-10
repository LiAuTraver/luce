#include "deps.hh"

#include "luce/Disassembler.hpp"

#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCDisassembler/MCDisassembler.h>
#include <llvm/MC/MCInstPrinter.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/MCTargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Triple.h>
namespace accat::luce {
Disassembler::Disassembler(Disassembler &&that) noexcept
    : Component(std::move(that)) {
  _do_move_impl(std::move(that));
}
Disassembler &Disassembler::operator=(Disassembler &&that) noexcept {
  if (this != std::addressof(that)) {
    Component::operator=(std::move(that));
    _do_move_impl(std::move(that));
  }
  return *this;
}
Disassembler::Disassembler(Mediator *parent) : Component(parent) {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllDisassemblers();
}
auxilia::Status Disassembler::set_target(isa::instruction_set instructionSet) {
  if (instructionSet != isa::instruction_set::riscv32)
    return auxilia::UnimplementedError("Unsupported instruction set");

  triple = new llvm::Triple("riscv32");
  auto error = std::string{};
  target = llvm::TargetRegistry::lookupTarget(triple->getTriple(), error);
  if (!error.empty() or !target)
    return auxilia::InternalError("Failed to lookup target: " + error);

  register_info = target->createMCRegInfo(triple->getTriple());
  target_options = new llvm::MCTargetOptions();
  asm_info = target->createMCAsmInfo(
      *register_info, triple->getTriple(), *target_options);
  subtarget_info =
      target->createMCSubtargetInfo(triple->getTriple(), "generic", "");
  instruction_info = target->createMCInstrInfo();
  context = new llvm::MCContext(*triple, asm_info, register_info, nullptr);
  disassembler = target->createMCDisassembler(*subtarget_info, *context);
  instruction_printer = target->createMCInstPrinter(
      *triple, 0, *asm_info, *instruction_info, *register_info);

  return auxilia::OkStatus();
}
Disassembler::~Disassembler() {
  delete triple;
  delete target;
  delete register_info;
  delete target_options;
  delete asm_info;
  delete subtarget_info;
  delete instruction_info;
  delete context;
  delete disassembler;
  delete instruction_printer;
  
  dbg_block
  {
    triple = nullptr;
    target = nullptr;
    register_info = nullptr;
    target_options = nullptr;
    asm_info = nullptr;
    subtarget_info = nullptr;
    instruction_info = nullptr;
    context = nullptr;
    disassembler = nullptr;
    instruction_printer = nullptr;
  };
}
Disassembler &Disassembler::_do_move_impl(Disassembler &&that) noexcept {
  this->triple->~Triple();
  this->target->~Target();
  this->register_info->~MCRegisterInfo();
  this->target_options->~MCTargetOptions();
  this->asm_info->~MCAsmInfo();
  this->subtarget_info->~MCSubtargetInfo();
  this->instruction_info->~MCInstrInfo();
  this->context->~MCContext();
  this->disassembler->~MCDisassembler();
  this->instruction_printer->~MCInstPrinter();

  triple = that.triple;
  target = that.target;
  register_info = that.register_info;
  target_options = that.target_options;
  asm_info = that.asm_info;
  subtarget_info = that.subtarget_info;
  instruction_info = that.instruction_info;
  context = that.context;
  disassembler = that.disassembler;
  instruction_printer = that.instruction_printer;

  that.triple = nullptr;
  that.target = nullptr;
  that.register_info = nullptr;
  that.target_options = nullptr;
  that.asm_info = nullptr;
  that.subtarget_info = nullptr;
  that.instruction_info = nullptr;
  that.context = nullptr;
  that.disassembler = nullptr;
  that.instruction_printer = nullptr;

  return *this;
}
} // namespace accat::luce
