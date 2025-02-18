
#include "accat/auxilia/details/format.hpp"
#include "accat/auxilia/details/views.hpp"
#include "luce/Support/isa/architecture.hpp"
#include "luce/Support/isa/riscv32/Register.hpp"
namespace accat::luce::isa::riscv32 {
auto GeneralPurposeRegisters::to_string(
    const auxilia::FormatPolicy policy) const -> string_type {
  auto str = string_type{};
  constexpr auto D = [](auto &&reg) AC_STATIC_CALL_OPERATOR constexpr {
    return reg.to_string(kDetailed);
  };
  using auxilia::operator""_raw;
  const auto &R = registers;
  if (policy == kDefault) {
    // reg number, short name, 0x00000000
    str = fmt::format(R"(
x0 (zero): {}
x1 (ra)  : {}
x2 (sp)  : {}
x3 (gp)  : {}
x4 (tp)  : {}
x5 (t0)  : {}
x6 (t1)  : {}
x7 (t2)  : {}
x8 (s0)  : {}
x9 (s1)  : {}
x10 (a0) : {}
x11 (a1) : {}
x12 (a2) : {}
x13 (a3) : {}
x14 (a4) : {}
x15 (a5) : {}
x16 (a6) : {}
x17 (a7) : {}
x18 (s2) : {}
x19 (s3) : {}
x20 (s4) : {}
x21 (s5) : {}
x22 (s6) : {}
x23 (s7) : {}
x24 (s8) : {}
x25 (s9) : {}
x26 (s10): {}
x27 (s11): {}
x28 (t3) : {}
x29 (t4) : {}
x30 (t5) : {}
x31 (t6) : {}
)"_raw, // remove leading and trailing newline
                      D(R.zero_reg),
                      D(R.ra),
                      D(R.sp),
                      D(R.gp),
                      D(R.tp),
                      D(R.t0),
                      D(R.t1),
                      D(R.t2),
                      D(R.s0),
                      D(R.s1),
                      D(R.a0),
                      D(R.a1),
                      D(R.a2),
                      D(R.a3),
                      D(R.a4),
                      D(R.a5),
                      D(R.a6),
                      D(R.a7),
                      D(R.s2),
                      D(R.s3),
                      D(R.s4),
                      D(R.s5),
                      D(R.s6),
                      D(R.s7),
                      D(R.s8),
                      D(R.s9),
                      D(R.s10),
                      D(R.s11),
                      D(R.t3),
                      D(R.t4),
                      D(R.t5),
                      D(R.t6));
  } else if (policy == kDetailed) {
    // full name, reg number, 0x00 0x00 0x00 0x00, group by classificaiton
    str = fmt::format(R"(
zero register  (x0): {}
return address (x1): {}
stack pointer  (x2): {}
global pointer (x3): {}
thread pointer (x4): {}
temporary registers:
          t0   (x5): {}
          t1   (x6): {}
          t2   (x7): {}
          t3  (x28): {}
          t4  (x29): {}
          t5  (x30): {}
          t6  (x31): {}
argument registers:
          a0  (x10): {}
          a1  (x11): {}
          a2  (x12): {}
          a3  (x13): {}
          a4  (x14): {}
          a5  (x15): {}
          a6  (x16): {}
          a7  (x17): {}
saved registers:
          s0   (x8): {}
          s1   (x9): {}
          s2  (x18): {}
          s3  (x19): {}
          s4  (x20): {}
          s5  (x21): {}
          s6  (x22): {}
          s7  (x23): {}
          s8  (x24): {}
          s9  (x25): {}
          s10 (x26): {}
          s11 (x27): {}
)"_raw,
                      D(R.zero_reg),
                      D(R.ra),
                      D(R.sp),
                      D(R.gp),
                      D(R.tp),
                      D(R.t0),
                      D(R.t1),
                      D(R.t2),
                      D(R.t3),
                      D(R.t4),
                      D(R.t5),
                      D(R.t6),
                      D(R.a0),
                      D(R.a1),
                      D(R.a2),
                      D(R.a3),
                      D(R.a4),
                      D(R.a5),
                      D(R.a6),
                      D(R.a7),
                      D(R.s0),
                      D(R.s1),
                      D(R.s2),
                      D(R.s3),
                      D(R.s4),
                      D(R.s5),
                      D(R.s6),
                      D(R.s7),
                      D(R.s8),
                      D(R.s9),
                      D(R.s10),
                      D(R.s11));
  } else if (policy == kBrief) {
    using auxilia::ranges::views::swap_endian;
    // 0x00000000 0x00000000
    str.reserve(352);
    str += fmt::format("{}", R.zero_reg.to_string(kBrief));
    for (const auto &reg : raw | std::views::drop(1))
      str += fmt::format(" {}", reg.to_string(kBrief));
  } else {
    str = "unsupported format policy";
  }
  return str;
}
auto GeneralPurposeRegisters::_get_impl(
    const std::string_view str) const noexcept -> const register_t * {
  const auto I = [this](const size_t offset) constexpr noexcept {
    return raw.data() + offset;
  };
  if (str == "zero") {
    return I(0);
  } else if (str == "ra") {
    return I(1);
  } else if (str == "sp") {
    return I(2);
  } else if (str == "gp") {
    return I(3);
  } else if (str == "tp") {
    return I(4);
  } else if (str == "t0") {
    return I(5);
  } else if (str == "t1") {
    return I(6);
  } else if (str == "t2") {
    return I(7);
  } else if (str == "s0") {
    return I(8);
  } else if (str == "s1") {
    return I(9);
  } else if (str == "a0") {
    return I(10);
  } else if (str == "a1") {
    return I(11);
  } else if (str == "a2") {
    return I(12);
  } else if (str == "a3") {
    return I(13);
  } else if (str == "a4") {
    return I(14);
  } else if (str == "a5") {
    return I(15);
  } else if (str == "a6") {
    return I(16);
  } else if (str == "a7") {
    return I(17);
  } else if (str == "s2") {
    return I(18);
  } else if (str == "s3") {
    return I(19);
  } else if (str == "s4") {
    return I(20);
  } else if (str == "s5") {
    return I(21);
  } else if (str == "s6") {
    return I(22);
  } else if (str == "s7") {
    return I(23);
  } else if (str == "s8") {
    return I(24);
  } else if (str == "s9") {
    return I(25);
  } else if (str == "s10") {
    return I(26);
  } else if (str == "s11") {
    return I(27);
  } else if (str == "t3") {
    return I(28);
  } else if (str == "t4") {
    return I(29);
  } else if (str == "t5") {
    return I(30);
  } else if (str == "t6") {
    return I(31);
  }
  return nullptr;
}
} // namespace accat::luce::isa::riscv32
