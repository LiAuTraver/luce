#pragma once
#include <accat/auxilia/defines.hpp>

#include "luce/Support/isa/constants/riscv32.hpp"

namespace accat::luce::isa::riscv32::instruction::mixin {

// we choose not to use CRTP but deducing this.
// Here, it's not `Static Interface`(cannot use deducing this)
// but `Mixin`(explicit object parameter can help).

#define AC_MIXIN_DECLARE_BEGIN(_name_, ...)                                    \
  struct AC_EMPTY_BASES AC_NOVTABLE _name_ __VA_OPT__( :) __VA_ARGS__ {
#define AC_MIXIN_DECLARE_END                                                   \
  }                                                                            \
  ;
AC_MIXIN_DECLARE_BEGIN(rs1st15To20Common)
constexpr auto rs1(this auto &&self) noexcept {
  return self.template extractBits<15, 20>(self.num()); // [15, 20)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(rs2nd20To25Common)
constexpr auto rs2(this auto &&self) noexcept {
  return self.template extractBits<20, 25>(self.num()); // [20, 25)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(funct3rd12To15Common)
constexpr auto funct3(this auto &&self) noexcept {
  return self.template extractBits<12, 15>(self.num()); // [12, 15)
}
AC_MIXIN_DECLARE_END
AC_MIXIN_DECLARE_BEGIN(funct5th27To32Common)
constexpr auto funct5(this auto &&self) noexcept {
  return self.template extractBits<27, 32>(self.num()); // [27, 32)
}
AC_MIXIN_DECLARE_END
AC_MIXIN_DECLARE_BEGIN(funct7th25To32Common)
constexpr auto funct7(this auto &&self) noexcept {
  return self.template extractBits<25, 32>(self.num()); // [25, 32)
}
AC_MIXIN_DECLARE_END
AC_MIXIN_DECLARE_BEGIN(rd7To12Common)
constexpr auto rd(this auto &&self) noexcept {
  return self.template extractBits<7, 12>(self.num()); // [7, 12)
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(opcode0To6Common)
constexpr auto opcode(this auto &&self) noexcept {
  return self.template extractBits<0, 7>(self.num()); // [0, 7)
}
template <size_t Base = 16>
constexpr auto opcode_str(this auto &&self) noexcept {
  return self.template formatBits<decltype(self.opcode())>(self.opcode());
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(imm2StrCommon)
protected:
template <size_t Align> static consteval const char *getfmtStr() {
  static_assert(Align > 0 && Align <= 32, "Invalid alignment");
  if constexpr (Align == 12) {
    return "{:#03x}"; // I-type, S-type
  } else if constexpr (Align == 13) {
    return "{:#04x}"; // B-type (12 + 1)
  } else if constexpr (Align == 21) {
    return "{:#06x}"; // J-type (20 + 1)
  } else if constexpr (Align == 32) {
    return "{:#08x}"; // U-type (20 + 12)
  } else {
    static_assert(false, "not implemented yet");
  }
}
template <size_t Align> auto imm_str_impl(this auto &&self) noexcept {
  constexpr auto fmtStr = getfmtStr<Align>();
  return fmt::format(fmtStr, self.template decode2sComplement<>(self.imm()));
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(RFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3rd12To15Common,
                       rd7To12Common,
                       opcode0To6Common,
                       funct7th25To32Common)
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(IFormat,
                       rs1st15To20Common,
                       funct3rd12To15Common,
                       rd7To12Common,
                       opcode0To6Common,
                       imm2StrCommon)
constexpr auto imm(this auto &&self) noexcept {
  auto real_imm = self.template concatBits<instruction_size_t, 20, 32>(
      self.num()); // [7, 12) and [25, 32)

  // sign-extend 12 bits to 32 bits
  return self.template signExtend<12>(real_imm);
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<12>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(SFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3rd12To15Common,
                       opcode0To6Common,
                       imm2StrCommon)
constexpr auto imm(this auto &&self) noexcept {
  auto real_imm = self.template concatBits<instruction_size_t, 7, 12>(
      self.num()); // [7, 12) and [25, 32)
  // sign-extend 12 bits to 32 bits
  return self.template signExtend<12>(real_imm);
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<12>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(BFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3rd12To15Common,
                       opcode0To6Common,
                       imm2StrCommon)
auto imm(this auto &&self) noexcept {
  auto real_imm = self.template concatBits<instruction_size_t,
                                           8,
                                           12,
                                           25,
                                           31,
                                           7,
                                           8,
                                           31,
                                           32>(self.num())
                  << 1; // imm[0] is implicitly 0
  // sign-extend 13 bits to 32 bits
  return self.template signExtend<13>(real_imm);
}

protected:
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<12>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(UFormat, rd7To12Common, opcode0To6Common, imm2StrCommon)
auto imm(this auto &&self) noexcept {
  auto real_imm = self.template concatBits<instruction_size_t, 12, 32>(
      self.num()); // [12, 32)
  // sign-extend 20 bits to 32 bits
  return self.template signExtend<20>(real_imm);
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<32>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(JFormat, rd7To12Common, opcode0To6Common, imm2StrCommon)
auto imm(this auto &&self) noexcept {
  auto real_imm = self.template concatBits<instruction_size_t,
                                           21,
                                           31,
                                           20,
                                           21,
                                           12,
                                           20,
                                           31,
                                           32>(self.num())
                  << 1; // imm[0] is implicitly 0

  // sign-extend 21 bits to 32 bits
  return self.template signExtend<21>(real_imm);
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<21>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(ARFormat,
                       mixin::rs2nd20To25Common,
                       mixin::rs1st15To20Common,
                       mixin::funct3rd12To15Common,
                       funct5th27To32Common,
                       mixin::rd7To12Common,
                       mixin::opcode0To6Common)
constexpr auto rl(this auto &&self) noexcept {
  return self.template extractBits<25, 26>(self.num());
}
constexpr auto aq(this auto &&self) noexcept {
  return self.template extractBits<26, 27>(self.num());
}
AC_MIXIN_DECLARE_END

} // namespace accat::luce::isa::riscv32::instruction::mixin
