#include <accat/auxilia/auxilia.hpp>

#include "accat/auxilia/details/format.hpp"
#include "luce/Task.hpp"
#include "luce/cpu/cpu.hpp"

#include "luce/Support/isa/riscv32/Instruction.hpp"
#include "luce/Support/isa/riscv32/Decoder.hpp"

namespace accat::luce::isa::riscv32::instruction::base {
using auxilia::as;
using auxilia::FormatPolicy;
#pragma region Cal
auto Add::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] + gpr[rs2()];
  return kSuccess;
}
auto Add::asmStr() const noexcept -> string_type {
  return fmt::format("add x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Sub::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] - gpr[rs2()];
  return kSuccess;
}
auto Sub::asmStr() const noexcept -> string_type {
  return fmt::format("sub x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Xor::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] ^ gpr[rs2()];
  return kSuccess;
}
auto Xor::asmStr() const noexcept -> string_type {
  return fmt::format("xor x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Or::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] | gpr[rs2()];
  return kSuccess;
}
auto Or::asmStr() const noexcept -> string_type {
  return fmt::format("or x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto And::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] & gpr[rs2()];
  return kSuccess;
}
auto And::asmStr() const noexcept -> string_type {
  return fmt::format("and x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Sll::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] << (gpr[rs2()] & 0x1F); // mask the shift
  return kSuccess;
}
auto Sll::asmStr() const noexcept -> string_type {
  return fmt::format("sll x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Srl::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // logical right shift
  gpr.write_at(rd()) = gpr[rs1()] >> (gpr[rs2()] & 0x1F); // ditto
  return kSuccess;
}
auto Srl::asmStr() const noexcept -> string_type {
  return fmt::format("srl x{}, x{}, x{}", rd(), rs1(), rs2());
}

auto Sra::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // arithmetic right shift (MSB-extended)
  gpr.write_at(rd()) = as<signed_num_type>(gpr[rs1()]) >> (gpr[rs2()] & 0x1F);
  return kSuccess;
}
auto Sra::asmStr() const noexcept -> string_type {
  return fmt::format("sra x{}, x{}, x{}", rd(), rs1(), rs2());
}

auto Slt::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) =
      as<signed_num_type>(gpr[rs1()]) < as<signed_num_type>(gpr[rs2()]);
  return kSuccess;
}
auto Slt::asmStr() const noexcept -> string_type {
  return fmt::format("slt x{}, x{}, x{}", rd(), rs1(), rs2());
}

auto Sltu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()]);
  return kSuccess;
}
auto Sltu::asmStr() const noexcept -> string_type {
  return fmt::format("sltu x{}, x{}, x{}", rd(), rs1(), rs2());
}
#pragma endregion Cal
#pragma region Imm
auto Addi::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] + imm();
  return kSuccess;
}
auto Addi::asmStr() const noexcept -> string_type {
  return fmt::format("addi x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Xori::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] ^ imm();
  return kSuccess;
}
auto Xori::asmStr() const noexcept -> string_type {
  return fmt::format("xori x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Ori::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] | imm();
  return kSuccess;
}
auto Ori::asmStr() const noexcept -> string_type {
  return fmt::format("ori x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Andi::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] & imm();
  return kSuccess;
}
auto Andi::asmStr() const noexcept -> string_type {
  return fmt::format("andi x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Slli::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 << imm[0:4]
  gpr.write_at(rd()) = gpr[rs1()] << (imm() & 0x1F);
  return kSuccess;
}
auto Slli::asmStr() const noexcept -> string_type {
  return fmt::format("slli x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Srli::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  gpr.write_at(rd()) = gpr[rs1()] >> (imm() & 0x1F);
  return kSuccess;
}
auto Srli::asmStr() const noexcept -> string_type {
  return fmt::format("srli x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Srai::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  // msb-extend
  gpr.write_at(rd()) = as<signed_num_type>(gpr[rs1()]) >> (imm() & 0x1F);
  return kSuccess;
}
auto Srai::asmStr() const noexcept -> string_type {
  return fmt::format("srai x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Slti::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = as<signed_num_type>(gpr[rs1()] < imm());
  return kSuccess;
}
auto Slti::asmStr() const noexcept -> string_type {
  return fmt::format("slti x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Sltiu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = as<num_type>(gpr[rs1()] < imm());
  return kSuccess;
}
auto Sltiu::asmStr() const noexcept -> string_type {
  return fmt::format("sltiu x{}, x{}, {}", rd(), rs1(), imm_str());
}

auto Lb::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr.write_at(rd()) = as<signed_num_type>(bytes & 0xFF);
  return kSuccess;
}
auto Lb::asmStr() const noexcept -> string_type {
  return fmt::format("lb x{}, {}(x{})", rd(), imm_str(), rs1());
}

auto Lh::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr.write_at(rd()) = as<signed_num_type>(bytes & 0xFFFF);
  return kSuccess;
}
auto Lh::asmStr() const noexcept -> string_type {
  return fmt::format("lh x{}, {}(x{})", rd(), imm_str(), rs1());
}

auto Lw::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr.write_at(rd()) = as<signed_num_type>(bytes);
  return kSuccess;
}
auto Lw::asmStr() const noexcept -> string_type {
  return fmt::format("lw x{}, {}(x{})", rd(), imm_str(), rs1());
}

auto Lbu::execute(Icpu *cpu) const -> ExecutionStatus {
  // zero-extend(unsigned)
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr.write_at(rd()) = bytes & 0xFF;
  return kSuccess;
}
auto Lbu::asmStr() const noexcept -> string_type {
  return fmt::format("lbu x{}, {}(x{})", rd(), imm_str(), rs1());
}

auto Lhu::execute(Icpu *cpu) const -> ExecutionStatus {
  // zero-extend
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr.write_at(rd()) = bytes & 0xFFFF;
  return kSuccess;
}
auto Lhu::asmStr() const noexcept -> string_type {
  return fmt::format("lhu x{}, {}(x{})", rd(), imm_str(), rs1());
}
#pragma endregion Imm
#pragma region Store
auto Sb::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // M[rs1+imm][0:7] = rs2[0:7]
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  bytes = (bytes & 0xFFFFFF00) | (gpr[rs2()] & 0xFF);
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&bytes, 1}));
  if (!status) {
    return kMemoryViolation;
  }
  return kSuccess;
}
auto Sb::asmStr() const noexcept -> string_type {
  return fmt::format("sb x{}, {}(x{})", rs2(), imm_str(), rs1());
}

auto Sh::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // M[rs1+imm][0:15] = rs2[0:15]
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  bytes = (bytes & 0xFFFF0000) | (gpr[rs2()] & 0xFFFF);
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&bytes, 2}));
  if (!status) {
    return kMemoryViolation;
  }
  return kSuccess;
}
auto Sh::asmStr() const noexcept -> string_type {
  return fmt::format("sh x{}, {}(x{})", rs2(), imm_str(), rs1());
}

