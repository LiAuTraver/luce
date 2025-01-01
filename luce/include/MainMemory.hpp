#pragma once
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
#include "accat/auxilia/Status.hpp"
#include "accat/auxilia/details/macros.hpp"
#include "accat/auxilia/format.hpp"
#include "accat/auxilia/memory.hpp"
#include "isa/architecture.hpp"

namespace accat::luce {
// template <isa::instruction_set ISA, class Ty = std::byte>
// struct memory_access_iterator : private isa::Architecture<ISA> {
// private:
//   using memory_access_iterator::Architecture::physical_base_address;
//   using
//       typename
//       memory_access_iterator::Architecture::minimal_addressable_unit_t;
//   using typename memory_access_iterator::Architecture::physical_address_t;

// public:
//   using iterator_concept = std::contiguous_iterator_tag;
//   using iterator_category = std::random_access_iterator_tag;
//   using value_type = std::remove_cv_t<Ty>;
//   using difference_type = ptrdiff_t;
//   using pointer = value_type *;
//   using reference = value_type &;

//   constexpr memory_access_iterator() noexcept = default;
//   memory_access_iterator(pointer ptr, pointer begin, pointer end) noexcept
//       : my_ptr(ptr) {
//     dbg_only(my_begin = begin; my_end = end;)
//   }

//   memory_access_iterator(const std::span<value_type> s) noexcept {

//     my_begin = s.data();
//     my_end = s.data() + s.size();
//   }

//   physical_address_t address() const noexcept {
//     return physical_base_address + (my_ptr - my_begin);
//   }

//   [[nodiscard]] constexpr reference operator*() const noexcept {
//     precondition(my_begin,
//                  "cannot dereference value-initialized memory iterator");
//     precondition(my_ptr < my_end, "cannot dereference end memory iterator");
//     return *my_ptr;
//   }

//   [[nodiscard]] constexpr pointer operator->() const noexcept {
//     precondition(my_begin,
//                  "cannot dereference value-initialized memory iterator");
//     precondition(my_ptr < my_end, "cannot dereference end memory iterator");
//     return my_ptr;
//   }

//   constexpr memory_access_iterator &operator++() noexcept {
//     precondition(my_begin,
//                  "cannot increment value-initialized memory iterator");
//     precondition(my_ptr < my_end, "cannot increment memory iterator past
//     end");
//     ++my_ptr;
//     return *this;
//   }

//   constexpr memory_access_iterator operator++(int) noexcept {
//     memory_access_iterator my_tmp{*this};
//     ++*this;
//     return my_tmp;
//   }

//   constexpr memory_access_iterator &operator--() noexcept {
//     precondition(my_begin,
//                  "cannot decrement value-initialized memory iterator");
//     precondition(my_ptr > my_begin,
//                  "cannot decrement memory iterator before begin");
//     --my_ptr;
//     return *this;
//   }

//   constexpr memory_access_iterator operator--(int) noexcept {
//     memory_access_iterator my_tmp{*this};
//     --*this;
//     return my_tmp;
//   }

//   constexpr void verify_offset(
//       [[maybe_unused]] const difference_type my_offset) const noexcept {
//     contract_assert(my_offset != 0,
//                     "cannot seek value-initialized span iterator");
//     contract_assert(my_offset < 0 || my_ptr - my_begin >= -my_offset,
//                     "cannot seek span iterator before begin");
//     contract_assert(my_offset > 0 || my_end - my_ptr >= my_offset,
//                     "cannot seek span iterator after end");
//   }

//   constexpr memory_access_iterator &
//   operator+=(const difference_type my_offset) noexcept {
//     verify_offset(my_offset);
//     my_ptr += my_offset;
//     return *this;
//   }

//   [[nodiscard]] constexpr memory_access_iterator
//   operator+(const difference_type my_offset) const noexcept {
//     memory_access_iterator my_tmp{*this};
//     my_tmp += my_offset;
//     return my_tmp;
//   }

//   [[nodiscard]] friend constexpr memory_access_iterator
//   operator+(const difference_type my_offset,
//             memory_access_iterator my_next) noexcept {
//     my_next += my_offset;
//     return my_next;
//   }

//   constexpr memory_access_iterator &
//   operator-=(const difference_type my_offset) noexcept {
//     precondition(my_offset != std::numeric_limits<difference_type>::min(),
//                  "integer overflow");
//     verify_offset(-my_offset);
//     my_ptr -= my_offset;
//     return *this;
//   }

//   [[nodiscard]] constexpr memory_access_iterator
//   operator-(const difference_type my_offset) const noexcept {
//     memory_access_iterator my_tmp{*this};
//     my_tmp -= my_offset;
//     return my_tmp;
//   }

//   [[nodiscard]] constexpr difference_type
//   operator-(const memory_access_iterator &that) const noexcept {
//     precondition(my_begin == that.my_begin && my_end == that.my_end,
//                  "cannot subtract incompatible span iterators");
//     return my_ptr - that.my_ptr;
//   }

//   [[nodiscard]] constexpr reference
//   operator[](const difference_type my_offset) const noexcept {
//     return *(*this + my_offset);
//   }

//   [[nodiscard]] constexpr bool
//   operator==(const memory_access_iterator &that) const noexcept {
//     precondition(my_begin == that.my_begin && my_end == that.my_end,
//                  "cannot compare incompatible span iterators for equality");
//     return my_ptr == that.my_ptr;
//   }

//   [[nodiscard]] constexpr std::strong_ordering
//   operator<=>(const memory_access_iterator &that) const noexcept {
//     precondition(my_begin == that.my_begin && my_end == that.my_end,
//                  "cannot compare incompatible span iterators");
//     return my_ptr <=> that.my_ptr;
//   }

// private:
//   pointer my_ptr = nullptr;
//   dbg_only(                       //
//       pointer my_begin = nullptr; //
//       pointer my_end = nullptr;   //
//   )
// };

// template class memory_access_iterator<isa::instruction_set::riscv32>;

template <isa::instruction_set ISA>
struct MemoryAccess : private isa::Architecture<ISA> {
  // i don't like `unqualified name lookup` :(
  using MemoryAccess::Architecture::physical_base_address;
  using MemoryAccess::Architecture::physical_memory_begin;
  using MemoryAccess::Architecture::physical_memory_end;
  using MemoryAccess::Architecture::physical_memory_size;

