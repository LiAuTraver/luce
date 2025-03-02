#pragma once
#include <cstddef>
#include "cpu.hpp"
namespace accat::luce {
/// @implements Component
class CPUs : public Component {
  // for debug and easy to understand, we use 1 cpus
  std::array<std::unique_ptr<isa::Icpu>, 1> cpus{};
  using vaddr_t = isa::virtual_address_t;

public:
  CPUs() = default;
  CPUs(const CPUs &) = delete;
  auto operator=(const CPUs &) = delete;
  CPUs(CPUs &&) noexcept = default;
  CPUs &operator=(CPUs &&) noexcept = default;
  virtual ~CPUs() override = default;

  CPUs(Mediator *parent = nullptr) : Component(parent) {
    cpus[0] = std::make_unique<CentralProcessingUnit>(parent);
  }

public:
  auxilia::Status execute_shuttle() {
    if (cpus[0]->is_vacant()) {
      if (auto res = cpus[0]->execute_shuttle(); !res) {
        return res;
      }
      return {};
    }

    spdlog::warn(
        "No CPU available. \n{}",
        fmt::format(
            fmt::fg(fmt::color::cyan),
            "Note: currently not implemented for multi-core and parallel "
            "execution"));
    return {};
  }
  auto attach_task(Task *task) noexcept -> CPUs & {
    cpus[0]->switch_task(task);
    return *this;
  }
  auto check_atomic(const vaddr_t addr, const size_t size) noexcept -> CPUs & {
    std::ranges::for_each(cpus, [addr, size](auto &cpu) {
      auto &atomic_addr = cpu->atomic_address();
      if (atomic_addr >= addr && *atomic_addr < addr + size) {
        // clear atomic address if it's in the range
        atomic_addr.reset();
      }
    });
    return *this;
  }
};
} // namespace accat::luce
