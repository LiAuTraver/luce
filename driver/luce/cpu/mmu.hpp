#pragma once

#include "Support/isa/architecture.hpp"
namespace accat::luce {
class CentralProcessingUnit;
// memory management unit -- MMU: converts virtual addresses to physical
// addresses. currently does NOTHING since the v base addr and p base addr are
// the same and our emulator is simple.
// resides in the CPU, no need to mark it as a component
class MemoryManagementUnit {
public:
  using cpu_t = CentralProcessingUnit;
  using paddr_t = isa::physical_address_t;
  using vaddr_t = isa::virtual_address_t;

public:
  MemoryManagementUnit() = default;
  explicit MemoryManagementUnit(cpu_t *cpu) : cpu_(cpu) {}
  MemoryManagementUnit(const MemoryManagementUnit &) = delete;
  MemoryManagementUnit &operator=(const MemoryManagementUnit &) = delete;
  MemoryManagementUnit(MemoryManagementUnit &&) noexcept = default;
  MemoryManagementUnit &operator=(MemoryManagementUnit &&) noexcept = default;
public:
  paddr_t virtual_to_physical(const vaddr_t vaddr) const noexcept {
    return vaddr;
  }
  vaddr_t physical_to_virtual(const paddr_t paddr) const noexcept {
    return paddr;
  }
  auto physical_to_virtual(auto&& whatever) const noexcept {
    return whatever;
  }
public:
  cpu_t *cpu() const noexcept {
    return cpu_;
  }

private:
  cpu_t *cpu_ = nullptr;
};
} // namespace accat::luce
