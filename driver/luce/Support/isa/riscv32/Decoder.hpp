// rewrite this using registry-pattern
#include "Instruction.hpp"
#include "../IDecoder.hpp"
#include "../IDisassembler.hpp"
namespace accat::luce::isa::riscv32::instruction::base {
class DecodeImpl : public Word,
                mixin::opcode0To6Common,
                mixin::rd7To12Common,
                mixin::funct3nd12To15Common,
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
class Decoder : public IDecoder{
public:
  virtual auto decode(uint32_t) -> std::unique_ptr<IInstruction> override;
  virtual ~Decoder() override;
};
} // namespace accat::luce::isa::riscv32::instruction::base
namespace accat::luce::isa::riscv32 {
class Disassembler : public IDisassembler {
public:
  virtual auto initializeDefault() -> IDisassembler& override;
};
} // namespace accat::luce::isa::riscv32
