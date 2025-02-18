#include "luce/Support/isa/riscv32/Decoder.hpp"
#include "luce/Support/isa/IDisassembler.hpp"
#include "accat/auxilia/details/macros.hpp"

namespace accat::luce::isa::riscv32::instruction::base {
#pragma region DecodeImpl
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
  const auto f7 = extractBits<25, 32>(num());
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

  const auto imm = extractBits<20, 32>(num());
  if (imm == 0x000) {
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
Decoder::~Decoder() = default;
} // namespace accat::luce::isa::riscv32::instruction::base
namespace accat::luce::isa::riscv32 {
auto Disassembler::initializeDefault() -> IDisassembler & {
  defer {
    spdlog::info(
        "Successfully initialized rv32i base integer instruction decoder.");
  };
  return this->addDecoder(std::make_unique<instruction::base::Decoder>());
}
} // namespace accat::luce::isa::riscv32
#pragma endregion Decoder
