#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <bit>
#include <memory>
#include <span>
#include <type_traits>

#include "mixin.hpp"

namespace accat::luce::isa::riscv32::instruction::base {
// don't ask
#define AC_UNDEF_YOUR_FXXKING_MACRO
#include "debunk_your_macro-inl.hpp"

INST(Add, R);
INST(Sub, R);
INST(Xor, R);
INST(Or, R);
INST(And, R);
INST(Sll, R);
INST(Srl, R);
INST(Sra, R);
INST(Slt, R);
INST(Sltu, R);
INST(Addi, I);
INST(Xori, I);
INST(Ori, I);
INST(Andi, I);
INST(Slli, I);
INST(Srli, I);
INST(Srai, I);
INST(Slti, I);
INST(Sltiu, I);
INST(Lb, I);
INST(Lh, I);
INST(Lw, I);
INST(Lbu, I);
INST(Lhu, I);
INST(Sb, S);
INST(Sh, S);
INST(Sw, S);
INST(Beq, B);
INST(Bne, B);
INST(Blt, B);
INST(Bge, B);
INST(Bltu, B);
INST(Bgeu, B);
INST(Jal, J);
INST(Jalr, I);
INST(Lui, U);
INST(Auipc, U);
INST(Ecall, I);
INST(Ebreak, I);

// restore your meow meow meow
#define AC_RESTORE_YOUR_FXXKING_MACRO
#include "debunk_your_macro-inl.hpp"

} // namespace accat::luce::isa::riscv32::instruction::base
