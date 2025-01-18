#include <corecrt_terminate.h>
#include <coroutine>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>
#include "accat/auxilia/details/macros.hpp"

template <typename Ty, typename Alloc = std::allocator<char>> class Generator {
  static_assert(std::is_nothrow_move_constructible_v<Ty>,
                "Ty must be nothrow move constructible");
  static_assert(!std::is_same_v<Ty, void>, "Ty must not be void");

public:
  struct promise_type {
    union {
      const Ty *current_value = nullptr;
      Ty final_return_value;
    };
#ifdef _CPPUNWIND
    std::exception_ptr exception;
#endif

    Generator get_return_object() {
      return Generator{
          std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_always initial_suspend() noexcept {
      return {};
    }
    std::suspend_always final_suspend() noexcept {
      return {};
    }

    std::suspend_always yield_value(const Ty &value) noexcept {
      current_value = std::addressof(value);
      return {};
    }

    // void return_void() noexcept {}
    void return_value(const Ty &value) noexcept {
      final_return_value = value;
    }

#ifdef _CPPUNWIND
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

    using CharAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<char>;
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

  struct iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = Ty;
    using difference_type = std::ptrdiff_t;
    using pointer = const Ty *;
    using reference = const Ty &;

    iterator() = default;
    iterator(std::coroutine_handle<promise_type> handle)
        : handle_(handle) {
      if (handle_) {
        handle_.resume();
        if (handle_.done()) {
#ifdef _CPPUNWIND
          handle_.promise()._rethrow_if_exception();
#endif
          handle_ = nullptr;
        }
      }
    }

    iterator &operator++() {
      handle_.resume();
      if (handle_.done()) {
#ifdef _CPPUNWIND
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

    if (handle_) {

      defer {
        handle_.destroy();
        handle_ = nullptr;
      };

      while (!handle_.done()) {
        handle_.resume();
      }

#ifdef _CPPUNWIND
      handle_.promise()._rethrow_if_exception();
#endif

      return handle_.promise().final_return_value;
    }

    // return Ty{};
    if constexpr (std::is_default_constructible_v<Ty>) {
      return Ty{};
    } else {
      dbg_break
      std::terminate();
    }
  }

private:
  std::coroutine_handle<promise_type> handle_ = nullptr;
};

Generator<int> fibonacci() {
  int a = 0, b = 1;
  while (a < 100) {
    co_yield a;
    int next = a + b;
    a = b;
    b = next;
  }
  // the a here is the first number that is greater than 100
  co_return a;
}


// int main() {
//   auto fibTask = fibonacci();
//   std::cout << "Fibonacci sequence:\n";
//   auto myRange = fibTask | std::views::take(10);
//   for (auto i : myRange) {
//     std::cout << i << " ";
//   }
//   std::cout << "\ntypeof myRange: " << typeid(myRange).name() << "\n"
//             << typeid(myRange).hash_code() << '\n';
//   std::cout << '\n';

//   auto g = fibTask.get(); // shall not call it before the coroutine is done
//   std::cout << "fibTask.get(): " << g << '\n';

//   return 0;
// }

#include <coroutine>
#include <iostream>
#include <string>

class MyCoro {
public:
    struct promise_type {
        std::string currentValue;
        std::suspend_always yield_value(const std::string& value) {
            currentValue = value;
            return {};
        }
        MyCoro get_return_object() {
            return MyCoro(std::coroutine_handle<promise_type>::from_promise(*this));
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        void return_void() {}
    };

    MyCoro(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~MyCoro() { if (coro) coro.destroy(); }
    bool move_next() {
        if (!coro.done()) coro.resume();
        return !coro.done();
    }
    std::string current_value() const {
        return coro.promise().currentValue;
    }
private:
    std::coroutine_handle<promise_type> coro;
};

MyCoro handleUserInput() {
    std::string line;
    while (true) {
        if (!std::getline(std::cin, line)) {
            co_return;
        }
        co_yield line;
    }
}

// int main() {
//     MyCoro coro = handleUserInput();
//     while (coro.move_next()) {
//         std::cout << "You entered: " << coro.current_value() << std::endl;
//     }
//     return 0;
// }

#include <iostream>
#include <coroutine>
#include <optional>

struct DataFeeder {
    std::optional<int> data;

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        // Simulate waiting for data
        std::cout << "Coroutine suspended, waiting for data...\n";
        // After data is available, resume the coroutine
        if (data) {
            handle.resume();
        }
    }

    int await_resume() {
        std::cout << "Data received: " << *data << "\n";
        return *data;
    }
};

struct MyCoroutine {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        MyCoroutine get_return_object() { return MyCoroutine{handle_type::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {}
    };

    handle_type h;

    MyCoroutine(handle_type h) : h(h) {}
    ~MyCoroutine() { if (h) h.destroy(); }

    void run() {
        h.resume();
    }
};

MyCoroutine my_coroutine(DataFeeder& feeder) {
    int result = co_await feeder;
    std::cout << "Coroutine finished, result: " << result << "\n";
}

int main() {
    DataFeeder feeder;
    auto coro = my_coroutine(feeder);

    // Simulating the feeding of data
    feeder.data = 42; // Data is now available
    coro.run(); // This will resume the coroutine

    return 0;
}

