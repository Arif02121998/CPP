/*******************************************************************************
 * 04_multithreading_patterns.cpp — Thread pools, producer-consumer, atomics
 *******************************************************************************
 *
 *  C++ Threading Building Blocks:
 *
 *   ┌─────────────────┬────────────────────────────────────────────┐
 *   │  Primitive       │  Purpose                                   │
 *   ├─────────────────┼────────────────────────────────────────────┤
 *   │  std::thread     │  OS thread wrapper                         │
 *   │  std::mutex      │  Mutual exclusion lock                     │
 *   │  std::lock_guard │  RAII mutex locking                        │
 *   │  std::unique_lock│  Flexible locking (for condition_variable) │
 *   │  std::cond. var  │  Wait/notify signaling                     │
 *   │  std::atomic     │  Lock-free shared variables                │
 *   │  std::future     │  Get result from async operation           │
 *   │  std::jthread    │  Auto-joining thread (C++20)               │
 *   └─────────────────┴────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -pthread -o threading 04_multithreading_patterns.cpp
 ******************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <queue>
#include <functional>
#include <vector>
#include <chrono>
#include <numeric>

using namespace std::chrono_literals;

// ═══════════════════════════════════════════════════════════════
//  1. Thread-safe queue (Producer-Consumer pattern)
// ═══════════════════════════════════════════════════════════════

/*
 *   Producer ──► Queue ──► Consumer
 *
 *   ┌─────────────────────────────────────────┐
 *   │  mutex protects the queue               │
 *   │  condition_variable signals:             │
 *   │    - producer → "data available"         │
 *   │    - consumer → waits for data           │
 *   └─────────────────────────────────────────┘
 */

template <typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool done_ = false;

public:
    void push(T value) {
        {
            std::lock_guard lock(mutex_);
            queue_.push(std::move(value));
        }
        cv_.notify_one();
    }

    // Returns false if queue is done and empty
    bool pop(T& value) {
        std::unique_lock lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty() || done_; });
        if (queue_.empty()) return false;
        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    void setDone() {
        {
            std::lock_guard lock(mutex_);
            done_ = true;
        }
        cv_.notify_all();
    }

    bool empty() const {
        std::lock_guard lock(mutex_);
        return queue_.empty();
    }
};

void demo_producer_consumer() {
    std::cout << "═══ PRODUCER-CONSUMER ═══\n";

    ThreadSafeQueue<int> queue;
    std::atomic<int> consumed{0};

    // Producer
    auto producer = std::jthread([&queue] {
        for (int i = 1; i <= 10; ++i) {
            queue.push(i);
            std::this_thread::sleep_for(10ms);
        }
        queue.setDone();
    });

    // Consumer
    auto consumer = std::jthread([&queue, &consumed] {
        int value;
        while (queue.pop(value)) {
            consumed += value;
        }
    });

    producer.join();
    consumer.join();

    std::cout << "  Sum of consumed values: " << consumed.load() << '\n';
    std::cout << "  Expected: 55\n";
}


// ═══════════════════════════════════════════════════════════════
//  2. Simple thread pool
// ═══════════════════════════════════════════════════════════════

/*
 *   ┌────────────────────────────────────────────────────────────┐
 *   │  Thread Pool                                               │
 *   │  ┌──────────┐ ┌──────────┐ ┌──────────┐                  │
 *   │  │ Worker 1  │ │ Worker 2  │ │ Worker 3  │  ← threads     │
 *   │  └─────┬────┘ └─────┬────┘ └─────┬────┘                  │
 *   │        │             │             │                       │
 *   │  ┌─────▼─────────────▼─────────────▼────┐                 │
 *   │  │        Task Queue                     │  ← shared      │
 *   │  │  [task1] [task2] [task3] [task4]      │                 │
 *   │  └──────────────────────────────────────┘                  │
 *   └────────────────────────────────────────────────────────────┘
 */

class ThreadPool {
    std::vector<std::jthread> workers_;
    ThreadSafeQueue<std::function<void()>> tasks_;

public:
    explicit ThreadPool(size_t numThreads) {
        for (size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] {
                std::function<void()> task;
                while (tasks_.pop(task)) {
                    task();
                }
            });
        }
    }

    void submit(std::function<void()> task) {
        tasks_.push(std::move(task));
    }

    ~ThreadPool() {
        tasks_.setDone();
        // jthreads auto-join
    }
};