  using typename MemoryAccess::Architecture::minimal_addressable_unit_t;
  using typename MemoryAccess::Architecture::physical_address_t;

  static_assert(sizeof(std::byte) == sizeof(minimal_addressable_unit_t),
                "current implementation requires std::byte to be same size as "
                "minimal_addressable_unit_t");
  using polymorphic_allocator_t = auxilia::FixedSizeMemoryPool;
  std::vector<std::byte> real_data;

public:
  MemoryAccess() {
    auto sz = physical_memory_size * sizeof(minimal_addressable_unit_t) /
              sizeof(typename isa::Architecture<
                     isa::instruction_set::host>::physical_address_t);

    // auto my_alloc = polymorphic_allocator_t::FromSize(sz);
    // real_data = std::pmr::vector<minimal_addressable_unit_t>(&my_alloc);
    real_data.resize(sz);
  }
  auto operator[](this auto &&self, physical_address_t addr) -> decltype(auto) {
    precondition(addr >= physical_base_address &&
                     addr < physical_base_address + physical_memory_size,
                 "Caller should ensure address is within memory bounds before "
                 "accessing the memory. Call is_in_range() first.")

        return self.real_data[addr - physical_base_address];
  }
  auto at(physical_address_t addr) const {
    return real_data.at(addr - physical_base_address);
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
    return physical_base_address + offset;
  }
  template <typename... Args>
  bool is_in_range(physical_address_t addr, Args... addrs) const noexcept
    requires(std::convertible_to<Args, physical_address_t> && ...)
  {
    if constexpr (sizeof...(Args) == 0)
      return addr >= physical_memory_begin && addr < physical_memory_end;

    return (is_in_range(addr) && ... && is_in_range(addrs));
  }
};

template <isa::instruction_set ISA>
class MainMemory : private isa::Architecture<ISA> {

