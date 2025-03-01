#pragma once
#include "luce/Support/isa/IDisassembler.hpp"
namespace accat::luce::isa::riscv32 {
class Disassembler : public IDisassembler {
public:
  virtual ~Disassembler() = default;

private:
  virtual auto initializeDefaultImpl() -> IDisassembler & override;
};
} // namespace accat::luce::isa::riscv32
