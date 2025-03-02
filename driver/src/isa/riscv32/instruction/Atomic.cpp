#include <accat/auxilia/auxilia.hpp>
#include <utility>

#include "accat/auxilia/details/macros.hpp"
#include "luce/Support/isa/Icpu.hpp"

#include "luce/Support/isa/riscv32/instruction/Atomic.hpp"

namespace accat::luce::isa::riscv32::instruction::atomic {
using auxilia::as;
using auxilia::FormatPolicy;
#pragma region Atomic
// TODO: implement atomic instructions
auto Lr::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  auto data = cpu->fetch(gpr[rs1()]);
  if (!data)
    return kMemoryViolation;
  gpr.write_at(rd()) =
      *reinterpret_cast<const num_type *>(std::move(data)->data());
  cpu->atomic_address() = rs1();
  return kSuccess;
}
auto Lr::asmStr() const noexcept -> string_type {
  return fmt::format("lr.w x{}, (x{})", rd(), rs1());
}
auto Sc::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  if (cpu->atomic_address() != rs1()) {
    // atomic address not match
    gpr.write_at(rd()) = 1;
    return kSuccess;
  }

  [[maybe_unused]] auto res = cpu->write(gpr[rs1()],
                                         std::as_bytes( // write a word
                                             std::span{&gpr[rs2()], 4}));
  contract_assert(res.ok(),
                  "write failed. you should check the address before it "
                  "stored into atomic_address.");
  gpr.write_at(rd()) = 0;
  return kSuccess;
}
auto Sc::asmStr() const noexcept -> string_type {
  return fmt::format("sc.w x{}, x{}, (x{})", rd(), rs2(), rs1());
}
auto Swap::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto Swap::asmStr() const noexcept -> string_type {
  return fmt::format("swap.w x{}, x{}, (x{})", rd(), rs2(), rs1());
}
auto Add::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto Add::asmStr() const noexcept -> string_type {
  return fmt::format("add.w x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto And::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto And::asmStr() const noexcept -> string_type {
  return fmt::format("and.w x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Or::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto Or::asmStr() const noexcept -> string_type {
  return fmt::format("or.w x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Xor::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto Xor::asmStr() const noexcept -> string_type {
  return fmt::format("xor.w x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Max::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto Max::asmStr() const noexcept -> string_type {
  return fmt::format("max.w x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Min::execute(Icpu *cpu) const -> ExecutionStatus {
  TODO(...)
}
auto Min::asmStr() const noexcept -> string_type {
  return fmt::format("min.w x{}, x{}, x{}", rd(), rs1(), rs2());
}
#pragma endregion Atomic
#pragma region DecodeImpl
class DecodeImpl : public Word,
                   mixin::opcode0To6Common,
                   mixin::funct3rd12To15Common,
                   mixin::funct5th27To32Common {
  using inst_t = IInstruction;
  using inst_ptr_t = std::unique_ptr<inst_t>;
  friend class Decoder;

protected:
  using Word::Word;
  inst_ptr_t decode() const;
};
auto DecodeImpl::decode() const -> inst_ptr_t {
  if (opcode() != 0b0101111)
    return nullptr;
  if (funct3() != 0x2)
    return nullptr;
  switch (funct5()) {
  case 0x02:
    return std::make_unique<Lr>(num());
  case 0x03:
    return std::make_unique<Sc>(num());
  case 0x01:
    return std::make_unique<Swap>(num());
  case 0x00:
    return std::make_unique<Add>(num());
  case 0x0C:
    return std::make_unique<And>(num());
  case 0x0A:
    return std::make_unique<Or>(num());
  case 0x04:
    return std::make_unique<Xor>(num());
  case 0x14:
    return std::make_unique<Max>(num());
  case 0x10:
    return std::make_unique<Min>(num());
  default:
    return nullptr;
  }
}
#pragma endregion DecodeImpl
#pragma region Decoder
auto Decoder::decode(uint32_t num) -> std::unique_ptr<IInstruction> {
  DecodeImpl decoder(num);
  return decoder.decode();
}
#pragma endregion Decoder
} // namespace accat::luce::isa::riscv32::instruction::atomic
