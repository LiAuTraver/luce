#pragma once

#include "./config.hpp"

namespace accat::auxilia {
[[noreturn]] inline void alloc_failed() {
  contract_assert(0, "Failed to allocate memory")
  std::abort();
}
inline void *dynamic_alloc(const size_t size) {
  if (auto ptr = _malloca(size))
    return ptr;
  alloc_failed();
}
/// @note this allocator has some issues when using MSVC compiler
EXPORT_AUXILIA
class MemoryPool : public std::pmr::memory_resource {
private:
  void *const buffer;
  size_t remaining_size;
  const size_t total_size;

public:
  MemoryPool(void *t, const size_t size)
      : buffer(t), remaining_size(size), total_size(size) {}
  static MemoryPool FromSize(const size_t size) noexcept {
    return {dynamic_alloc(size), size};
  }

private:
  virtual void *do_allocate(const size_t bytes, const size_t align) override {
    auto current =
        reinterpret_cast<uintptr_t>(buffer) + total_size - remaining_size;
    auto aligned = (current + align - 1) & ~(align - 1);
    auto aligned_offset = aligned - current;
    if (aligned_offset + bytes > remaining_size) {
      contract_assert(0, "out of memory");
      return nullptr;
    }
    remaining_size -= (aligned_offset + bytes);
    return reinterpret_cast<void *>(aligned);
  };

  virtual void
  do_deallocate(void *, size_t, size_t) noexcept override { /* do nothing */ }

  virtual bool
  do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
    return this == &other;
  }
};
} // namespace accat::auxilia
