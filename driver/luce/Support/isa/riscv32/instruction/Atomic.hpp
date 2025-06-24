#pragma once

#include "luce/Support/isa/IDecoder.hpp"
#include "details/mixin.hpp"

/// @note currently this has no usage since we only have one cpu.
namespace accat::luce::isa::riscv32::instruction::atomic {
#define AC_UNDEF_YOUR_MACRO
#include "details/debunk_your_macro-inl.hpp"

INST(Lr, AR);
INST(Sc, AR);
INST(Swap, AR);
INST(Add, AR);
INST(And, AR);
INST(Or, AR);
INST(Xor, AR);
INST(Max, AR);
INST(Min, AR);

INST_DECODER();

#define AC_RESTORE_YOUR_MACRO
#include "details/debunk_your_macro-inl.hpp"

} // namespace accat::luce::isa::riscv32::instruction::atomic
