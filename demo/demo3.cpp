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

