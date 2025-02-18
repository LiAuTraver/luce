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
#include "../Word.hpp"
#include "../IInstruction.hpp"
namespace accat::luce::isa::riscv32::instruction::mixin {

// we choose not to use CRTP but deducing this.
// Here, it's not `Static Interface`(cannot use deducing this)
// but `Mixin`(explicit object parameter can help).

#define AC_MIXIN_DECLARE_BEGIN(_name_, ...)                                    \
  struct __declspec(empty_bases, novtable) _name_ __VA_OPT__( :) __VA_ARGS__ {
#define AC_MIXIN_DECLARE_END                                                   \
  }                                                                            \
  ;

AC_MIXIN_DECLARE_BEGIN(rs2nd20To25Common)
auto rs2(this auto &&self) noexcept {
  return self.template extractBits<20, 25>(self.num()); // [20, 25)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(rs1st15To20Common)
auto rs1(this auto &&self) noexcept {
  return self.template extractBits<15, 20>(self.num()); // [15, 20)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(funct3nd12To15Common)
auto funct3(this auto &&self) noexcept {
  return self.template extractBits<12, 15>(self.num()); // [12, 15)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(rd7To12Common)
auto rd(this auto &&self) noexcept {
  return self.template extractBits<7, 12>(self.num()); // [7, 12)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(opcode0To6Common)
auto opcode(this auto &&self) noexcept {
  return self.template extractBits<0, 7>(self.num()); // [0, 7)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(RFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3nd12To15Common,
                       rd7To12Common,
                       opcode0To6Common)
auto funct7(this auto &&self) noexcept {
  return self.template extractBits<25, 32>(self.num()); // [25, 32)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(IFormat,
                       rs1st15To20Common,
                       funct3nd12To15Common,
                       rd7To12Common,
                       opcode0To6Common)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<uint32_t, 7, 12, 25, 32>(
      self.num()); // [7, 12) and
                   // [25, 32)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(SFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3nd12To15Common,
                       opcode0To6Common)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<uint32_t, 7, 12, 25, 32>(
      self.num()); // [7, 12) and
                   // [25, 32)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(BFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3nd12To15Common,
                       opcode0To6Common)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<uint32_t, 8, 12, 25, 31, 7, 8, 31, 32>(
      self.num());
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(UFormat, rd7To12Common, opcode0To6Common)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<uint32_t, 12, 32, 12, 20>(self.num());
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(JFormat, rd7To12Common, opcode0To6Common)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<uint32_t, 21, 31, 20, 21, 12, 20, 31, 32>(
      self.num());
}
AC_MIXIN_DECLARE_END
} // namespace accat::luce::isa::riscv32::instruction::mixin

#if defined(Add) || defined(Xor) || defined(Or) || defined(And)
#  error "Bad user. Bad code."
#endif

namespace accat::luce::isa::riscv32::instruction::base {
#pragma push_macro("ACOE")
#define ACOE()                                                                 \
  using base_type = IInstruction;                                              \
  using base_type::base_type;                                                  \
  virtual ExecutionStatus execute(Icpu *) const override
// clang-format off
struct Add    : IInstruction, mixin::RFormat { ACOE(); };
struct Sub    : IInstruction, mixin::RFormat { ACOE(); };
struct Xor    : IInstruction, mixin::RFormat { ACOE(); };
struct Or     : IInstruction, mixin::RFormat { ACOE(); };
struct And    : IInstruction, mixin::RFormat { ACOE(); };
struct Sll    : IInstruction, mixin::RFormat { ACOE(); };
struct Srl    : IInstruction, mixin::RFormat { ACOE(); };
struct Sra    : IInstruction, mixin::RFormat { ACOE(); };
struct Slt    : IInstruction, mixin::RFormat { ACOE(); };
struct Sltu   : IInstruction, mixin::RFormat { ACOE(); };
struct Addi   : IInstruction, mixin::IFormat { ACOE(); };
struct Xori   : IInstruction, mixin::IFormat { ACOE(); };
struct Ori    : IInstruction, mixin::IFormat { ACOE(); };
struct Andi   : IInstruction, mixin::IFormat { ACOE(); };
struct Slli   : IInstruction, mixin::IFormat { ACOE(); };
struct Srli   : IInstruction, mixin::IFormat { ACOE(); };
struct Srai   : IInstruction, mixin::IFormat { ACOE(); };
struct Slti   : IInstruction, mixin::IFormat { ACOE(); };
struct Sltiu  : IInstruction, mixin::IFormat { ACOE(); };
struct Lb     : IInstruction, mixin::IFormat { ACOE(); };
struct Lh     : IInstruction, mixin::IFormat { ACOE(); };
struct Lw     : IInstruction, mixin::IFormat { ACOE(); };
struct Lbu    : IInstruction, mixin::IFormat { ACOE(); };
struct Lhu    : IInstruction, mixin::IFormat { ACOE(); };
struct Sb     : IInstruction, mixin::SFormat { ACOE(); };
struct Sh     : IInstruction, mixin::SFormat { ACOE(); };
struct Sw     : IInstruction, mixin::SFormat { ACOE(); };
struct Beq    : IInstruction, mixin::BFormat { ACOE(); };
struct Bne    : IInstruction, mixin::BFormat { ACOE(); };
struct Blt    : IInstruction, mixin::BFormat { ACOE(); };
struct Bge    : IInstruction, mixin::BFormat { ACOE(); };
struct Bltu   : IInstruction, mixin::BFormat { ACOE(); };
struct Bgeu   : IInstruction, mixin::BFormat { ACOE(); };
struct Jal    : IInstruction, mixin::JFormat { ACOE(); };
struct Jalr   : IInstruction, mixin::IFormat { ACOE(); };
struct Lui    : IInstruction, mixin::UFormat { ACOE(); };
struct Auipc  : IInstruction, mixin::UFormat { ACOE(); };
struct Ecall  : IInstruction, mixin::IFormat { ACOE(); };
struct Ebreak : IInstruction, mixin::IFormat { ACOE(); };
// clang-format on
#undef ACOE
#pragma pop_macro("ACOE")
} // namespace accat::luce::isa::riscv32::instruction


