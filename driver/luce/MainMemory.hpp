#pragma once
#include <stdlib.h>
#include <accat/auxilia/auxilia.hpp>
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

#include "Support/isa/Word.hpp"
#include "Support/isa/constants/riscv32.hpp"
#include "luce/Support/utils/Pattern.hpp"
#include "config.hpp"
#include "luce/Support/isa/architecture.hpp"

namespace accat::luce {

class LUCE_API MemoryAccess {
public:
  using polymorphic_allocator_t = auxilia::MemoryPool;
  std::pmr::vector<std::byte> real_data;

  MemoryAccess();
  /// @brief Access the memory at the given address. Equivalent to
  ///
  /// @param addr the address to check, not the index
  auto operator[](this auto &&self, const isa::physical_address_t addr)
      -> decltype(auto) {
    precondition(addr >= isa::physical_base_address &&
                     addr <
                         isa::physical_base_address + isa::physical_memory_size,
                 "Caller should ensure address is within memory bounds before "
                 "accessing the memory. Call is_in_range() first.")

    return self.real_data[addr - isa::physical_base_address];
  }
  constexpr auto at(const isa::physical_address_t addr) const {
    return real_data.at(addr - isa::physical_base_address);
  }
  // does not provide non-const iterators
  auto begin() const {
    return real_data.begin();
  }
  auto end() const {
    return real_data.end();
  }
  auto iter_at_address(this auto &&self, const isa::physical_address_t addr) {
    auto offset = addr - isa::physical_base_address;
    return self.real_data.begin() + offset;
  }
  auto cbegin() const noexcept {
    return real_data.cbegin();
  }
  auto cend() const noexcept {
    return real_data.cend();
  }
  auto size() const noexcept {
    return real_data.size();
  }
  auto empty() const noexcept {
    return real_data.empty();
  }
  auto data(this auto &&self) -> decltype(auto) {
    return self.real_data.data();
  }
  auto addressof(const size_t offset) const {
    return isa::physical_base_address + offset;
  }
  template <typename... Args>
  bool is_in_range(isa::physical_address_t addr, Args... addrs) const noexcept
    requires(std::convertible_to<Args, isa::physical_address_t> && ...)
  {
    if constexpr (sizeof...(Args) == 0)
      return addr >= isa::physical_memory_begin &&
             addr < isa::physical_memory_end;
    else // warning `unreachable code` if not using `else`
      return (is_in_range(addr) && ... && is_in_range(addrs));
  }
};
class Monitor;
class LUCE_API MainMemory : public Component {

  MemoryAccess memory;

private:
  auto monitor() const noexcept -> Monitor *;
  static consteval auto _make_fmt_str() {
    constexpr auto C = sizeof(isa::physical_address_t) * 2;
    static_assert(C == 8 || C == 16, "Invalid address size");
    if constexpr (C == 8)
      return "Memory access violation at address 0x{:08x}";
    else
      return "Memory access violation at address 0x{:016x}";
  }
  AC_FORCEINLINE static auto
  MakeMemoryAccessViolationError(isa::physical_address_t addr) noexcept {
    return auxilia::OutOfRangeError(_make_fmt_str(), addr);
  }
  void _write_unchecked(isa::physical_address_t,
                        std::span<const std::byte>) noexcept;

public:
  auto read(isa::physical_address_t) const noexcept
      -> auxilia::StatusOr<std::byte>;
  auto read_n(isa::physical_address_t, size_t) const noexcept
      -> auxilia::StatusOr<std::span<const std::byte>>;
  auto read_word(const isa::Word addr) const noexcept {
    return read_typed<isa::Word>(addr.num());
  }
  auto write(isa::physical_address_t, isa::minimal_addressable_unit_t) noexcept
      -> auxilia::Status;

  auto write_n(isa::physical_address_t,
               size_t,
               std::span<const std::byte>) noexcept -> auxilia::Status;
  auto write_word(const isa::physical_address_t addr,
                  const isa::Word value) noexcept {
    return write_typed(addr, value.num());
  }
  auto load_program(std::span<const std::byte>,
                    isa::physical_address_t,
                    isa::physical_address_t,
                    bool = false) -> auxilia::Status;

  void fill(isa::physical_address_t, size_t, isa::minimal_addressable_unit_t);

  void generate(isa::physical_address_t, size_t, std::invocable auto &&);

  template <typename T>
  auxilia::StatusOr<T> read_typed(isa::physical_address_t addr) const {
    if (!memory.is_in_range(addr, addr + sizeof(T))) {
      return MakeMemoryAccessViolationError(addr);
    }
    // Solution 1: use std::start_lifetime_as
    // until C++23 compiler magic std utils `std::start_lifetime_as`, reading
    // through the lens of another type(except for unsigned char, char, or
    // std::byte) is undefined behavior. currently my stl doesn't support it, so
    // we have to create a copy. when it's supported, we can use the
    // std::start_lifetime_as to avoid the copy.

    // Solution 2: use std::as_writable_bytes
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
      return MakeMemoryAccessViolationError(addr);
    }
    _write_unchecked(addr, std::as_bytes(std::span{&value, 1}));
    return {};
  }

public:
  MainMemory(Mediator *parent) : Component(parent) {}
  ~MainMemory() = default;
  MainMemory(const MainMemory &) = delete;
  MainMemory &operator=(const MainMemory &) = delete;
  MainMemory(MainMemory &&) noexcept = default;
  MainMemory &operator=(MainMemory &&) noexcept = default;
};
} // namespace accat::luce
