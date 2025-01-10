#pragma once

#include <accat/auxilia/details/Status.hpp>

#include "luce/Pattern.hpp"
#include "luce/isa/architecture.hpp"

namespace llvm {
class Triple;
class Target;
class MCRegisterInfo;
class MCTargetOptions;
class MCAsmInfo;
class MCSubtargetInfo;
class MCInstrInfo;
class MCContext;
class MCDisassembler;
class MCInstPrinter;
} // namespace llvm
namespace accat::luce {
class Disassembler : Component {
public:
  Disassembler() = default;
  Disassembler(const Disassembler &) = delete;
  Disassembler &operator=(const Disassembler &) = delete;
  Disassembler(Disassembler &&) noexcept;
  Disassembler &operator=(Disassembler &&) noexcept;
  Disassembler(Mediator *);

  auxilia::Status set_target(isa::instruction_set);
  virtual ~Disassembler() override;

private:
  llvm::Triple *triple = nullptr;
  const llvm::Target *target = nullptr;
  llvm::MCRegisterInfo *register_info = nullptr;
  llvm::MCTargetOptions *target_options = nullptr;
  llvm::MCAsmInfo *asm_info = nullptr;
  llvm::MCSubtargetInfo *subtarget_info = nullptr;
  llvm::MCInstrInfo *instruction_info = nullptr;
  llvm::MCContext *context = nullptr;
  llvm::MCDisassembler *disassembler = nullptr;
  llvm::MCInstPrinter *instruction_printer = nullptr;

private:
  Disassembler &_do_move_impl(Disassembler &&) noexcept;
};
} // namespace accat::luce
