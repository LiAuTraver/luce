#pragma once
#include "Pattern.hpp"
#include "config.hpp"

#include <accat/auxilia/auxilia.hpp>
#include <luce/isa/architecture.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <type_traits>
#include <vector>
#include <ranges>
#include <bit>
#include <memory>
#include <cstdint>
#include <stdexcept>

namespace accat::luce {

class LUCE_API MemoryAccess {
public:
  static_assert(sizeof(std::byte) == sizeof(isa::minimal_addressable_unit_t),
                "current implementation requires std::byte to be same size as "
                "minimal_addressable_unit_t");
  using polymorphic_allocator_t = auxilia::MemoryPool;
  std::pmr::vector<std::byte> real_data;

  MemoryAccess();
  auto operator[](this auto &&self, isa::physical_address_t addr)
      -> decltype(auto) {
    precondition(addr >= isa::physical_base_address &&
                     addr <
                         isa::physical_base_address + isa::physical_memory_size,
                 "Caller should ensure address is within memory bounds before "
                 "accessing the memory. Call is_in_range() first.")

    return self.real_data[addr - isa::physical_base_address];
  }
  auto at(isa::physical_address_t addr) const {
    return real_data.at(addr - isa::physical_base_address);
  }
  auto begin(this auto &&self) { return self.real_data.begin(); }
  auto end(this auto &&self) { return self.real_data.end(); }
  auto cbegin() const { return real_data.cbegin(); }
  auto cend() const { return real_data.cend(); }
  auto size() const { return real_data.size(); }
  auto data(this auto &&self) -> decltype(auto) {
    return self.real_data.data();
  }
  auto address_of(size_t offset) const {
    return isa::physical_base_address + offset;
  }
#pragma warning(push)
#pragma warning(disable: 4702) // unreachable code
  template <typename... Args>
  bool is_in_range(isa::physical_address_t addr, Args... addrs) const noexcept
    requires(std::convertible_to<Args, isa::physical_address_t> && ...)
  {
    if constexpr (sizeof...(Args) == 0)
      return addr >= isa::physical_memory_begin &&
             addr < isa::physical_memory_end;
    return (is_in_range(addr) && ... && is_in_range(addrs));
  }
};
#pragma warning(pop)

class LUCE_API MainMemory : public Component {

  MemoryAccess memory;

public:
  MainMemory(Mediator *parent) : Component(parent) {}
  ~MainMemory() = default;

public:
  auto read(isa::physical_address_t) const noexcept
      -> auxilia::StatusOr<std::byte>;
  auto read_n(isa::physical_address_t,size_t) const 
      -> auxilia::StatusOr<std::span<const std::byte>>;

  auto write(isa::physical_address_t, isa::minimal_addressable_unit_t) noexcept
      -> auxilia::Status;

  auto load_program(std::span<const std::byte>,
                    isa::physical_address_t,
                    isa::physical_address_t,
                    bool = false) -> auxilia::Status;

  auto fill(isa::physical_address_t, size_t, isa::minimal_addressable_unit_t)
      -> auxilia::Status;

  auto generate(isa::physical_address_t, size_t, std::invocable auto &&)
      -> auxilia::Status;

  template <typename T>
  auxilia::StatusOr<T> read_typed(isa::physical_address_t addr) const {
    if (!memory.is_in_range(addr, addr + sizeof(T))) {
      return auxilia::OutOfRangeError("Memory access violation");
    }
    // Solution 1: use std::start_lifetime_as
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

  template <typename T>
  auxilia::Status write_typed(isa::physical_address_t addr, const T &value) {
    if (!memory.is_in_range(addr, addr + sizeof(T))) {
      return auxilia::OutOfRangeError("Memory access violation");
    }
    std::span<const T, 1> tmp{&value, 1};
    std::ranges::copy(std::as_bytes({&value, 1}),
                      memory.begin() + addr,
                      memory.begin() + addr + sizeof(T));
    return auxilia::OkStatus();
  }

private:
  MainMemory(const MainMemory &) = delete;
  MainMemory &operator=(const MainMemory &) = delete;

public:
  MainMemory(MainMemory &&) noexcept = default;
  MainMemory &operator=(MainMemory &&) noexcept = default;
};
} // namespace accat::luce
