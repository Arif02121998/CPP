/*******************************************************************************
 * 06_threads.cpp — Multithreading in C++11
 *******************************************************************************
 *
 *  ┌─────────────────────────────────────────────────────────────┐
 *  │  THREAD BASICS                                               │
 *  │                                                              │
 *  │  Main Thread ─────┬──── Thread 1 (parallel)                 │
 *  │                   ├──── Thread 2 (parallel)                 │
 *  │                   └──── Thread 3 (parallel)                 │
 *  │                   │                                          │
 *  │                   ▼ join() — wait for all to finish          │
 *  │  Main continues ──                                           │
 *  │                                                              │
 *  │  SYNCHRONIZATION:                                            │
 *  │  mutex         — Lock/unlock (one thread at a time)          │
 *  │  lock_guard    — RAII mutex wrapper                          │
 *  │  atomic        — Lock-free thread-safe variables             │
 *  │  condition_var — Wait/notify between threads                 │
 *  │  async/future  — Async tasks with return values              │
 *  └─────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -pthread -o threads 06_threads.cpp
 ******************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <vector>
#include <chrono>

// ═════════════════════════════════════════════════════════════════
// 1. BASIC THREADS
// ═════════════════════════════════════════════════════════════════

void threadFunction(int id) {
    std::cout << "  Thread " << id << " running on thread "
              << std::this_thread::get_id() << '\n';
}

// ═════════════════════════════════════════════════════════════════
// 2. MUTEX — Mutual Exclusion
// ═════════════════════════════════════════════════════════════════

std::mutex mtx;
int shared_counter = 0;

void incrementSafe(int times) {
    for (int i = 0; i < times; i++) {
        std::lock_guard<std::mutex> lock(mtx);  // RAII lock
        shared_counter++;
        // Lock released automatically when lock goes out of scope
    }
}

// ═════════════════════════════════════════════════════════════════
// 3. ATOMIC — Lock-free thread safety
// ═════════════════════════════════════════════════════════════════

std::atomic<int> atomic_counter{0};

void incrementAtomic(int times) {
    for (int i = 0; i < times; i++) {
        atomic_counter++;  // Thread-safe without mutex!
    }
}

// ═════════════════════════════════════════════════════════════════
// 4. ASYNC / FUTURE — Tasks with return values
// ═════════════════════════════════════════════════════════════════

int computeSum(int n) {
    int sum = 0;
    for (int i = 1; i <= n; i++) sum += i;
    return sum;
}

int main() {
    std::cout << "=== BASIC THREADS ===\n";
    std::cout << "Main thread: " << std::this_thread::get_id() << '\n';
    {
        std::thread t1(threadFunction, 1);
        std::thread t2(threadFunction, 2);
        std::thread t3(threadFunction, 3);

        // Lambda thread
        std::thread t4([]() {
            std::cout << "  Lambda thread running\n";
        });

        t1.join();  // Wait for t1 to finish
        t2.join();
        t3.join();
        t4.join();
    }

    std::cout << "\n=== MUTEX ===\n";
    {
        shared_counter = 0;
        std::thread t1(incrementSafe, 10000);
        std::thread t2(incrementSafe, 10000);
        t1.join();
        t2.join();
        std::cout << "Mutex counter: " << shared_counter << " (expected: 20000)\n";
    }

    std::cout << "\n=== ATOMIC ===\n";
    {
        atomic_counter = 0;
        std::thread t1(incrementAtomic, 10000);
        std::thread t2(incrementAtomic, 10000);
        t1.join();
        t2.join();
        std::cout << "Atomic counter: " << atomic_counter << " (expected: 20000)\n";
    }

    std::cout << "\n=== ASYNC / FUTURE ===\n";
    {
        // Launch async task
        auto future1 = std::async(std::launch::async, computeSum, 1000);
        auto future2 = std::async(std::launch::async, computeSum, 2000);

        // Do other work here while tasks run...
        std::cout << "Waiting for results...\n";

        // Get results (blocks if not ready)
        int result1 = future1.get();
        int result2 = future2.get();

        std::cout << "Sum(1..1000) = " << result1 << '\n';
        std::cout << "Sum(1..2000) = " << result2 << '\n';
    }

    std::cout << "\n=== HARDWARE CONCURRENCY ===\n";
    std::cout << "CPU threads available: "
              << std::thread::hardware_concurrency() << '\n';

    return 0;
}
