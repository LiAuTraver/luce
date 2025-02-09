#include "deps.hh"

#include <cstddef>
#include <functional>
#include <luce/MainMemory.hpp>

namespace accat::luce {
using auxilia::OutOfRangeError;
using auxilia::ResourceExhaustedError;
using auxilia::Status;
using auxilia::StatusOr;
MemoryAccess::MemoryAccess() {
  auto sz = isa::physical_memory_size * sizeof(isa::minimal_addressable_unit_t);
  auto my_alloc = polymorphic_allocator_t::FromSize(sz);
  real_data = std::pmr::vector<std::byte>(&my_alloc);
  real_data.resize(sz);
}

auto MainMemory::read(isa::physical_address_t addr) const noexcept
    -> StatusOr<std::byte> {
  if (!memory.is_in_range(addr)) {
    return MakeMemoryAccessViolationError(addr);
  }
  return {memory[addr]};
}
auto MainMemory::read_n(isa::physical_address_t addr,
                        size_t count) const noexcept
    -> StatusOr<std::span<const std::byte>> {
  if (!memory.is_in_range(addr, addr + count)) {
    return {MakeMemoryAccessViolationError(addr)};
  }
  // dont write `memory.begin() + addr` here; the operator is overloaded with
  // different meaning
  return {std::as_bytes(std::span{&memory[addr], count})};
}

auto MainMemory::write(isa::physical_address_t addr,
                       isa::minimal_addressable_unit_t value) noexcept
    -> Status {
  if (!memory.is_in_range(addr)) {
    return {MakeMemoryAccessViolationError(addr)};
  }
  memory[addr] = static_cast<std::byte>(value);
  return {};
}

void MainMemory::fill(const isa::physical_address_t start,
                      const size_t size,
                      const isa::minimal_addressable_unit_t value) {
  std::ranges::fill_n(
      memory.begin() + start, size, static_cast<std::byte>(value));
}
auto MainMemory::load_program(const std::span<const std::byte> bytes,
                              const isa::physical_address_t start_addr,
                              const isa::physical_address_t block_size,
                              const bool randomize) -> Status {
  if (!memory.is_in_range(start_addr, start_addr + block_size)) {
    return ResourceExhaustedError("Program too large for memory");
  }
  // TODO(...)
  // stupid way
  for (size_t i = 0; i < bytes.size(); ++i) {
    memory[start_addr + i] = bytes[i];
  }
  if (randomize) {
    this->generate(
        start_addr + bytes.size(), block_size - bytes.size(), auxilia::rand_u8);
  }

  return {};
}
void MainMemory::generate(const isa::physical_address_t start,
                          const size_t size,
                          std::invocable auto &&generator) {
  std::ranges::generate_n(memory.begin() + start, size, [&] {
    return static_cast<std::byte>(std::invoke(generator));
  });
}
} // namespace accat::luce
