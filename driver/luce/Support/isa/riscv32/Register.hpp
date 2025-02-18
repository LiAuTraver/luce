#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <mdspan>
#include <optional>
#include <span>
#include <string_view>
#include <utility>
#include <accat/auxilia/auxilia.hpp>

#include "../Word.hpp"

namespace accat::luce::isa::riscv32 {
struct GeneralPurposeRegisters
    : auxilia::Printable {
private:
  using enum auxilia::FormatPolicy;
  using self_type = GeneralPurposeRegisters;

public:
  // union register_t {
  //   constexpr register_t() noexcept : num{0ull} {}
  //   uint32_t num{};
  //   std::array<std::byte, 4> bytes;
  //   std::bitset<32> bits;
  // };
  using register_t = Word;
  using registers_t = std::array<register_t, general_purpose_register_count>;
  using iSpan = auxilia::incontiguous_span<register_t>;

  constexpr GeneralPurposeRegisters() {}
  GeneralPurposeRegisters(const GeneralPurposeRegisters &) = delete;
  GeneralPurposeRegisters &operator=(const GeneralPurposeRegisters &) = delete;
  GeneralPurposeRegisters(self_type &&that) noexcept {
    raw = std::move(that.raw);
  }
  auto &operator=(self_type &&that) noexcept {
    raw = std::move(that.raw);
    return *this;
  }

  constexpr register_t /* not a reference */ zero() const noexcept {
    dbg_block
    {
      // contract_assert(register_t{}.num_ == registers.zero_reg,
      //                 "zero register is not zero");
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
  [[clang::reinitializes]] auto &
  reset(const registers_t &newVal = {}) noexcept {
    raw = newVal;
    return *this;
  }
  auto read(const std::string_view str) const noexcept
      -> std::optional<register_t> {
    if (auto reg = get_register_by_string(str)) {
      return *reg;
    }
    return std::nullopt;
  }
  auto &operator[](this auto &&self, const std::string_view str) noexcept
      [[clang::lifetimebound]] {
    auto res = self.get_register_by_string(str);
    precondition(res, "register not found");
    return *res;
  }
  auto &operator[](this auto &&self, const size_t idx) noexcept
      [[clang::lifetimebound]] {
    precondition(idx < general_purpose_register_count, "index out of bounds");
    return self.raw[idx].num();
  }
  auto &at(const size_t idx) const noexcept [[clang::lifetimebound]] {
    precondition(idx < general_purpose_register_count, "index out of bounds");
    return raw[idx];
  }
  auto &at(const std::string_view str) const noexcept [[clang::lifetimebound]] {
    auto reg = get_register_by_string(str);
    contract_assert(reg, "register not found");
    return *reg;
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
      [[clang::lifetimebound]] -> register_t * {
    return const_cast<register_t *>(
        const_cast<const decltype(self) &>(self)._get_impl(str));
  }
  auto _get_impl(std::string_view str) const noexcept -> const register_t *;

private:
#pragma pack(push, 1)
  // clang-format off
  union alignas(4)   {
    using R = register_t;
    alignas(4) registers_t raw = {};
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
  auto bytes_view() const noexcept [[clang::lifetimebound]] {
    return std::mdspan<const std::byte, std::dextents<std::size_t, 2>>{
        reinterpret_cast<const std::byte *>(raw.data()),
        general_purpose_register_count,
        register_t{}.bytes().size()};
  }
  auto bytes_view() noexcept [[clang::lifetimebound]] {
    return std::mdspan<std::byte, std::dextents<std::size_t, 2>>{
        reinterpret_cast<std::byte *>(raw.data()),
        general_purpose_register_count,
        register_t{}.bytes().size()};
  }
  auto view() const noexcept [[clang::lifetimebound]] {
    return std::span<const register_t, general_purpose_register_count>{raw};
  }
  auto view() noexcept [[clang::lifetimebound]] {
    return std::span<register_t, general_purpose_register_count>{raw};
  }
};

} // namespace accat::luce::isa::riscv32
