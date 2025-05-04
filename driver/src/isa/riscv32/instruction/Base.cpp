#include <accat/auxilia/auxilia.hpp>

#include "luce/Support/isa/IInstruction.hpp"
#include "luce/Support/isa/Icpu.hpp"

#include "luce/Support/isa/riscv32/instruction/Base.hpp"

namespace accat::luce::isa::riscv32::instruction::base {
using auxilia::as;
using auxilia::FormatPolicy;
#pragma region Cal
auto Add::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] + gpr[rs2()];
  return kOk;
}
auto Add::asmStr() const noexcept -> string_type {
  return fmt::format("add x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Sub::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] - gpr[rs2()];
  return kOk;
}
auto Sub::asmStr() const noexcept -> string_type {
  return fmt::format("sub x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Xor::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] ^ gpr[rs2()];
  return kOk;
}
auto Xor::asmStr() const noexcept -> string_type {
  return fmt::format("xor x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Or::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] | gpr[rs2()];
  return kOk;
}
auto Or::asmStr() const noexcept -> string_type {
  return fmt::format("or x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto And::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] & gpr[rs2()];
  return kOk;
}
auto And::asmStr() const noexcept -> string_type {
  return fmt::format("and x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Sll::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] << (gpr[rs2()] & 0x1F); // mask the shift
  return kOk;
}
auto Sll::asmStr() const noexcept -> string_type {
  return fmt::format("sll x{}, x{}, x{}", rd(), rs1(), rs2());
}
auto Srl::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // logical right shift
  gpr.write_at(rd()) = gpr[rs1()] >> (gpr[rs2()] & 0x1F); // ditto
  return kOk;
}
auto Srl::asmStr() const noexcept -> string_type {
  return fmt::format("srl x{}, x{}, x{}", rd(), rs1(), rs2());
}

auto Sra::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // arithmetic right shift (MSB-extended)
  gpr.write_at(rd()) = as<signed_num_type>(gpr[rs1()]) >> (gpr[rs2()] & 0x1F);
  return kOk;
}
auto Sra::asmStr() const noexcept -> string_type {
  return fmt::format("sra x{}, x{}, x{}", rd(), rs1(), rs2());
}

auto Slt::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) =
      as<signed_num_type>(gpr[rs1()]) < as<signed_num_type>(gpr[rs2()]);
  return kOk;
}
auto Slt::asmStr() const noexcept -> string_type {
  return fmt::format("slt x{}, x{}, x{}", rd(), rs1(), rs2());
}

auto Sltu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()]);
  return kOk;
}
auto Sltu::asmStr() const noexcept -> string_type {
  return fmt::format("sltu x{}, x{}, x{}", rd(), rs1(), rs2());
}
#pragma endregion Cal
#pragma region Imm
auto Addi::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] + imm();
  return kOk;
}
auto Addi::asmStr() const noexcept -> string_type {
  return fmt::format("addi x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Xori::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] ^ imm();
  return kOk;
}
auto Xori::asmStr() const noexcept -> string_type {
  return fmt::format("xori x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Ori::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] | imm();
  return kOk;
}
auto Ori::asmStr() const noexcept -> string_type {
  return fmt::format("ori x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Andi::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = gpr[rs1()] & imm();
  return kOk;
}
auto Andi::asmStr() const noexcept -> string_type {
  return fmt::format("andi x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Slli::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 << imm[0:4]
  gpr.write_at(rd()) = gpr[rs1()] << (imm() & 0x1F);
  return kOk;
}
auto Slli::asmStr() const noexcept -> string_type {
  return fmt::format("slli x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Srli::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  gpr.write_at(rd()) = gpr[rs1()] >> (imm() & 0x1F);
  return kOk;
}
auto Srli::asmStr() const noexcept -> string_type {
  return fmt::format("srli x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Srai::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  // rd = rs1 >> imm[0:4]
  // msb-extend
  gpr.write_at(rd()) = as<signed_num_type>(gpr[rs1()]) >> (imm() & 0x1F);
  return kOk;
}
auto Srai::asmStr() const noexcept -> string_type {
  return fmt::format("srai x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Slti::execute(Icpu *cpu) const -> ExecutionStatus {
  // signed
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = as<signed_num_type>(gpr[rs1()] < imm());
  return kOk;
}
auto Slti::asmStr() const noexcept -> string_type {
  return fmt::format("slti x{}, x{}, {}", rd(), rs1(), imm_str());
}
auto Sltiu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = as<num_type>(gpr[rs1()] < imm());
  return kOk;
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
  return kOk;
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
  return kOk;
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
  return kOk;
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
  return kOk;
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
  return kOk;
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
  return kOk;
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
  return kOk;
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
  return kOk;
}
auto Sw::asmStr() const noexcept -> string_type {
  return fmt::format("sw x{}, {}(x{})", rs2(), imm_str(), rs1());
}
#pragma endregion Store
#pragma region Branch
auto Beq::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  cpu->pc().num() += (gpr[rs1()] == gpr[rs2()]) ? imm() : 4;
  return kOkButDontBotherPC;
}
auto Beq::asmStr() const noexcept -> string_type {
  return fmt::format("beq x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bne::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  cpu->pc().num() += (gpr[rs1()] != gpr[rs2()]) ? imm() : 4;
  return kOkButDontBotherPC;
}
auto Bne::asmStr() const noexcept -> string_type {
  return fmt::format("bne x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Blt::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  cpu->pc().num() +=
      (as<signed_num_type>(gpr[rs1()]) < as<signed_num_type>(gpr[rs2()]))
          ? imm()
          : 4;
  return kOkButDontBotherPC;
}
auto Blt::asmStr() const noexcept -> string_type {
  return fmt::format("blt x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bge::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  cpu->pc().num() +=
      (as<signed_num_type>(gpr[rs1()]) >= as<signed_num_type>(gpr[rs2()]))
          ? imm()
          : 4;
  return kOkButDontBotherPC;
}
auto Bge::asmStr() const noexcept -> string_type {
  return fmt::format("bge x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bltu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  cpu->pc().num() +=
      (as<num_type>(gpr[rs1()]) < as<num_type>(gpr[rs2()])) ? imm() : 4;
  return kOkButDontBotherPC;
}
auto Bltu::asmStr() const noexcept -> string_type {
  return fmt::format("bltu x{}, x{}, {}", rs1(), rs2(), imm_str());
}

auto Bgeu::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  cpu->pc().num() +=
      (as<num_type>(gpr[rs1()]) >= as<num_type>(gpr[rs2()])) ? imm() : 4;
  return kOkButDontBotherPC;
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
  return kOkButDontBotherPC;
}
auto Jal::asmStr() const noexcept -> string_type {
  return fmt::format("jal x{}, {}", rd(), imm_str());
}

auto Jalr::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  auto t = cpu->pc().num() + 4;
  cpu->pc().num() = (gpr[rs1()] + imm());
  gpr.write_at(rd()) = t;
  return kOkButDontBotherPC;
}
auto Jalr::asmStr() const noexcept -> string_type {
  return fmt::format("jalr x{}, x{}, {}", rd(), rs1(), imm_str());
}
#pragma endregion Jump
#pragma region UpperImm
auto Lui::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = imm();
  return kOk;
}
auto Lui::asmStr() const noexcept -> string_type {
  return fmt::format("lui x{}, {}", rd(), imm_str());
}
auto Auipc::execute(Icpu *cpu) const -> ExecutionStatus {
  auto &gpr = cpu->gpr();
  gpr.write_at(rd()) = cpu->pc().num() + imm();
  return kOk;
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
#pragma region DecodeImpl
class DecodeImpl : public Word,
                   mixin::opcode0To6Common,
                   mixin::rd7To12Common,
                   mixin::funct3rd12To15Common,
                   mixin::funct7th25To32Common,
                   mixin::rs1st15To20Common,
                   mixin::rs2nd20To25Common {
  using inst_t = IInstruction;
  using inst_ptr_t = std::unique_ptr<inst_t>;
  friend class Decoder;

protected:
  using Word::Word;
  inst_ptr_t decode() const;
  inst_ptr_t decodeBaseRType() const;
  inst_ptr_t decodeBaseIType() const;
  inst_ptr_t decodeLoadIType() const;
  inst_ptr_t decodeJalr() const;
  inst_ptr_t decodeSpecialCategory() const;
  inst_ptr_t decodeSType() const;
  inst_ptr_t decodeBType() const;
  inst_ptr_t decodeLui() const;
  inst_ptr_t decodeAuipc() const;
  inst_ptr_t decodeJal() const;
};
auto DecodeImpl::decode() const -> inst_ptr_t {
  switch (opcode()) {
  case 0b0110011:
    return decodeBaseRType();
  case 0b0010011:
    return decodeBaseIType();
  case 0b0000011:
    return decodeLoadIType();
  case 0b0100011:
    return decodeSType();
  case 0b1100011:
    return decodeBType();
  case 0b0110111:
    return decodeLui();
  case 0b0010111:
    return decodeAuipc();
  case 0b1101111:
    return decodeJal();
  case 0b1100111:
    return decodeJalr();
  case 0b1110011:
    return decodeSpecialCategory();
  }
  return nullptr;
}
auto DecodeImpl::decodeBaseRType() const -> inst_ptr_t {
  precondition(opcode() == 0b0110011, "Not R-type");
  const auto f3 = funct3();
  const auto f7 = funct7();
  if (f3 == 0x0 and f7 == 0x00) {
    return std::make_unique<Add>(num());
  } else if (f3 == 0x0 and f7 == 0x20) {
    return std::make_unique<Sub>(num());
  } else if (f3 == 0x4 and f7 == 0x00) {
    return std::make_unique<Xor>(num());
  } else if (f3 == 0x6 and f7 == 0x00) {
    return std::make_unique<Or>(num());
  } else if (f3 == 0x7 and f7 == 0x00) {
    return std::make_unique<And>(num());
  } else if (f3 == 0x1 and f7 == 0x00) {
    return std::make_unique<Sll>(num());
  } else if (f3 == 0x5 and f7 == 0x00) {
    return std::make_unique<Srl>(num());
  } else if (f3 == 0x5 and f7 == 0x20) {
    return std::make_unique<Sra>(num());
  } else if (f3 == 0x2 and f7 == 0x00) {
    return std::make_unique<Slt>(num());
  } else if (f3 == 0x3 and f7 == 0x00) {
    return std::make_unique<Sltu>(num());
  } else {
    return nullptr;
  }
}
auto DecodeImpl::decodeBaseIType() const -> inst_ptr_t {
  const auto imm5To11 = extractBits<25, 32>(num());
  const auto f3 = funct3();
  if (f3 == 0x0) {
    return std::make_unique<Addi>(num());
  } else if (f3 == 0x4) {
    return std::make_unique<Xori>(num());
  } else if (f3 == 0x6) {
    return std::make_unique<Ori>(num());
  } else if (f3 == 0x7) {
    return std::make_unique<Andi>(num());
  } else if (f3 == 0x1 and imm5To11 == 0x00) {
    return std::make_unique<Slli>(num());
  } else if (f3 == 0x5 and imm5To11 == 0x00) {
    return std::make_unique<Srli>(num());
  } else if (f3 == 0x5 and imm5To11 == 0x20) {
    return std::make_unique<Srai>(num());
  } else if (f3 == 0x2) {
    return std::make_unique<Slti>(num());
  } else if (f3 == 0x3) {
    return std::make_unique<Sltiu>(num());
  } else {
    return nullptr;
  }
}
DecodeImpl::inst_ptr_t DecodeImpl::decodeLoadIType() const {
  switch (funct3()) {
  case 0x0:
    return std::make_unique<Lb>(num());
  case 0x1:
    return std::make_unique<Lh>(num());
  case 0x2:
    return std::make_unique<Lw>(num());
  case 0x4:
    return std::make_unique<Lbu>(num());
  case 0x5:
    return std::make_unique<Lhu>(num());
  }
  return nullptr;
}
DecodeImpl::inst_ptr_t DecodeImpl::decodeJalr() const {
  precondition(opcode() == 0b1100111, "Not a JALR type");
  if (funct3() != 0x0)
    return nullptr;
  return std::make_unique<Jalr>(num());
}
DecodeImpl::inst_ptr_t DecodeImpl::decodeSpecialCategory() const {
  precondition(opcode() == 0b1110011, "Not a system type");
  if (funct3() != 0x0)
    return nullptr;

  if (const auto imm = extractBits<20, 32>(num()); imm == 0x000) {
    return std::make_unique<Ecall>(num());
  } else if (imm == 0x001) {
    return std::make_unique<Ebreak>(num());
  } else {
    return nullptr;
  }
}
auto DecodeImpl::decodeSType() const -> inst_ptr_t {
  precondition(opcode() == 0b0100011, "Not a S-type");

  switch (funct3()) {
  case 0x0:
    return std::make_unique<Sb>(num());
  case 0x1:
    return std::make_unique<Sh>(num());
  case 0x2:
    return std::make_unique<Sw>(num());
  }
  return nullptr;
}
auto DecodeImpl::decodeBType() const -> inst_ptr_t {
  precondition(opcode() == 0b1100011, "Not a B-type");

  switch (funct3()) {
  case 0x0:
    return std::make_unique<Beq>(num());
  case 0x1:
    return std::make_unique<Bne>(num());
  case 0x4:
    return std::make_unique<Blt>(num());
  case 0x5:
    return std::make_unique<Bge>(num());
  case 0x6:
    return std::make_unique<Bltu>(num());
  case 0x7:
    return std::make_unique<Bgeu>(num());
  }
  return nullptr;
}
auto DecodeImpl::decodeLui() const -> inst_ptr_t {
  precondition(opcode() == 0b0110111, "Not a LUI type");
  return std::make_unique<Lui>(num());
}
auto DecodeImpl::decodeAuipc() const -> inst_ptr_t {
  precondition(opcode() == 0b0010111, "Not an AUIPC type");
  return std::make_unique<Auipc>(num());
}
auto DecodeImpl::decodeJal() const -> inst_ptr_t {
  precondition(opcode() == 0b1101111, "Not a JAL type");
  return std::make_unique<Jal>(num());
}
#pragma endregion DecodeImpl

#pragma region Decoder
auto Decoder::decode(uint32_t inst) -> std::unique_ptr<IInstruction> {
  DecodeImpl decoder(inst);
  return decoder.decode();
}
#pragma endregion Decoder
} // namespace accat::luce::isa::riscv32::instruction::base