  using typename MainMemory::Architecture::minimal_addressable_unit_t;
  using typename MainMemory::Architecture::physical_address_t;
  MemoryAccess<ISA> memory;

public:
  MainMemory() = default;
  ~MainMemory() = default;

public:
  // Basic memory operations
  auxilia::StatusOr<std::byte> read(physical_address_t addr) const noexcept {
    if (!memory.is_in_range(addr)) {
      return auxilia::OutOfRangeError("Memory access violation");
    }
    return {memory[addr]};
  }

  auxilia::Status write(physical_address_t addr,
                        minimal_addressable_unit_t value) noexcept {
    if (!memory.is_in_range(addr)) {
      return auxilia::OutOfRangeError("Memory access violation");
    }
    memory[addr] = value;
    return auxilia::OkStatus();
  }

  auxilia::Status load_program(const std::ranges::range auto &data,
                               const physical_address_t start_addr =
                                   MemoryAccess<ISA>::physical_base_address) {
    auto bytes = std::as_bytes(data);
    if (!memory.is_in_range(start_addr, start_addr + bytes.size())) {
      return auxilia::ResourceExhaustedError("Program too large for memory");
    }
    // TODO(...)
    // stupid way
    for (size_t i = 0; i < bytes.size(); ++i) {
      memory[start_addr + i] = bytes[i];
    }

    return auxilia::OkStatus();
  }

  // Memory block operations for process management
  // auxilia::Status
  // copy_block(physical_address_t dest, physical_address_t src, size_t size) {
  //   if (!memory.is_in_range(dest, src, src + size)) {
  //     return auxilia::OutOfRangeError("Memory block operation out of
  //     bounds");
  //   }
  //   std::copy(memory.begin() + src,
  //             memory.begin() + src + size,
  //             memory.begin() + dest);
  //   return auxilia::OkStatus();
  // }

  // auxilia::Status zero_block(physical_address_t start, size_t size) {
  //   if (!memory.is_in_range(start, (start + size))) {
  //     return auxilia::OutOfRangeError("Memory block operation out of
  //     bounds");
  //   }
  //   std::fill(memory.begin() + start, memory.begin() + start + size, 0);
  //   return auxilia::OkStatus();
  // }

  // reading different sized values
  template <typename T>
  auxilia::StatusOr<T> read_typed(physical_address_t addr) const {
    if (!memory.is_in_range(addr, addr + sizeof(T))) {
      return auxilia::OutOfRangeError("Memory access violation");
    }
    // until C++23 compilers magic `std::start_lifetime_as`, reading through the
    // lens of another type(except for unsigned char, char, or std::byte) is
    // undefined behavior. currently my stl doesn't support it, so we have to
    // create a copy. when it's supported, we can use the std::start_lifetime_as
    // to avoid the copy.
    T value;
    auto bytes = std::as_writable_bytes(std::span{&value, 1});
    for (size_t i = 0; i < sizeof(T); ++i) {
      bytes[i] = memory[addr + i];
    }
    return value;
  }

  // writing different sized values
  template <typename T>
  auxilia::Status write_typed(physical_address_t addr, const T &value) {
    if (!memory.is_in_range(addr, addr + sizeof(T))) {
      return auxilia::OutOfRangeError("Memory access violation");
    }
    std::span<const T, 1> tmp{&value, 1};
    std::ranges::copy(std::as_bytes(tmp),
                      memory.begin() + addr,
                      memory.begin() + addr + sizeof(T));
    return auxilia::OkStatus();
  }

private:
  MainMemory(const MainMemory &) = delete;
  MainMemory &operator=(const MainMemory &) = delete;

public:
  MainMemory(MainMemory &&) = default;
  MainMemory &operator=(MainMemory &&) = default;
};

} // namespace accat::luce
