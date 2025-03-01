#include <accat/auxilia/auxilia.hpp>
#include <cstdint>

#include "luce/Support/isa/Icpu.hpp"

#include "luce/Support/isa/riscv32/instruction/Multiply.hpp"
#include <fmt/format.h>

namespace accat::luce::isa::riscv32::instruction::multiply {
using auxilia::as;
using auxilia::FormatPolicy;
#pragma region Multiply
auto Mul::asmStr() const noexcept -> string_type {
  return fmt::format("mul x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Mul::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed, signed -> signed
  auto &gpr = cpu->gpr();
  auto num = as<int64_t>(gpr[rs1()]) * as<int64_t>(gpr[rs2()]);
  // [31:0]
  gpr.write_at(rd()) = as<signed_num_type>(extractBits<0, 32>(num));
  return kSuccess;
}
auto Mulh::asmStr() const noexcept -> string_type {
  return fmt::format("mulh x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Mulh::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed, signed -> signed
  auto &gpr = cpu->gpr();
  auto num = as<int64_t>(gpr[rs1()]) * as<int64_t>(gpr[rs2()]);
  // [63:32]
  gpr.write_at(rd()) = as<signed_num_type>(extractBits<32, 64>(num));
  return kSuccess;
}
auto Mulsu::asmStr() const noexcept -> string_type {
  return fmt::format("mulsu x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Mulsu::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed, unsigned -> signed
  auto &gpr = cpu->gpr();
  auto num = as<int64_t>(gpr[rs1()]) * as<uint64_t>(gpr[rs2()]);
  // [63:32]
  gpr.write_at(rd()) = as<signed_num_type>(extractBits<32, 64>(num));
  return kSuccess;
}
auto Mulu::asmStr() const noexcept -> string_type {
  return fmt::format("mulu x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Mulu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned, unsigned -> unsigned
  auto &gpr = cpu->gpr();
  auto num = as<uint64_t>(gpr[rs1()]) * as<uint64_t>(gpr[rs2()]);
  // [63:32]
  gpr.write_at(rd()) = as<num_type>(extractBits<32, 64>(num));
  return kSuccess;
}
auto Div::asmStr() const noexcept -> string_type {
  return fmt::format("div x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Div::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed, signed -> signed
  auto &gpr = cpu->gpr();
  auto num = as<signed_num_type>(gpr[rs1()]) / as<signed_num_type>(gpr[rs2()]);
  gpr.write_at(rd()) = as<signed_num_type>(num);
  return kSuccess;
}
auto Divu::asmStr() const noexcept -> string_type {
  return fmt::format("divu x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Divu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned, unsigned -> unsigned
  auto &gpr = cpu->gpr();
  auto num = as<num_type>(gpr[rs1()]) / as<num_type>(gpr[rs2()]);
  gpr.write_at(rd()) = as<num_type>(num);
  return kSuccess;
}
auto Rem::asmStr() const noexcept -> string_type {
  return fmt::format("rem x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Rem::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed, signed -> signed
  auto &gpr = cpu->gpr();
  auto num = as<signed_num_type>(gpr[rs1()]) % as<signed_num_type>(gpr[rs2()]);
  gpr.write_at(rd()) = as<signed_num_type>(num);
  return kSuccess;
}
auto Remu::asmStr() const noexcept -> string_type {
  return fmt::format("remu x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Remu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned, unsigned -> unsigned
  auto &gpr = cpu->gpr();
  auto num = as<num_type>(gpr[rs1()]) % as<num_type>(gpr[rs2()]);
  gpr.write_at(rd()) = as<num_type>(num);
  return kSuccess;
}
#pragma endregion Multiply
#pragma region DecodeImpl
class DecodeImpl : public Word,
                   mixin::opcode0To6Common,
                   mixin::funct3rd12To15Common,
                   mixin::funct7th25To32Common {
  using inst_t = IInstruction;
  using inst_ptr_t = std::unique_ptr<inst_t>;
  friend class Decoder;

protected:
  using Word::Word;
  inst_ptr_t decode() const;
};
auto DecodeImpl::decode() const -> inst_ptr_t {
  if (opcode() != 0b0110011)
    return nullptr;
  if (funct7() != 0b01)
    return nullptr;

  switch (funct3()) {
  case 0x0:
    return std::make_unique<Mul>(num());
  case 0x1:
    return std::make_unique<Mulh>(num());
  case 0x2:
    return std::make_unique<Mulsu>(num());
  case 0x3:
    return std::make_unique<Mulu>(num());
  case 0x4:
    return std::make_unique<Div>(num());
  case 0x5:
    return std::make_unique<Divu>(num());
  case 0x6:
    return std::make_unique<Rem>(num());
  case 0x7:
    return std::make_unique<Remu>(num());
  default:
    return nullptr;
  }
}
#pragma endregion DecodeImpl
#pragma region Decoder
auto Decoder::decode(uint32_t inst) -> std::unique_ptr<IInstruction> {
  DecodeImpl decoder(inst);
  return decoder.decode();
}
#pragma endregion Decoder
} // namespace accat::luce::isa::riscv32::instruction::multiply
