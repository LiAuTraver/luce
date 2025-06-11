#pragma once
#include <vector>
#include <memory>

#include "IInstruction.hpp"
#include "IDecoder.hpp"
#include "accat/auxilia/details/macros.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
namespace accat::luce::isa {
class IDisassembler {
protected:
  std::vector<std::unique_ptr<IDecoder>> decoders;

public:
  auto disassemble(uint32_t num) -> std::unique_ptr<IInstruction> {
    contract_assert(initialized_, "Disassembler not initialized");
    for (auto &decoder : decoders)
      if (auto instr = decoder->decode(num))
        return instr;

    return nullptr;
  };
  auto addDecoder(std::unique_ptr<IDecoder> decoder) -> IDisassembler & {
    dbg(info, "Adding decoder: {}", typeid(*decoder).name());
    decoders.emplace_back(std::move(decoder));
    return *this;
  }

public:
  auto initializeDefault() -> IDisassembler & {
    if (!initialized_) {
      initialized_ = true;
      return initializeDefaultImpl();
    }
    return *this;
  }
  virtual ~IDisassembler() = default;

public:
  auto initialized() const noexcept -> bool {
    return initialized_;
  }

private:
  virtual auto initializeDefaultImpl() -> IDisassembler & = 0;
  bool initialized_ = false;
};
#pragma clang diagnostic pop
} // namespace accat::luce::isa
