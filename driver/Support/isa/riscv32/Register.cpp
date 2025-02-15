#include "./Register.hpp"
#include <span>
#include "accat/auxilia/details/views.hpp"
namespace accat::luce::isa::riscv32 {
auto GeneralPurposeRegisters::to_string(
    const auxilia::FormatPolicy policy) const -> string_type {
  auto str = string_type{};
  constexpr auto J = [](auto &&reg) AC_STATIC_CALL_OPERATOR constexpr {
    return fmt::join(reg, " ");
  };
  using auxilia::operator""_raw;
  const auto &R = registers;
  if (policy == kDefault) {
    // reg number, short name, 0x00000000
    str = fmt::format(R"(
x0 (zero): {:#04x}
x1 (ra)  : {:#04x}
x2 (sp)  : {:#04x}
x3 (gp)  : {:#04x}
x4 (tp)  : {:#04x}
x5 (t0)  : {:#04x}
x6 (t1)  : {:#04x}
x7 (t2)  : {:#04x}
x8 (s0)  : {:#04x}
x9 (s1)  : {:#04x}
x10 (a0) : {:#04x}
x11 (a1) : {:#04x}
x12 (a2) : {:#04x}
x13 (a3) : {:#04x}
x14 (a4) : {:#04x}
x15 (a5) : {:#04x}
x16 (a6) : {:#04x}
x17 (a7) : {:#04x}
x18 (s2) : {:#04x}
x19 (s3) : {:#04x}
x20 (s4) : {:#04x}
x21 (s5) : {:#04x}
x22 (s6) : {:#04x}
x23 (s7) : {:#04x}
x24 (s8) : {:#04x}
x25 (s9) : {:#04x}
x26 (s10): {:#04x}
x27 (s11): {:#04x}
x28 (t3) : {:#04x}
x29 (t4) : {:#04x}
x30 (t5) : {:#04x}
x31 (t6) : {:#04x}
)"_raw, // remove leading and trailing newline
                      J(R.zero_reg),
                      J(R.ra),
                      J(R.sp),
                      J(R.gp),
                      J(R.tp),
                      J(R.t0),
                      J(R.t1),
                      J(R.t2),
                      J(R.s0),
                      J(R.s1),
                      J(R.a0),
                      J(R.a1),
                      J(R.a2),
                      J(R.a3),
                      J(R.a4),
                      J(R.a5),
                      J(R.a6),
                      J(R.a7),
                      J(R.s2),
                      J(R.s3),
                      J(R.s4),
                      J(R.s5),
                      J(R.s6),
                      J(R.s7),
                      J(R.s8),
                      J(R.s9),
                      J(R.s10),
                      J(R.s11),
                      J(R.t3),
                      J(R.t4),
                      J(R.t5),
                      J(R.t6));
  } else if (policy == kDetailed) {
    // full name, reg number, 0x00 0x00 0x00 0x00, group by classificaiton
    str = fmt::format(R"(
zero register  (x0): {:#04x}
return address (x1): {:#04x}
stack pointer  (x2): {:#04x}
global pointer (x3): {:#04x}
thread pointer (x4): {:#04x}
temporary registers:
          t0   (x5): {:#04x}
          t1   (x6): {:#04x}
          t2   (x7): {:#04x}
          t3  (x28): {:#04x}
          t4  (x29): {:#04x}
          t5  (x30): {:#04x}
          t6  (x31): {:#04x}
argument registers:
          a0  (x10): {:#04x}
          a1  (x11): {:#04x}
          a2  (x12): {:#04x}
          a3  (x13): {:#04x}
          a4  (x14): {:#04x}
          a5  (x15): {:#04x}
          a6  (x16): {:#04x}
          a7  (x17): {:#04x}
saved registers:
          s0   (x8): {:#04x}
          s1   (x9): {:#04x}
          s2  (x18): {:#04x}
          s3  (x19): {:#04x}
          s4  (x20): {:#04x}
          s5  (x21): {:#04x}
          s6  (x22): {:#04x}
          s7  (x23): {:#04x}
          s8  (x24): {:#04x}
          s9  (x25): {:#04x}
          s10 (x26): {:#04x}
          s11 (x27): {:#04x}
)"_raw,
                      J(R.zero_reg),
                      J(R.ra),
                      J(R.sp),
                      J(R.gp),
                      J(R.tp),
                      J(R.t0),
                      J(R.t1),
                      J(R.t2),
                      J(R.t3),
                      J(R.t4),
                      J(R.t5),
                      J(R.t6),
                      J(R.a0),
                      J(R.a1),
                      J(R.a2),
                      J(R.a3),
                      J(R.a4),
                      J(R.a5),
                      J(R.a6),
                      J(R.a7),
                      J(R.s0),
                      J(R.s1),
                      J(R.s2),
                      J(R.s3),
                      J(R.s4),
                      J(R.s5),
                      J(R.s6),
                      J(R.s7),
                      J(R.s8),
                      J(R.s9),
                      J(R.s10),
                      J(R.s11));
  } else if (policy == kBrief) {
    using auxilia::ranges::views::swap_endian;
    // 0x00000000 0x00000000
    str.reserve(351);
    str +=
        fmt::format("0x{:02x}", fmt::join(R.zero_reg | swap_endian, ""));
    for (const auto &reg : raw | std::views::drop(1))
      str += fmt::format(" 0x{:02x}", fmt::join(reg | swap_endian, ""));
  } else {
    str = "unsupported format policy";
  }
  return str;
}
auto GeneralPurposeRegisters::_get_impl(
    const std::string_view str) const noexcept -> const register_t * {
  if (str == "zero") {
    return raw.data() + 0;
  } else if (str == "ra") {
    return raw.data() + 1;
  } else if (str == "sp") {
    return raw.data() + 2;
  } else if (str == "gp") {
    return raw.data() + 3;
  } else if (str == "tp") {
    return raw.data() + 4;
  } else if (str == "t0") {
    return raw.data() + 5;
  } else if (str == "t1") {
    return raw.data() + 6;
  } else if (str == "t2") {
    return raw.data() + 7;
  } else if (str == "s0") {
    return raw.data() + 8;
  } else if (str == "s1") {
    return raw.data() + 9;
  } else if (str == "a0") {
    return raw.data() + 10;
  } else if (str == "a1") {
    return raw.data() + 11;
  } else if (str == "a2") {
    return raw.data() + 12;
  } else if (str == "a3") {
    return raw.data() + 13;
  } else if (str == "a4") {
    return raw.data() + 14;
  } else if (str == "a5") {
    return raw.data() + 15;
  } else if (str == "a6") {
    return raw.data() + 16;
  } else if (str == "a7") {
    return raw.data() + 17;
  } else if (str == "s2") {
    return raw.data() + 18;
  } else if (str == "s3") {
    return raw.data() + 19;
  } else if (str == "s4") {
    return raw.data() + 20;
  } else if (str == "s5") {
    return raw.data() + 21;
  } else if (str == "s6") {
    return raw.data() + 22;
  } else if (str == "s7") {
    return raw.data() + 23;
  } else if (str == "s8") {
    return raw.data() + 24;
  } else if (str == "s9") {
    return raw.data() + 25;
  } else if (str == "s10") {
    return raw.data() + 26;
  } else if (str == "s11") {
    return raw.data() + 27;
  } else if (str == "t3") {
    return raw.data() + 28;
  } else if (str == "t4") {
    return raw.data() + 29;
  } else if (str == "t5") {
    return raw.data() + 30;
  } else if (str == "t6") {
    return raw.data() + 31;
  }
  return nullptr;
}
} // namespace accat::luce::isa::riscv32
