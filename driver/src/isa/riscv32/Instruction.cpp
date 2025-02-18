#include <algorithm>
#include <cstdint>
#include <memory>
#include <span>

#include <accat/auxilia/auxilia.hpp>

#include "luce/Task.hpp"
#include "luce/cpu/cpu.hpp"

#include "luce/Support/isa/riscv32/Instruction.hpp"
#include "luce/Support/isa/riscv32/Decoder.hpp"

namespace accat::luce::isa::riscv32::instruction {
using auxilia::as;
#pragma region Cal
void Add::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] + gpr[rs2()];
}
void Sub::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] - gpr[rs2()];
}
void Xor::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] ^ gpr[rs2()];
}
void Or::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] | gpr[rs2()];
}
void And::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] & gpr[rs2()];
}
void Sll::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] << (gpr[rs2()] & 0x1F);
}
void Srl::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // logical right shift
  gpr[rd()] = gpr[rs1()] >> (gpr[rs2()] & 0x1F);
}

void Sra::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // arithmetic right shift (MSB-extended)
  gpr[rd()] = as<std::int32_t>(gpr[rs1()]) >> (gpr[rs2()] & 0x1F);
}

void Slt::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<std::int32_t>(gpr[rs1()]) < as<std::int32_t>(gpr[rs2()]);
}

void Sltu::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()]);
}
#pragma endregion Cal
#pragma region Imm
void Addi::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] + imm();
}
void Xori::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] ^ imm();
}
void Ori::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] | imm();
}
void Andi::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = gpr[rs1()] & imm();
}
void Slli::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // rd = rs1 << imm[0:4]
  gpr[rd()] = gpr[rs1()] << (imm() & 0x1F);
}
void Srli::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  gpr[rd()] = gpr[rs1()] >> (imm() & 0x1F);
}
void Srai::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  // msb-extend
  gpr[rd()] = as<std::int32_t>(gpr[rs1()]) >> (imm() & 0x1F);
}
void Slti::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<num_type>(gpr[rs1()] < imm());
}
void Sltiu::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = as<num_type>(gpr[rs1()] < imm());
}

void Lb::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<std::int32_t>(bytes & 0xFF);
}

void Lh::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<std::int32_t>(bytes & 0xFFFF);
}

void Lw::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<std::int32_t>(bytes);
}

void Lbu::execute(Icpu *cpu) const {
  // zero-extend
  TODO(...)
}

void Lhu::execute(Icpu *cpu) const {
  // zero-extend
  TODO(...)
}
#pragma endregion Imm
#pragma region Store
void Sb::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // M[rs1+imm][0:7] = rs2[0:7]
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  bytes = (bytes & 0xFFFFFF00) | (gpr[rs2()] & 0xFF);
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&bytes, 1}));
  if (!status) {
    TODO(...)
  }
}

void Sh::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // M[rs1+imm][0:15] = rs2[0:15]
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  bytes = (bytes & 0xFFFF0000) | (gpr[rs2()] & 0xFFFF);
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&bytes, 2}));
  if (!status) {
    TODO(...)
  }
}

void Sw::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  // M[rs1+imm][0:31] = rs2[0:31]
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&gpr[rs2()], 4}));
  if (!status) {
    TODO(...)
  }
}
#pragma endregion Store
#pragma region Branch
void Beq::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  if (gpr[rs1()] == gpr[rs2()]) {
    cpu->context()->program_counter.num() += imm();
  }
}

void Bne::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  if (gpr[rs1()] != gpr[rs2()]) {
    cpu->context()->program_counter.num() += imm();
  }
}

void Blt::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  if (as<std::int32_t>(gpr[rs1()]) < as<std::int32_t>(gpr[rs2()])) {
    cpu->context()->program_counter.num() += imm();
  }
}

void Bge::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  if (as<std::int32_t>(gpr[rs1()]) >= as<std::int32_t>(gpr[rs2()])) {
    cpu->context()->program_counter.num() += imm();
  }
}

void Bltu::execute(Icpu *cpu) const {
  TODO(...)
}

void Bgeu::execute(Icpu *cpu) const {
  TODO(...)
}
#pragma endregion Branch
#pragma region Jump
void Jal::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = cpu->context()->program_counter.num() + 4;
  cpu->context()->program_counter.num() += imm();
}

void Jalr::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  auto t = cpu->context()->program_counter.num() + 4;
  cpu->context()->program_counter.num() = (gpr[rs1()] + imm());
  gpr[rd()] = t;
}
#pragma endregion Jump
#pragma region UpperImm
void Lui::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = imm() << 12;
}
void Auipc::execute(Icpu *cpu) const {
  auto &gpr = cpu->gpr();
  gpr[rd()] = cpu->context()->program_counter.num() + (imm() << 12);
}
#pragma endregion UpperImm
#pragma region System
void Ecall::execute(Icpu *cpu) const {
  TODO(...)
}
void Ebreak::execute(Icpu *cpu) const {
  TODO(...)
}
#pragma endregion System
}
