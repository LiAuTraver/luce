#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <array>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>
#include <accat/auxilia/auxilia.hpp>

#include "./isa.hpp"
#include "Support/isa/config.hpp"

namespace accat::luce::isa::riscv32 {

struct LUCE_SUPPORT_ISA_API GeneralPurposeRegisters : auxilia::Printable<GeneralPurposeRegisters> {
private:
  using enum auxilia::FormatPolicy;
  using self_type = GeneralPurposeRegisters;

public:
  using register_t = std::array<std::byte, instruction_size_bytes>;
  using iSpan = auxilia::incontiguous_span<register_t>;

  constexpr GeneralPurposeRegisters() {}
  GeneralPurposeRegisters(const GeneralPurposeRegisters &) = delete;
  GeneralPurposeRegisters &operator=(const GeneralPurposeRegisters &) = delete;
  GeneralPurposeRegisters(GeneralPurposeRegisters &&that) noexcept {
    memmove(raw, that.raw, sizeof(raw));
  }
  GeneralPurposeRegisters &operator=(GeneralPurposeRegisters &&that) noexcept {
    memmove(raw, that.raw, sizeof(raw));
    return *this;
  }

  constexpr register_t /* not a reference */ zero() const noexcept {
    dbg_block
    {
      contract_assert(register_t{} == registers.zero_reg,
                      "zero register is not zero");
    };
    return registers.zero_reg;
  }
  constexpr register_t &ra(this auto &&self) noexcept {
    return self.registers.ra;
  }
  constexpr register_t &sp(this auto &&self) noexcept {
    return self.registers.sp;
  }
  constexpr register_t &gp(this auto &&self) noexcept {
    return self.registers.gp;
  }
  constexpr register_t &tp(this auto &&self) noexcept {
    return self.registers.tp;
  }
  auto arguments(this auto &&self) noexcept {
    return iSpan{{&self.registers.a0, 8}};
  }
  auto saved(this auto &&self) noexcept {
    return iSpan{{&self.registers.s0, 2}, {&self.registers.s2, 8}};
  }
  auto temporary(this auto &&self) noexcept {
    return iSpan{{&self.registers.t0, 3}, {&self.registers.t3, 4}};
  }
  auto &
  reset(const register_t (&newVal)[general_purpose_register_count]) noexcept {
    memcpy(raw, newVal, sizeof(raw));
    return *this;
  }
  [[clang::reinitializes]] auto &reset() noexcept {
    memset(raw, 0, sizeof(raw));
    return *this;
  }
  auto read(const std::string_view str) const noexcept
      -> std::optional<register_t> {
    if (auto reg = get_register_by_string(str)) {
      return *reg;
    }
    return std::nullopt;
  }
  /// @return previous value, if the register is not writable, return nullopt
  auto write(const std::string_view str, const register_t val)
      -> std::optional<register_t> {
    // if the register is not writable, return nullopt
    if (auto reg = get_register_by_string(str); reg) {
      if (reg == &registers.zero_reg) {
        spdlog::warn("attempted to write to zero register");
        return std::nullopt;
      }
      auto prev = *reg;
      *reg = val;
      return std::make_optional(prev);
    }
    return std::nullopt;
  }
  auto to_string(auxilia::FormatPolicy policy = kDefault) const -> string_type;

private:
  auto get_register_by_string(this auto &&self,
                              const std::string_view str) noexcept
      -> register_t * {
    return const_cast<register_t *>(
        const_cast<const decltype(self) &>(self)._get_impl(str));
  }
  auto _get_impl(std::string_view str) const noexcept -> const register_t *;

private:
#pragma pack(push, 1)
  // clang-format off
  union alignas(4)   {
    using R = register_t;
    alignas(4) R raw[general_purpose_register_count] = {};
    alignas(4) struct {
      const R zero_reg;
      R ra; R sp; R gp; R tp;
      R t0; R t1; R t2;
      R s0; R s1;
      R a0; R a1; R a2; R a3; R a4; R a5; R a6; R a7;
      R s2; R s3; R s4; R s5; R s6; R s7; R s8; R s9; R s10; R s11;
      R t3; R t4; R t5; R t6;
    } registers;
  };
  // clang-format on
#pragma pack(pop)
public:
  // shall NOT use this: avoid setting zero register to non-zero value
  std::span<register_t, general_purpose_register_count>
      general_purpose_registers = std::span{raw};
};

} // namespace accat::luce::isa::riscv32
