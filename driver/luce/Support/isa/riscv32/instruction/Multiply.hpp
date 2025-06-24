#pragma once

#include "luce/Support/isa/IDecoder.hpp"
#include "details/mixin.hpp"

namespace accat::luce::isa::riscv32::instruction::multiply {
// don't ask
#define AC_UNDEF_YOUR_MACRO
#include "details/debunk_your_macro-inl.hpp"

INST(Mul, R);
INST(Mulh, R);
INST(Mulsu, R);
INST(Mulu, R);
INST(Div, R);
INST(Divu, R);
INST(Rem, R);
INST(Remu, R);

INST_DECODER();

#define AC_RESTORE_YOUR_MACRO
#include "details/debunk_your_macro-inl.hpp"

} // namespace accat::luce::isa::riscv32::instruction::multiply
