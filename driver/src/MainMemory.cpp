#include "deps.hh"

#include <luce/MainMemory.hpp>

namespace accat::luce {
MemoryAccess::MemoryAccess() {
  auto sz = isa::physical_memory_size * sizeof(isa::minimal_addressable_unit_t);
  auto my_alloc = polymorphic_allocator_t::FromSize(sz);
  real_data = std::pmr::vector<std::byte>(&my_alloc);
  real_data.resize(sz);
}

auxilia::StatusOr<std::byte>
MainMemory::read(isa::physical_address_t addr) const noexcept {
  if (!memory.is_in_range(addr)) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  return {memory[addr]};
}

auxilia::Status MainMemory::write(isa::physical_address_t addr,
                                  isa::minimal_addressable_unit_t value) noexcept {
  if (!memory.is_in_range(addr)) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  memory[addr] = static_cast<std::byte>(value);
  return auxilia::OkStatus();
}

auxilia::Status MainMemory::fill(const isa::physical_address_t start,
                                 const size_t size,
                                 const isa::minimal_addressable_unit_t value) {
  if (!memory.is_in_range(start, start + size)) {
    return auxilia::OutOfRangeError("Memory block operation out of bounds");
  }
  std::ranges::fill_n(
      memory.begin() + start, size, static_cast<std::byte>(value));
  return auxilia::OkStatus();
}

} // namespace accat::luce
