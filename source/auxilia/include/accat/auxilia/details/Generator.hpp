#pragma once

#include <algorithm>
#include <coroutine>
#include <optional>
#include <type_traits>
#include "./Status.hpp"

namespace accat::auxilia {
/// @brief A generator class that can be used to generate values
/// lazily, and can be used in a range-based for loop.
/// @tparam YieldType the type of the value to be yielded
/// @tparam ReturnType the type of the value to be returned, usually
/// its 'void' for no return value(infinite sequence generator)
/// @tparam AllocatorType the allocator type to be used for the generator
template <typename YieldType,
          typename ReturnType = void,
          typename AllocatorType = std::allocator<char>>
class Generator {
  static_assert(std::is_default_constructible_v<YieldType>,
                "YieldType must be default constructible");
  static_assert(std::is_nothrow_move_constructible_v<YieldType>,
                "YieldType must be nothrow move constructible");
  static_assert(!std::is_same_v<YieldType, void>,
                "YieldType must not be void; why use a generator for void?");

public:
  template <typename R, bool = std::is_void_v<R>> struct promise_type_impl;
  using promise_type = promise_type_impl<ReturnType>;

private:
  template <typename Derived> struct promise_type_base {
    ~promise_type_base() {}
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    std::exception_ptr exception;
#endif

    Generator get_return_object() {
      return Generator{std::coroutine_handle<promise_type>::from_promise(
          *static_cast<Derived *>(this))};
    }

    std::suspend_always initial_suspend() noexcept {
      return {};
    }
    std::suspend_always final_suspend() noexcept {
      return {};
    }

    std::suspend_always yield_value(const YieldType &value) noexcept {
      static_cast<Derived *>(this)->current_value = std::addressof(value);
      return {};
    }

#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    void unhandled_exception() noexcept {
      exception = std::current_exception();
    }

    void _rethrow_if_exception() {
      if (exception) {
        std::rethrow_exception(exception);
      }
    }

#else
    void unhandled_exception() noexcept {}
#endif

    using CharAlloc = typename std::allocator_traits<
        AllocatorType>::template rebind_alloc<char>;
    static void *operator new(size_t size) {
      CharAlloc my_alloc{};
      return std::allocator_traits<CharAlloc>::allocate(my_alloc, size);
    }

    static void operator delete(void *ptr, size_t size) noexcept {
      CharAlloc my_alloc{};
      std::allocator_traits<CharAlloc>::deallocate(
          my_alloc, static_cast<char *>(ptr), size);
    }
  };

  template <typename R>
  struct promise_type_impl<R, false> : public promise_type_base<promise_type> {
    static_assert(std::is_nothrow_move_constructible_v<R>,
                  "ReturnType must be nothrow move constructible");
    union {
      const YieldType *current_value = nullptr;
      R final_return_value;
    };
    void return_value(const R &value) noexcept {
      final_return_value = value;
    }
  };
  template <typename R>
  struct promise_type_impl<R, true> : public promise_type_base<promise_type> {
    const YieldType *current_value = nullptr;
    void return_void() noexcept {}
  };

public:
  struct iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = YieldType;
    using difference_type = std::ptrdiff_t;
    using pointer = const YieldType *;
    using reference = const YieldType &;

    iterator() = default;
    iterator(std::coroutine_handle<promise_type> handle) : handle_(handle) {
      if (handle_) {
        handle_.resume();
        if (handle_.done()) {
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
          handle_.promise()._rethrow_if_exception();
#endif
          handle_ = nullptr;
        }
      }
    }

    iterator &operator++() {
      handle_.resume();
      if (handle_.done()) {
#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
        std::exchange(handle_, nullptr).promise()._rethrow_if_exception();
#else
        handle_ = nullptr;
#endif
      }
      return *this;
    }

    void operator++(int) {
      ++*this;
    }

    reference operator*() const {
      return *handle_.promise().current_value;
    }
    pointer operator->() const {
      return handle_.promise().current_value;
    }

    bool operator==(const iterator &other) const noexcept {
      return handle_ == other.handle_;
    }
    auto operator<=>(const iterator &other) const noexcept {
      return *this <=> other;
    }

  private:
    std::coroutine_handle<promise_type> handle_ = nullptr;
  };
  Generator() = default;
  explicit Generator(std::coroutine_handle<promise_type> handle)
      : handle_(handle) {}
  Generator(Generator &&other) noexcept
      : handle_(std::exchange(other.handle_, nullptr)) {}

  Generator &operator=(Generator &&other) noexcept {
    handle_ = std::exchange(other.handle_, nullptr);
    return *this;
  }

  ~Generator() {
    if (handle_)
      handle_.destroy();
  }

  iterator begin() {
    return {handle_};
  }
  static iterator end() noexcept {
    return {};
  }
  /// @note not a shared resource, like a plain `future::get()`,
  /// which cannot be called multiple times
  auto get() {
    precondition(handle_, "Generator::get() called twice")

    defer {
      handle_.destroy();
      handle_ = nullptr;
    };

    while (!handle_.done()) {
      handle_.resume();
    }

#if defined(__cpp_exceptions) || defined(_CPPUNWIND)
    handle_.promise()._rethrow_if_exception();
#endif
    // return the final value(moved, for someone who don't understand: RVO and
    // NRVO only suited for local variables. we didn't return a local
    // variable, nor do we expect the `get` called multiple times)
    return std::move(handle_.promise().final_return_value);
  }

  // next yield value
  auto next() -> std::optional<YieldType> {
    precondition(handle_, "handle has already been destroyed")
    if (!handle_ || handle_.done())
      return std::nullopt;

    handle_.resume();
    return std::make_optional(std::move(*handle_.promise().current_value));
  }

private:
  std::coroutine_handle<promise_type> handle_ = nullptr;
};
} // namespace accat::auxilia
