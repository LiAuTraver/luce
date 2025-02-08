#include "./Register.hpp"
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
x0 (zero): {:#x}
x1 (ra)  : {:#x}
x2 (sp)  : {:#x}
x3 (gp)  : {:#x}
x4 (tp)  : {:#x}
x5 (t0)  : {:#x}
x6 (t1)  : {:#x}
x7 (t2)  : {:#x}
x8 (s0)  : {:#x}
x9 (s1)  : {:#x}
x10 (a0) : {:#x}
x11 (a1) : {:#x}
x12 (a2) : {:#x}
x13 (a3) : {:#x}
x14 (a4) : {:#x}
x15 (a5) : {:#x}
x16 (a6) : {:#x}
x17 (a7) : {:#x}
x18 (s2) : {:#x}
x19 (s3) : {:#x}
x20 (s4) : {:#x}
x21 (s5) : {:#x}
x22 (s6) : {:#x}
x23 (s7) : {:#x}
x24 (s8) : {:#x}
x25 (s9) : {:#x}
x26 (s10): {:#x}
x27 (s11): {:#x}
x28 (t3) : {:#x}
x29 (t4) : {:#x}
x30 (t5) : {:#x}
x31 (t6) : {:#x}
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
zero register  (x0): {:#x}
return address (x1): {:#x}
stack pointer  (x2): {:#x}
global pointer (x3): {:#x}
thread pointer (x4): {:#x}
temporary registers:
          t0   (x5): {:#x}
          t1   (x6): {:#x}
          t2   (x7): {:#x}
          t3  (x28): {:#x}
          t4  (x29): {:#x}
          t5  (x30): {:#x}
          t6  (x31): {:#x}
argument registers:
          a0  (x10): {:#x}
          a1  (x11): {:#x}
          a2  (x12): {:#x}
          a3  (x13): {:#x}
          a4  (x14): {:#x}
          a5  (x15): {:#x}
          a6  (x16): {:#x}
          a7  (x17): {:#x}
saved registers:
          s0   (x8): {:#x}
          s1   (x9): {:#x}
          s2  (x18): {:#x}
          s3  (x19): {:#x}
          s4  (x20): {:#x}
          s5  (x21): {:#x}
          s6  (x22): {:#x}
          s7  (x23): {:#x}
          s8  (x24): {:#x}
          s9  (x25): {:#x}
          s10 (x26): {:#x}
          s11 (x27): {:#x}
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
    // 0x00000000 0x00000000
    str.reserve(351);
    str += fmt::format("0x{:02x}", fmt::join(R.zero_reg, ""));
    for (const auto &reg : general_purpose_registers.subspan(1))
      str += fmt::format(" 0x{:02x}", fmt::join(reg, ""));
  } else {
    str = "unsupported format policy";
  }
  return str;
}
auto GeneralPurposeRegisters::_get_impl(
    const std::string_view str) const noexcept -> const register_t * {
  if (str == "zero") {
    return raw + 0;
  } else if (str == "ra") {
    return raw + 1;
  } else if (str == "sp") {
    return raw + 2;
  } else if (str == "gp") {
    return raw + 3;
  } else if (str == "tp") {
    return raw + 4;
  } else if (str == "t0") {
    return raw + 5;
  } else if (str == "t1") {
    return raw + 6;
  } else if (str == "t2") {
    return raw + 7;
  } else if (str == "s0") {
    return raw + 8;
  } else if (str == "s1") {
    return raw + 9;
  } else if (str == "a0") {
    return raw + 10;
  } else if (str == "a1") {
    return raw + 11;
  } else if (str == "a2") {
    return raw + 12;
  } else if (str == "a3") {
    return raw + 13;
  } else if (str == "a4") {
    return raw + 14;
  } else if (str == "a5") {
    return raw + 15;
  } else if (str == "a6") {
    return raw + 16;
  } else if (str == "a7") {
    return raw + 17;
  } else if (str == "s2") {
    return raw + 18;
  } else if (str == "s3") {
    return raw + 19;
  } else if (str == "s4") {
    return raw + 20;
  } else if (str == "s5") {
    return raw + 21;
  } else if (str == "s6") {
    return raw + 22;
  } else if (str == "s7") {
    return raw + 23;
  } else if (str == "s8") {
    return raw + 24;
  } else if (str == "s9") {
    return raw + 25;
  } else if (str == "s10") {
    return raw + 26;
  } else if (str == "s11") {
    return raw + 27;
  } else if (str == "t3") {
    return raw + 28;
  } else if (str == "t4") {
    return raw + 29;
  } else if (str == "t5") {
    return raw + 30;
  } else if (str == "t6") {
    return raw + 31;
  }
  return nullptr;
}
} // namespace accat::luce::isa::riscv32
