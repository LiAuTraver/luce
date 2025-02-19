#pragma once
#include <vector>
#include <memory>

#include "IInstruction.hpp"
#include "IDecoder.hpp"
namespace accat::luce::isa {
class IDisassembler {
protected:
  std::vector<std::unique_ptr<IDecoder>> decoders;

public:
  auto disassemble(uint32_t num) -> std::unique_ptr<IInstruction> {
    for (auto &decoder : decoders) {
      if (auto instr = decoder->decode(num); instr) {
        return instr;
      }
    }
    return nullptr;
  };
  auto addDecoder(std::unique_ptr<IDecoder> decoder) -> IDisassembler & {
    decoders.emplace_back(std::move(decoder));
    return *this;
  }
  virtual auto initializeDefault() -> IDisassembler& = 0;
  virtual ~IDisassembler() = default;
};
} // namespace accat::luce::isa
