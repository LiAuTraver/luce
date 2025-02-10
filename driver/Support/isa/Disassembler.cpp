#include "deps.hh"

#include "Support/isa/Disassembler.hpp"

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
  _do_move_impl(*this, std::move(that));
}
Disassembler &Disassembler::operator=(Disassembler &&that) noexcept {
  if (this == std::addressof(that))
    return *this;
  Component::operator=(std::move(that));
  // NOLINTNEXTLINE(*-unconventional-assign-operator)
  return _do_move_impl(*this, std::move(that));
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
  target_options = new llvm::MCTargetOptions{};
  asm_info = target->createMCAsmInfo(
      *register_info, triple->getTriple(), *target_options);
  subtarget_info =
      target->createMCSubtargetInfo(triple->getTriple(), "generic", "");
  instruction_info = target->createMCInstrInfo();
  context = new llvm::MCContext(*triple, asm_info, register_info, nullptr);
  disassembler = target->createMCDisassembler(*subtarget_info, *context);
  instruction_printer = target->createMCInstPrinter(
      *triple, 0, *asm_info, *instruction_info, *register_info);

  return {};
}
Disassembler::~Disassembler() {
  delete triple;
  // delete target; // target is a pointer to a static object not a heap object
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
Disassembler &_do_move_impl(Disassembler &lhs, Disassembler &&rhs) noexcept {

  std::exchange(lhs.target, rhs.target);

  lhs.triple->~Triple();
  lhs.register_info->~MCRegisterInfo();
  lhs.target_options->~MCTargetOptions();
  lhs.asm_info->~MCAsmInfo();
  lhs.subtarget_info->~MCSubtargetInfo();
  lhs.instruction_info->~MCInstrInfo();
  lhs.context->~MCContext();
  lhs.disassembler->~MCDisassembler();
  lhs.instruction_printer->~MCInstPrinter();

  lhs.triple = rhs.triple;
  lhs.register_info = rhs.register_info;
  lhs.target_options = rhs.target_options;
  lhs.asm_info = rhs.asm_info;
  lhs.subtarget_info = rhs.subtarget_info;
  lhs.instruction_info = rhs.instruction_info;
  lhs.context = rhs.context;
  lhs.disassembler = rhs.disassembler;
  lhs.instruction_printer = rhs.instruction_printer;

  rhs.triple = nullptr;
  rhs.register_info = nullptr;
  rhs.target_options = nullptr;
  rhs.asm_info = nullptr;
  rhs.subtarget_info = nullptr;
  rhs.instruction_info = nullptr;
  rhs.context = nullptr;
  rhs.disassembler = nullptr;
  rhs.instruction_printer = nullptr;

  return lhs;
}
} // namespace accat::luce
