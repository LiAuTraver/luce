#pragma once
#include "Instruction.hpp"

namespace accat::luce::isa::riscv32::instruction {
class Factory : public Word,
                mixin::opcode0To6Common,
                mixin::rd7To12Common,
                mixin::funct3nd12To15Common,
                mixin::rs1st15To20Common,
                mixin::rs2nd20To25Common {
  using inst_t = Instruction;
  using inst_ptr_t = std::unique_ptr<inst_t>;

protected:
  using Word::Word;

public:
  static inst_ptr_t createInstruction(num_type);
  static inst_ptr_t createInstruction(bytes_type);
  static inst_ptr_t createInstruction(bits_type);

protected:
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
} // namespace accat::luce::isa::riscv32::instruction
