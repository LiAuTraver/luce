#include <algorithm>
#include <cstdint>
#include <memory>
#include <span>

#include <accat/auxilia/auxilia.hpp>

#include "luce/Task.hpp"
#include "luce/cpu/cpu.hpp"

namespace accat::luce::isa::riscv32::instruction {
using CPU = CentralProcessingUnit;
using auxilia::as;
#pragma region Cal
void Add::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] + gpr[rs2()];
}
void Sub::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] - gpr[rs2()];
}
void Xor::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] ^ gpr[rs2()];
}
void Or::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] | gpr[rs2()];
}
void And::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] & gpr[rs2()];
}
void Sll::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] << (gpr[rs2()] & 0x1F);
}
void Srl::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  // logical right shift
  gpr[rd()] = gpr[rs1()] >> (gpr[rs2()] & 0x1F);
}

void Sra::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  // arithmetic right shift (MSB-extended)
  gpr[rd()] = as<std::int32_t>(gpr[rs1()]) >> (gpr[rs2()] & 0x1F);
}

void Slt::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = as<std::int32_t>(gpr[rs1()]) < as<std::int32_t>(gpr[rs2()]);
}

void Sltu::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()]);
}
#pragma endregion Cal

#pragma region Imm
void Addi::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] + imm();
}
void Xori::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] ^ imm();
}
void Ori::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] | imm();
}
void Andi::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = gpr[rs1()] & imm();
}
void Slli::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  // rd = rs1 << imm[0:4]
  gpr[rd()] = gpr[rs1()] << (imm() & 0x1F);
}
void Srli::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  // rd = rs1 >> imm[0:4]
  gpr[rd()] = gpr[rs1()] >> (imm() & 0x1F);
}
void Srai::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  // rd = rs1 >> imm[0:4]
  // msb-extend
  gpr[rd()] = as<std::int32_t>(gpr[rs1()]) >> (imm() & 0x1F);
}
void Slti::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = as<num_type>(gpr[rs1()] < imm());
}
void Sltiu::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = as<num_type>(gpr[rs1()] < imm());
}

void Lb::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<std::int32_t>(bytes & 0xFF);
}

void Lh::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<std::int32_t>(bytes & 0xFFFF);
}

void Lw::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  auto maybe_bytes = cpu->fetch(gpr[rs1()] + imm());
  if (!maybe_bytes) {
    TODO(...)
  }
  auto bytes =
      *reinterpret_cast<const num_type *>(std::move(maybe_bytes)->data());
  gpr[rd()] = as<std::int32_t>(bytes);
}

void Lbu::execute(CPU *cpu) const {
  // zero-extend
  TODO(...)
}

void Lhu::execute(CPU *cpu) const {
  // zero-extend
  TODO(...)
}
#pragma endregion Imm
#pragma region Store
void Sb::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
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

void Sh::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
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

void Sw::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  // M[rs1+imm][0:31] = rs2[0:31]
  auto status =
      cpu->write(gpr[rs1()] + imm(), std::as_bytes(std::span{&gpr[rs2()], 4}));
  if (!status) {
    TODO(...)
  }
}
#pragma endregion Store
#pragma region Branch
void Beq::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  if (gpr[rs1()] == gpr[rs2()]) {
    cpu->context()->program_counter += imm();
  }
}

void Bne::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  if (gpr[rs1()] != gpr[rs2()]) {
    cpu->context()->program_counter += imm();
  }
}

void Blt::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  if (as<std::int32_t>(gpr[rs1()]) < as<std::int32_t>(gpr[rs2()])) {
    cpu->context()->program_counter += imm();
  }
}

void Bge::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  if (as<std::int32_t>(gpr[rs1()]) >= as<std::int32_t>(gpr[rs2()])) {
    cpu->context()->program_counter += imm();
  }
}

void Bltu::execute(CPU *cpu) const {
  TODO(...)
}

void Bgeu::execute(CPU *cpu) const {
  TODO(...)
}
#pragma endregion Branch
#pragma region Jump
void Jal::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = cpu->context()->program_counter + 4;
  cpu->context()->program_counter += imm();
}

void Jalr::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  auto t = cpu->context()->program_counter + 4;
  cpu->context()->program_counter = (gpr[rs1()] + imm());
  gpr[rd()] = t;
}
#pragma endregion Jump
#pragma region UpperImm
void Lui::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = imm() << 12;
}
void Auipc::execute(CPU *cpu) const {
  auto &gpr = *cpu->context()->general_purpose_registers();
  gpr[rd()] = cpu->context()->program_counter + (imm() << 12);
}
#pragma endregion UpperImm
#pragma region System
void Ecall::execute(CPU *cpu) const {
  TODO(...)
}
void Ebreak::execute(CPU *cpu) const {
  TODO(...)
}
#pragma endregion System
} // namespace accat::luce::isa::riscv32::instruction
