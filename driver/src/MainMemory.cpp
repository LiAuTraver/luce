#include <cstddef>
#include <deps.hh>

#include <luce/MainMemory.hpp>

namespace accat::luce {
template <isa::instruction_set ISA> MemoryAccess<ISA>::MemoryAccess() {
  auto sz = physical_memory_size * sizeof(minimal_addressable_unit_t);
  auto my_alloc = polymorphic_allocator_t::FromSize(sz);
  real_data = std::pmr::vector<std::byte>(&my_alloc);
  real_data.resize(sz);
}
template <isa::instruction_set ISA>
auxilia::StatusOr<std::byte>
MainMemory<ISA>::read(physical_address_t addr) const noexcept {
  if (!memory.is_in_range(addr)) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  return {memory[addr]};
}
template <isa::instruction_set ISA>
auxilia::Status
MainMemory<ISA>::write(physical_address_t addr,
                       minimal_addressable_unit_t value) noexcept {
  if (!memory.is_in_range(addr)) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  memory[addr] = static_cast<std::byte>(value);
  return auxilia::OkStatus();
}

template <isa::instruction_set ISA>
auxilia::Status MainMemory<ISA>::fill(const physical_address_t start,
                                      const size_t size,
                                      const minimal_addressable_unit_t value) {
  if (!memory.is_in_range(start, start + size)) {
    return auxilia::OutOfRangeError("Memory block operation out of bounds");
  }
  std::ranges::fill_n(memory.begin() + start, size, static_cast<std::byte>(value));
  return auxilia::OkStatus();
}
template <isa::instruction_set ISA>
template <typename CallableRandom>
auxilia::Status MainMemory<ISA>::generate(const physical_address_t start,
                                          const size_t size,
                                          CallableRandom &&generator) {
  if (!memory.is_in_range(start, start + size)) {
    return auxilia::OutOfRangeError("Memory block operation out of bounds");
  }
  std::ranges::generate_n(
      memory.begin() + start, size, std::forward<CallableRandom>(generator));
  return auxilia::OkStatus();
}
template <isa::instruction_set ISA>
template <typename T>
auxilia::StatusOr<T>
MainMemory<ISA>::read_typed(physical_address_t addr) const {
  if (!memory.is_in_range(addr, addr + sizeof(T))) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  //    Solution 1: use std::start_lifetime_as
  // until C++23 compiler magic std utils `std::start_lifetime_as`, reading
  // through the lens of another type(except for unsigned char, char, or
  // std::byte) is undefined behavior. currently my stl doesn't support it, so
  // we have to create a copy. when it's supported, we can use the
  // std::start_lifetime_as to avoid the copy.
  //    Solution 2: use std::as_writable_bytes
  T value;
  auto bytes = std::as_writable_bytes(std::span{&value, 1});
  for (const auto i : std::views::iota(0ull, sizeof(T)))
    bytes[i] = memory[addr + i];

  return value;
  // Solution 3: use std::bit_cast
  // ...
  // Solution 4: use placement new (more verbose)
  //    alignas(T) std::byte buffer[sizeof(T)];
  //    memcpy(buffer, memory.begin() + addr, sizeof(T));
  //    T* value = new (buffer) T;
  //    T  result = *value;
  //    value->~T();
  //    return result;
  // Solution 5: use union  (might not be strictly aligned)
  //    union {
  //      T value;
  //      std::byte bytes[sizeof(T)];
  //    } u;
  //    memcpy(u.bytes, memory.begin() + addr, sizeof(T));
  //    return u.value;
}
template <isa::instruction_set ISA>
template <typename T>
auxilia::Status MainMemory<ISA>::write_typed(physical_address_t addr,
                                             const T &value) {
  if (!memory.is_in_range(addr, addr + sizeof(T))) {
    return auxilia::OutOfRangeError("Memory access violation");
  }
  std::span<const T, 1> tmp{&value, 1};
  std::ranges::copy(std::as_bytes(tmp),
                    memory.begin() + addr,
                    memory.begin() + addr + sizeof(T));
  return auxilia::OkStatus();
}

template class MemoryAccess<isa::instruction_set::riscv32>;
template class MainMemory<isa::instruction_set::riscv32>;
}
