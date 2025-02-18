#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <type_traits>
#include "Word.hpp"

namespace accat::luce {
class Icpu;
}
namespace accat::luce::isa {
struct IInstruction : Word {
public:
  IInstruction() noexcept = default;
  explicit IInstruction(num_type num) noexcept : Word(num) {}
  explicit IInstruction(bytes_type bytes) noexcept : Word(bytes) {}
  explicit IInstruction(bits_type bits) noexcept : Word(bits) {}
  IInstruction(const std::span<const std::byte, 4> bytes) noexcept
      : Word(bytes) {}
  IInstruction(const IInstruction &) = delete;
  IInstruction &operator=(const IInstruction &) = delete;
  IInstruction(IInstruction &&) = default;
  IInstruction &operator=(IInstruction &&) = default;
  virtual ~IInstruction() = default;

public:
  virtual void execute(Icpu *) const = 0;
};

} // namespace accat::luce::isa::riscv32
