#include "deps.hh"

#include <cstddef>
#include <functional>
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
auto MainMemory::read_n(isa::physical_address_t addr, size_t count) const
    -> auxilia::StatusOr<std::span<const std::byte>> {
  if (!memory.is_in_range(addr, addr + count)) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  // dont write `memory.begin() + addr` here; the operator is overloaded with
  // different meaning
  return std::as_bytes(std::span{&memory[addr], count});
}

auxilia::Status
MainMemory::write(isa::physical_address_t addr,
                  isa::minimal_addressable_unit_t value) noexcept {
  if (!memory.is_in_range(addr)) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  memory[addr] = static_cast<std::byte>(value);
  return {};
}

auxilia::Status MainMemory::fill(const isa::physical_address_t start,
                                 const size_t size,
                                 const isa::minimal_addressable_unit_t value) {
  if (!memory.is_in_range(start, start + size)) {
    return auxilia::OutOfRangeError("Memory block operation out of bounds");
  }
  std::ranges::fill_n(
      memory.begin() + start, size, static_cast<std::byte>(value));
  return {};
}
auto MainMemory::load_program(const std::span<const std::byte> bytes,
                              const isa::physical_address_t start_addr,
                              const isa::physical_address_t block_size,
                              const bool randomize) -> auxilia::Status {
  if (!memory.is_in_range(start_addr, start_addr + block_size)) {
    return auxilia::ResourceExhaustedError("Program too large for memory");
  }
  // TODO(...)
  // stupid way
  for (size_t i = 0; i < bytes.size(); ++i) {
    memory[start_addr + i] = bytes[i];
  }
  if (randomize) {
    this->generate(start_addr + bytes.size(),
                   block_size - bytes.size(),
                   auxilia::rand_u8);
  }

  return {};
}
auto MainMemory::generate(const isa::physical_address_t start,
                          const size_t size,
                          std::invocable auto &&generator) -> auxilia::Status {
  if (!memory.is_in_range(start, start + size)) {
    return auxilia::OutOfRangeError("Memory block operation out of bounds");
  }
  std::ranges::generate_n(memory.begin() + start, size, [&] {
    return static_cast<std::byte>(std::invoke(generator));
  });
  return {};
}
} // namespace accat::luce
