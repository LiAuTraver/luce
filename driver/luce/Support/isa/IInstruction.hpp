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
#include "accat/auxilia/details/format.hpp"

namespace accat::luce::isa {
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
  auto to_string(auxilia::FormatPolicy policy = auxilia::FormatPolicy::kDefault)
      const noexcept -> string_type {
    dbg(trace, "Bits: 0b{}", bits());
    if (policy == kDefault) {
      return asmStr();
    } else {
      // TODO(...)
      return asmStr();
    }
  }

public:
  enum class ExecutionStatus : std::uint8_t {
    // normal instruction execution, advancing PC by default
    kOk = 0,
    // not advancing PC, e.g., branch instructions
    kOkButDontBotherPC,

    // TODO: advance pc?
    kMemoryViolation,
    kInvalidInstruction,
    // save PC into mepc(not implemented now)
    kEnvCall,
    kEnvBreak,
    // bang!
    kUnknown
  };

public:
  virtual ExecutionStatus execute(Icpu *) const = 0;

protected:
  virtual auto asmStr() const noexcept -> string_type = 0;

protected:
  using enum ExecutionStatus;
};

} // namespace accat::luce::isa
