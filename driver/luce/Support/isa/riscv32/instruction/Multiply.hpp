#pragma once

#include "luce/Support/isa/IDecoder.hpp"
#include "mixin.hpp"

namespace accat::luce::isa::riscv32::instruction::multiply {
// don't ask
#define AC_UNDEF_YOUR_FXXKING_MACRO
#include "debunk_your_macro-inl.hpp"

INST(Mul, R);
INST(Mulh, R);
INST(Mulsu, R);
INST(Mulu, R);
INST(Div, R);
INST(Divu, R);
INST(Rem, R);
INST(Remu, R);

#define AC_RESTORE_YOUR_FXXKING_MACRO
#include "debunk_your_macro-inl.hpp"

class Decoder : public IDecoder {
public:
  virtual auto decode(uint32_t) -> std::unique_ptr<IInstruction> override;
  virtual ~Decoder() override = default;
};
} // namespace accat::luce::isa::riscv32::instruction::multiply
