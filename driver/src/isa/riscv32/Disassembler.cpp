#include "luce/Support/isa/riscv32/Disassembler.hpp"

#include "luce/Support/isa/IDisassembler.hpp"
#include "luce/Support/isa/riscv32/instruction/Base.hpp"


namespace accat::luce::isa::riscv32 {
auto Disassembler::initializeDefaultImpl() -> IDisassembler & {
  defer {
    spdlog::info(
        "Successfully initialized rv32i base integer instruction decoder.");
  };
  return this->addDecoder(std::make_unique<instruction::base::Decoder>());
}
} // namespace accat::luce::isa::riscv32
#pragma endregion Decoder
