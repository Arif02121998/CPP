/*******************************************************************************
 * 03_coroutines.cpp — Coroutines: co_await, co_yield, generators
 *******************************************************************************
 *
 *  A coroutine is a function that can SUSPEND and RESUME:
 *
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  Normal function:          Coroutine:                        │
 *   │                                                              │
 *   │  func() {                  coroutine() {                     │
 *   │    // runs fully           // start                          │
 *   │    // returns once         co_yield 1;  ← suspend, emit 1   │
 *   │  }                         // resume here later              │
 *   │                            co_yield 2;  ← suspend, emit 2   │
 *   │                            // resume here later              │
 *   │                            co_return;   ← done               │
 *   │                          }                                   │
 *   └──────────────────────────────────────────────────────────────┘
 *
 *  Three keywords make a function a coroutine:
 *    co_await  — suspend until an operation completes
 *    co_yield  — suspend and produce a value (generator pattern)
 *    co_return — complete the coroutine
 *
 *  NOTE: C++20 coroutines provide the MECHANISM, not ready-made types.
 *        You need to define promise_type, etc. Libraries like cppcoro
 *        provide ready-to-use types. Below we build a simple Generator.
 *
 * COMPILE: g++ -std=c++20 -o coroutines 03_coroutines.cpp
 *          (GCC 11+, Clang 14+, MSVC 19.28+)
 ******************************************************************************/

#include <iostream>
#include <coroutine>
#include <optional>
#include <cstdint>

// ═══════════════════════════════════════════════════════════════
//  1. Building a simple Generator<T>
// ═══════════════════════════════════════════════════════════════

/*
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │  Generator coroutine frame:                                   │
 *  │  ┌───────────────┐                                           │
 *  │  │ promise_type  │ ← controls behavior at suspend points     │
 *  │  │ - value_      │ ← stores co_yield-ed value                │
 *  │  ├───────────────┤                                           │
 *  │  │ coroutine     │                                           │
 *  │  │ state/locals  │ ← preserved across suspensions            │
 *  │  ├───────────────┤                                           │
 *  │  │ resume point  │ ← where to continue on next()             │
 *  │  └───────────────┘                                           │
 *  └──────────────────────────────────────────────────────────────┘
 */

template <typename T>
class Generator {
public:
    // === The promise_type is REQUIRED by the coroutine machinery ===
    struct promise_type {
        std::optional<T> current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() { return {}; }  // Don't start until asked
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    // === Generator interface ===
    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(Handle h) : handle_(h) {}
    ~Generator() { if (handle_) handle_.destroy(); }

    // Move only
    Generator(Generator&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    // Get next value
    bool next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        return !handle_.done();
    }

    T value() const { return *handle_.promise().current_value; }

    // Range-based for support
    struct Iterator {
        Handle handle;
        bool operator!=(std::default_sentinel_t) const { return handle && !handle.done(); }
        Iterator& operator++() { handle.resume(); return *this; }
        T operator*() const { return *handle.promise().current_value; }
    };

    Iterator begin() {
        if (handle_) handle_.resume();
        return Iterator{handle_};
    }
    std::default_sentinel_t end() { return {}; }

private:
    Handle handle_;
};


// ═══════════════════════════════════════════════════════════════
//  2. Using co_yield — Generator pattern
// ═══════════════════════════════════════════════════════════════

// Generate counting numbers
Generator<int> counter(int start, int end) {
    for (int i = start; i <= end; ++i) {
        co_yield i;  // Suspend and produce i
    }
}

// Infinite Fibonacci generator
Generator<uint64_t> fibonacci() {
    uint64_t a = 0, b = 1;
    while (true) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

// Generate even numbers lazily
Generator<int> evens(int limit) {
    for (int i = 0; i <= limit; i += 2) {
        co_yield i;
    }
}

void demo_generators() {
    std::cout << "═══ GENERATORS (co_yield) ═══\n";

    // Simple counter
    std::cout << "counter(1,5): ";
    for (auto val : counter(1, 5)) {
        std::cout << val << " ";
    }
    std::cout << '\n';

    // Fibonacci — take first 15
    std::cout << "fibonacci (first 15): ";
    auto fib = fibonacci();
    for (int i = 0; i < 15 && fib.next(); ++i) {
        std::cout << fib.value() << " ";
    }
    std::cout << '\n';

    // Even numbers
    std::cout << "evens(10): ";
    for (auto val : evens(10)) {
        std::cout << val << " ";
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Practical: Range generator
// ═══════════════════════════════════════════════════════════════

Generator<int> range(int start, int end, int step = 1) {
    if (step > 0) {
        for (int i = start; i < end; i += step) co_yield i;
    } else {
        for (int i = start; i > end; i += step) co_yield i;
    }
}

Generator<std::pair<int, int>> enumerate(Generator<int> gen) {
    int idx = 0;
    for (auto val : gen) {
        co_yield {idx++, val};
    }
}

void demo_practical() {
    std::cout << "\n═══ PRACTICAL GENERATORS ═══\n";

    // Range with step
    std::cout << "range(0, 20, 3): ";
    for (auto val : range(0, 20, 3)) {
        std::cout << val << " ";
    }
    std::cout << '\n';

    // Countdown
    std::cout << "range(10, 0, -2): ";
    for (auto val : range(10, 0, -2)) {
        std::cout << val << " ";
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. Summary diagram
// ═══════════════════════════════════════════════════════════════

/*
 *  ┌──────────────────────────────────────────────────────────┐
 *  │             Coroutine Execution Flow                      │
 *  │                                                          │
 *  │  Caller            Coroutine                             │
 *  │    │                  │                                   │
 *  │    │── create ──────►│                                   │
 *  │    │                  │ (initial_suspend)                 │
 *  │    │◄── handle ──────│                                   │
 *  │    │                  │                                   │
 *  │    │── resume() ────►│                                   │
 *  │    │                  │── runs until co_yield ──►│        │
 *  │    │◄── suspended ───│                                   │
 *  │    │── value() ─────►│                                   │
 *  │    │◄── 42 ──────────│                                   │
 *  │    │                  │                                   │
 *  │    │── resume() ────►│                                   │
 *  │    │                  │── runs until co_yield ──►│        │
 *  │    │◄── suspended ───│                                   │
 *  │    │                  │                                   │
 *  │    │── resume() ────►│                                   │
 *  │    │                  │── co_return (done) ──────►│       │
 *  │    │◄── done ────────│                                   │
 *  │    │── destroy() ───►│ (frame freed)                     │
 *  └──────────────────────────────────────────────────────────┘
 */


int main() {
    demo_generators();
    demo_practical();

    std::cout << "\n═══ SUMMARY ═══\n";
    std::cout << "co_yield  — suspend + produce a value (generator)\n";
    std::cout << "co_await  — suspend until async operation completes\n";
    std::cout << "co_return — finish the coroutine\n";
    std::cout << "NOTE: For production use, consider cppcoro or C++23 std::generator\n";

    return 0;
}
