#pragma once

#include "./architecture.hpp"
#include "luce/utils/Pattern.hpp"

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
namespace accat::auxilia {
class Status;
}
namespace accat::luce {
class LUCE_SUPPORT_ISA_API [[gsl::Pointer]] Disassembler : Component {
  friend Disassembler &_do_move_impl(Disassembler &, Disassembler &&) noexcept;

public:
  Disassembler() = default;
  Disassembler(const Disassembler &) = delete;
  Disassembler &operator=(const Disassembler &) = delete;
  Disassembler(Disassembler &&) noexcept;
  Disassembler &operator=(Disassembler &&) noexcept;
  explicit Disassembler(Mediator *);

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
};
} // namespace accat::luce
