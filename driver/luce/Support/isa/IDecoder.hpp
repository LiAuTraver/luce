#pragma once
#include <memory>
#include "IInstruction.hpp"
namespace accat::luce::isa {
struct IDecoder {
  virtual auto decode(uint32_t) -> std::unique_ptr<IInstruction> = 0;
  virtual ~IDecoder() = default;
};
}