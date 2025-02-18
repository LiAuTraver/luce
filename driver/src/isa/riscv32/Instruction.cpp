
#include <accat/auxilia/auxilia.hpp>

#include "luce/Task.hpp"
#include "luce/cpu/cpu.hpp"

#include "luce/Support/isa/riscv32/Instruction.hpp"
#include "luce/Support/isa/riscv32/Decoder.hpp"

namespace accat::luce::isa::riscv32::instruction::base {
using auxilia::as;
#pragma region Cal
auto Add::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] + gpr[rs2()];
  return kSuccess;
}
auto Sub::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] - gpr[rs2()];
  return kSuccess;
}
auto Xor::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] ^ gpr[rs2()];
  return kSuccess;
}
auto Or::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] | gpr[rs2()];
  return kSuccess;
}
auto And::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] & gpr[rs2()];
  return kSuccess;
}
auto Sll::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] << (gpr[rs2()] & 0x1F); // mask the shift
  return kSuccess;
}
auto Srl::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // logical right shift
  gpr[rd()] = gpr[rs1()] >> (gpr[rs2()] & 0x1F); // ditto
  return kSuccess;
}

auto Sra::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // arithmetic right shift (MSB-extended)
  gpr[rd()] = as<signed_num_type>(gpr[rs1()]) >> (gpr[rs2()] & 0x1F);
  return kSuccess;
}

auto Slt::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<signed_num_type>(gpr[rs1()]) < as<signed_num_type>(gpr[rs2()]);
  return kSuccess;
}

auto Sltu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()]);
  return kSuccess;
}
#pragma endregion Cal
#pragma region Imm
auto Addi::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] + imm();
  return kSuccess;
}
auto Xori::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] ^ imm();
  return kSuccess;
}
auto Ori::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] | imm();
  return kSuccess;
}
auto Andi::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] & imm();
  return kSuccess;
}
auto Slli::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 << imm[0:4]
  gpr[rd()] = gpr[rs1()] << (imm() & 0x1F);
  return kSuccess;
}
auto Srli::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  gpr[rd()] = gpr[rs1()] >> (imm() & 0x1F);
  return kSuccess;
}
auto Srai::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  // msb-extend
  gpr[rd()] = as<signed_num_type>(gpr[rs1()]) >> (imm() & 0x1F);
  return kSuccess;
}
auto Slti::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<signed_num_type>(gpr[rs1()] < imm());
  return kSuccess;
}
auto Sltiu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<num_type>(gpr[rs1()] < imm());
  return kSuccess;
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
  gpr[rd()] = as<signed_num_type>(bytes & 0xFF);
  return kSuccess;
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
  gpr[rd()] = as<signed_num_type>(bytes & 0xFFFF);
  return kSuccess;
}

auto Lw::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    return kMemoryViolation;
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<signed_num_type>(bytes);
  return kSuccess;
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
  gpr[rd()] = bytes & 0xFF;
  return kSuccess;
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
  gpr[rd()] = bytes & 0xFFFF;
  return kSuccess;
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
#pragma endregion Store
#pragma region Branch
auto Beq::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  if (gpr[rs1()] == gpr[rs2()]) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}

auto Bne::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  if (gpr[rs1()] != gpr[rs2()]) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}

auto Blt::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  if (as<signed_num_type>(gpr[rs1()]) < as<signed_num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}

auto Bge::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  if (as<signed_num_type>(gpr[rs1()]) >= as<signed_num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}

auto Bltu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned
  auto &gpr = cpu->gpr();
  if (as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}

auto Bgeu::execute(Icpu *cpu) const -> ExecutionStatus {
  // unsigned
  auto &gpr = cpu->gpr();
  if (as<num_type>(gpr[rs1()]) >= as<num_type>(gpr[rs2()])) {
    cpu->pc().num() += imm();
  }
  return kSuccess;
}
#pragma endregion Branch
#pragma region Jump
auto Jal::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = cpu->pc().num() + 4;
  cpu->pc().num() += imm();
  return kSuccess;
}

auto Jalr::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  auto t = cpu->pc().num() + 4;
  cpu->pc().num() = (gpr[rs1()] + imm());
  gpr[rd()] = t;
  return kSuccess;
}
#pragma endregion Jump
#pragma region UpperImm
auto Lui::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = imm() << 12;
  return kSuccess;
}
auto Auipc::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr[rd()] = cpu->pc().num() + (imm() << 12);
  return kSuccess;
}
#pragma endregion UpperImm
#pragma region System
auto Ecall::execute(Icpu *cpu) const -> ExecutionStatus {
  return kEnvCall;
}
auto Ebreak::execute(Icpu *cpu) const -> ExecutionStatus {
  return kEnvBreak;
}
#pragma endregion System
} // namespace accat::luce::isa::riscv32::instruction::base
