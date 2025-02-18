#pragma once
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include "./constants/riscv32.hpp"
#include "accat/auxilia/defines.hpp"
#include "accat/auxilia/details/format.hpp"
namespace accat::luce::isa {
/// @brief A class representing a word.
/// @remark A few things to be clarified:
/// - Word size is usually the size a general purpose register can hold.
/// - Word size is usually the size of the largest addressable unit.
/// - Word size has nothing to do with the size of the instruction,
///       but as for riscv32, it is equal to the size of the instruction;
///           as for riscv64, it is 64 bits whilst the instruction size is 32
///           bits.
///       exception: rv32C Compressed Instruction Set has 16-bit instructions;
///                  riscv128 has 128-bit instructions.
/// @note in the current implementation, we assume word size is larger than
///       the size of the instruction. (which is not true, especially for x86)
struct AC_EMPTY_BASES AC_NOVTABLE Word : auxilia::Printable {
protected:
  using num_type = instruction_size_t;
  using bytes_type = std::array<std::byte, instruction_size_bytes>;
  using bits_type = std::bitset<instruction_size_bytes * 8>;
  using bytes_view_type = std::span<const std::byte, instruction_size_bytes>;
  static_assert(std::is_trivially_destructible_v<num_type>,
                "num_ must be trivially destructible");
  static_assert(std::is_trivially_destructible_v<bytes_type>,
                "bytes_ must be trivially destructible");
  static_assert(std::is_trivially_destructible_v<bits_type>,
                "bits_ must be trivially destructible");

private:
  union {
    num_type num_ = 0ull;
    bytes_type bytes_;
    bits_type bits_;
  };
  using enum auxilia::FormatPolicy;

public:
  auto &bits(this auto &&self) noexcept {
    return self.bits_;
  }
  auto &num(this auto &&self) noexcept {
    return self.num_;
  }
  auto &bytes(this auto &&self) noexcept {
    return self.bytes_;
  }
  auto to_string(auxilia::FormatPolicy policy = kBrief) const noexcept {
    if (policy == kBrief) // 0x00000000 (big-endian)
      return fmt::format("0x{:08x}", num());
    else if (policy == kDetailed) // 0x00 0x00 0x00 0x00 
      return fmt::format("{:#04x}", fmt::join(bytes(), " "));
    else
      return fmt::format("{}", bits());
  }
  auto data(this auto &&self) noexcept {
    return self.bytes_.data();
  }
  [[clang::reinitializes]] auto reset(const num_type num = 0ull) noexcept {
    num_ = num;
    return *this;
  }

public:
  constexpr Word() noexcept = default;
  constexpr explicit Word(num_type num) noexcept : num_(num) {}
  constexpr explicit Word(
      std::array<std::byte, instruction_size_bytes> bytes) noexcept
      : bytes_(bytes) {}
  constexpr explicit Word(std::bitset<instruction_size_bytes * 8> bits) noexcept
      : bits_(bits) {}
  Word(const bytes_view_type bytes) noexcept {
    std::ranges::copy(bytes, bytes_.begin());
  }
  Word(const Word &) = default;
  Word &operator=(const Word &) = default;
  Word(Word &&) = default;
  Word &operator=(Word &&) = default;
  ~Word() = default;

  template <class... Args>
  constexpr Word(Args... args)
    requires((sizeof...(Args) == instruction_size_bytes) && ... &&
             (std::is_same_v<Args, std::byte> ||
              std::is_same_v<Args, minimal_addressable_unit_t>))
      : bytes_{args...} {}

public:
  // helper to extract bits [Beg, End)
  template <size_t Beg, size_t End, typename T = num_type>
  inline static constexpr T extractBits(const T value) noexcept {
    static_assert(Beg <= End, "Beg must be less than or equal to End");
    static_assert(End <= sizeof(T) * 8,
                  "End must be within the bit width of T");

    constexpr T mask = (static_cast<T>(1) << (End - Beg)) - 1;
    return (value >> Beg) & mask;
  }

  // helper to concatenate bits from multiple [Begin, End) ranges
  template <typename T, size_t Begin, size_t End, size_t... Rest>
  inline static constexpr T concatBits(const T value) noexcept {
    static_assert(sizeof...(Rest) % 2 == 0, "Rest must be a multiple of 2");
    if constexpr (sizeof...(Rest) == 0) {
      return extractBits<Begin, End>(value);
    } else {
      auto chunk = extractBits<Begin, End, T>(value);
      constexpr auto bitCount = End - Begin;
      return chunk | (concatBits<T, Rest...>(value) << bitCount);
    }
  }
};
} // namespace accat::luce::isa
