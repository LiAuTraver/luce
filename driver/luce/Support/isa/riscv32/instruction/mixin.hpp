#include "../../Word.hpp"
#include "../../IInstruction.hpp"
#include "Support/isa/constants/riscv32.hpp"
namespace accat::luce::isa::riscv32::instruction::mixin {

// we choose not to use CRTP but deducing this.
// Here, it's not `Static Interface`(cannot use deducing this)
// but `Mixin`(explicit object parameter can help).

#define AC_MIXIN_DECLARE_BEGIN(_name_, ...)                                    \
  struct AC_EMPTY_BASES AC_NOVTABLE _name_ __VA_OPT__( :) __VA_ARGS__ {
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
template <size_t Base = 16> auto opcode_str(this auto &&self) noexcept {
  return self.template formatBits<decltype(self.opcode())>(self.opcode());
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(imm2StrCommon)
template <size_t Align> static consteval const char *getfmtStr() {
  static_assert(Align > 0 && Align <= 32, "Invalid alignment");
  if constexpr (Align == 12) {
    return "{:#03x}";
  } else if constexpr (Align == 20) {
    return "{:#05x}";
  } else {
    return "{:#08x}";
  }
}
template <size_t Align> auto imm_str_impl(this auto &&self) noexcept {
  constexpr auto fmtStr = getfmtStr<Align>();
  return fmt::format(fmtStr, self.imm());
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
                       opcode0To6Common,
                       imm2StrCommon)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<instruction_size_t, 20, 32>(
      self.num()); // [7, 12) and
                   // [25, 32)
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<12>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(SFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3nd12To15Common,
                       opcode0To6Common,
                       imm2StrCommon)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<instruction_size_t, 7, 12, 25, 32>(
      self.num()); // [7, 12) and
                   // [25, 32)
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<12>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(BFormat,
                       rs2nd20To25Common,
                       rs1st15To20Common,
                       funct3nd12To15Common,
                       opcode0To6Common,
                       imm2StrCommon)
auto imm(this auto &&self) noexcept {
  return self
      .template concatBits<instruction_size_t, 8, 12, 25, 31, 7, 8, 31, 32>(
          self.num());
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<12>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(UFormat, rd7To12Common, opcode0To6Common, imm2StrCommon)
auto imm(this auto &&self) noexcept {
  return self.template concatBits<instruction_size_t, 12, 32>(self.num());
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<20>();
}
AC_MIXIN_DECLARE_END

AC_MIXIN_DECLARE_BEGIN(JFormat, rd7To12Common, opcode0To6Common, imm2StrCommon)
auto imm(this auto &&self) noexcept {
  return self
      .template concatBits<instruction_size_t, 21, 31, 20, 21, 12, 20, 31, 32>(
          self.num());
}
auto imm_str(this auto &&self) noexcept {
  return self.template imm_str_impl<20>();
}
AC_MIXIN_DECLARE_END
} // namespace accat::luce::isa::riscv32::instruction::mixin

