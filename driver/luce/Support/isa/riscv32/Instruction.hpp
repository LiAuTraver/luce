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
namespace accat::luce {
class CentralProcessingUnit;
}
namespace accat::luce::isa::riscv32 {
struct Instruction : Word {

public:
  Instruction() noexcept = default;
  explicit Instruction(num_type num) noexcept : Word(num) {}
  explicit Instruction(bytes_type bytes) noexcept : Word(bytes) {}
  explicit Instruction(bits_type bits) noexcept : Word(bits) {}
  Instruction(const std::span<const std::byte, 4> bytes) noexcept
      : Word(bytes) {}
  Instruction(const Instruction &) = delete;
  Instruction &operator=(const Instruction &) = delete;
  Instruction(Instruction &&) = default;
  Instruction &operator=(Instruction &&) = default;
  virtual ~Instruction() = default;
  virtual void execute(CentralProcessingUnit *) const = 0;
};

} // namespace accat::luce::isa::riscv32
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

namespace accat::luce::isa::riscv32::instruction {
#pragma push_macro("ACOE")
#define ACOE()                                                                 \
  using base_type = Instruction;                                               \
  using base_type::base_type;                                                  \
  virtual void execute(CentralProcessingUnit *) const override
// clang-format off
struct Add    : Instruction, mixin::RFormat { ACOE(); };
struct Sub    : Instruction, mixin::RFormat { ACOE(); };
struct Xor    : Instruction, mixin::RFormat { ACOE(); };
struct Or     : Instruction, mixin::RFormat { ACOE(); };
struct And    : Instruction, mixin::RFormat { ACOE(); };
struct Sll    : Instruction, mixin::RFormat { ACOE(); };
struct Srl    : Instruction, mixin::RFormat { ACOE(); };
struct Sra    : Instruction, mixin::RFormat { ACOE(); };
struct Slt    : Instruction, mixin::RFormat { ACOE(); };
struct Sltu   : Instruction, mixin::RFormat { ACOE(); };
struct Addi   : Instruction, mixin::IFormat { ACOE(); };
struct Xori   : Instruction, mixin::IFormat { ACOE(); };
struct Ori    : Instruction, mixin::IFormat { ACOE(); };
struct Andi   : Instruction, mixin::IFormat { ACOE(); };
struct Slli   : Instruction, mixin::IFormat { ACOE(); };
struct Srli   : Instruction, mixin::IFormat { ACOE(); };
struct Srai   : Instruction, mixin::IFormat { ACOE(); };
struct Slti   : Instruction, mixin::IFormat { ACOE(); };
struct Sltiu  : Instruction, mixin::IFormat { ACOE(); };
struct Lb     : Instruction, mixin::IFormat { ACOE(); };
struct Lh     : Instruction, mixin::IFormat { ACOE(); };
struct Lw     : Instruction, mixin::IFormat { ACOE(); };
struct Lbu    : Instruction, mixin::IFormat { ACOE(); };
struct Lhu    : Instruction, mixin::IFormat { ACOE(); };
struct Sb     : Instruction, mixin::SFormat { ACOE(); };
struct Sh     : Instruction, mixin::SFormat { ACOE(); };
struct Sw     : Instruction, mixin::SFormat { ACOE(); };
struct Beq    : Instruction, mixin::BFormat { ACOE(); };
struct Bne    : Instruction, mixin::BFormat { ACOE(); };
struct Blt    : Instruction, mixin::BFormat { ACOE(); };
struct Bge    : Instruction, mixin::BFormat { ACOE(); };
struct Bltu   : Instruction, mixin::BFormat { ACOE(); };
struct Bgeu   : Instruction, mixin::BFormat { ACOE(); };
struct Jal    : Instruction, mixin::JFormat { ACOE(); };
struct Jalr   : Instruction, mixin::IFormat { ACOE(); };
struct Lui    : Instruction, mixin::UFormat { ACOE(); };
struct Auipc  : Instruction, mixin::UFormat { ACOE(); };
struct Ecall  : Instruction, mixin::IFormat { ACOE(); };
struct Ebreak : Instruction, mixin::IFormat { ACOE(); };
// clang-format on
#undef ACOE
#pragma pop_macro("ACOE")
} // namespace accat::luce::isa::riscv32::instruction