void demo_thread_pool() {
    std::cout << "\n═══ THREAD POOL ═══\n";

    std::mutex printMtx;
    std::atomic<int> tasksDone{0};

    {
        ThreadPool pool(4);

        for (int i = 0; i < 8; ++i) {
            pool.submit([i, &printMtx, &tasksDone] {
                std::this_thread::sleep_for(50ms);
                {
                    std::lock_guard lock(printMtx);
                    std::cout << "  Task " << i << " done on thread "
                              << std::this_thread::get_id() << '\n';
                }
                ++tasksDone;
            });
        }
    }  // Pool destroyed here, all tasks complete

    std::cout << "  Total tasks completed: " << tasksDone.load() << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Atomics — lock-free shared variables
// ═══════════════════════════════════════════════════════════════

void demo_atomics() {
    std::cout << "\n═══ ATOMICS ═══\n";

    std::atomic<int> counter{0};

    // Multiple threads incrementing atomically
    std::vector<std::jthread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&counter] {
            for (int j = 0; j < 1000; ++j) {
                counter.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    threads.clear();  // join all

    std::cout << "  Counter: " << counter.load() << " (expected: 10000)\n";

    // Compare-and-swap (CAS) — fundamental lock-free operation
    std::atomic<int> value{10};
    int expected = 10;
    bool success = value.compare_exchange_strong(expected, 20);
    std::cout << "  CAS(10→20): success=" << success << ", value=" << value.load() << '\n';

    // Atomic flag — simplest atomic type (always lock-free)
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    bool was_set = flag.test_and_set();
    std::cout << "  test_and_set: was_set=" << was_set << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. std::async & std::future — high-level async
// ═══════════════════════════════════════════════════════════════

/*
 *  async() launches a function and returns a future.
 *  future.get() blocks until the result is ready.
 *
 *   auto future = std::async(std::launch::async, compute);
 *   // ... do other work ...
 *   auto result = future.get();  // blocks if not ready
 */

long long computeSum(long long start, long long end) {
    long long sum = 0;
    for (long long i = start; i <= end; ++i) sum += i;
    return sum;
}

void demo_async() {
    std::cout << "\n═══ ASYNC & FUTURE ═══\n";

    auto start = std::chrono::high_resolution_clock::now();

    // Launch parallel computations
    auto f1 = std::async(std::launch::async, computeSum, 1LL, 25000000LL);
    auto f2 = std::async(std::launch::async, computeSum, 25000001LL, 50000000LL);
    auto f3 = std::async(std::launch::async, computeSum, 50000001LL, 75000000LL);
    auto f4 = std::async(std::launch::async, computeSum, 75000001LL, 100000000LL);

    // Collect results
    long long total = f1.get() + f2.get() + f3.get() + f4.get();

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "  Sum 1..100M = " << total << '\n';
    std::cout << "  Time: " << ms << " ms (4 parallel tasks)\n";
}


// ═══════════════════════════════════════════════════════════════
//  5. C++20 jthread — auto-joining, stoppable
// ═══════════════════════════════════════════════════════════════

void demo_jthread() {
    std::cout << "\n═══ C++20 jthread ═══\n";

    // jthread: automatically joins on destruction (no more forgetting join!)
    // Also supports cooperative cancellation via stop_token

    {
        std::jthread worker([](std::stop_token stoken) {
            int count = 0;
            while (!stoken.stop_requested()) {
                ++count;
                std::this_thread::sleep_for(50ms);
            }
            std::cout << "  Worker stopped after " << count << " iterations\n";
        });

        std::this_thread::sleep_for(200ms);
        worker.request_stop();  // Cooperative cancellation!
        // Automatically joins when worker goes out of scope
    }

    std::cout << "  jthread auto-joined\n";
}


int main() {
    demo_producer_consumer();
    demo_thread_pool();
    demo_atomics();
    demo_async();
    demo_jthread();
    return 0;
}