auto Sw::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // M[rs1+imm][0:31] = rs2[0:31]
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&gpr[rs2()], 4}));
  if (!status) {
    return kMemoryViolation;
  }
  return kSuccess;
}
auto Sw::asmStr() const noexcept -> string_type {
  return fmt::format("sw x{}, {}(x{})", rs2(), imm_str(), rs1());
}
#pragma endregion Store
#pragma region Branch
auto Beq::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  if (gpr[rs1()] == gpr[rs2()]) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
auto Beq::asmStr() const noexcept -> string_type {
  return fmt::format("beq x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bne::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  if (gpr[rs1()] != gpr[rs2()]) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
auto Bne::asmStr() const noexcept -> string_type {
  return fmt::format("bne x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Blt::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  if (as<signed_num_type>(gpr[rs1()]) < as<signed_num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
auto Blt::asmStr() const noexcept -> string_type {
  return fmt::format("blt x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bge::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  if (as<signed_num_type>(gpr[rs1()]) >= as<signed_num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
auto Bge::asmStr() const noexcept -> string_type {
  return fmt::format("bge x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bltu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned
  auto &gpr = cpu->gpr();
  if (as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
auto Bltu::asmStr() const noexcept -> string_type {
  return fmt::format("bltu x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bgeu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned
  auto &gpr = cpu->gpr();
  if (as<num_type>(gpr[rs1()]) >= as<num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
auto Bgeu::asmStr() const noexcept -> string_type {
  return fmt::format("bgeu x{}, x{}, {}", rs1(), rs2(), imm_str());
}
#pragma endregion Branch
#pragma region Jump
auto Jal::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = cpu->pc().num() + 4;
  cpu->pc().num() += imm();
  return kSuccessAndNotAdvancePC;
}
auto Jal::asmStr() const noexcept -> string_type {
  return fmt::format("jal x{}, {}", rd(), imm_str());
}

auto Jalr::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  auto t = cpu->pc().num() + 4;
  cpu->pc().num() = (gpr[rs1()] + imm());
  gpr.write_at(rd()) = t;
  return kSuccessAndNotAdvancePC;
}
auto Jalr::asmStr() const noexcept -> string_type {
  return fmt::format("jalr x{}, x{}, {}", rd(), rs1(), imm_str());
}
#pragma endregion Jump
#pragma region UpperImm
auto Lui::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = imm() << 12;
  return kSuccess;
}
auto Lui::asmStr() const noexcept -> string_type {
  return fmt::format("lui x{}, {}", rd(), imm_str());
}
auto Auipc::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = cpu->pc().num() + (imm() << 12);
  return kSuccess;
}
auto Auipc::asmStr() const noexcept -> string_type {
  return fmt::format("auipc x{}, {}", rd(), imm_str());
}
#pragma endregion UpperImm
#pragma region System
auto Ecall::execute(Icpu *cpu) const -> ExecutionStatus {
  return kEnvCall;
}
auto Ecall::asmStr() const noexcept -> string_type {
  return "ecall";
}
auto Ebreak::execute(Icpu *cpu) const -> ExecutionStatus {
  return kEnvBreak;
}
auto Ebreak::asmStr() const noexcept -> string_type {
  return "ebreak";
}
#pragma endregion System
} // namespace accat::luce::isa::riscv32::instruction::base
