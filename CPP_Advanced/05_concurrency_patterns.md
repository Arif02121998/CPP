# Chapter 5 — Advanced Concurrency Patterns

> *Level: Advanced*
> Back to [Table of Contents](README.md)
> Builds on [C++11 concurrency](../CPP11/06_concurrency.md) (threads, mutexes,
> atomics, futures). This chapter covers the **patterns** built on those tools.

Knowing the primitives isn't enough — real systems need correct, reusable
concurrency *patterns*. This chapter shows the most important ones.

---

## 5.1 Producer/Consumer & Thread-Safe Queue

### The Problem

One or more threads produce work; others consume it. You need a queue that's
safe to push/pop concurrently, and consumers should **sleep** when it's empty
(not busy-wait).

### Thread-Safe Queue

```cpp
template <typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
public:
    void push(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value));
        }
        cv_.notify_one();                 // wake one waiting consumer
    }

    // Blocks until an item is available
    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]{ return !queue_.empty(); }); // predicate = spurious-safe
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

    bool try_pop(T& out) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) return false;
        out = std::move(queue_.front());
        queue_.pop();
        return true;
    }
};
```

**Key points:**
- `notify_one` is called **after** releasing the lock (scope block) to avoid the
  woken thread immediately blocking on the still-held mutex.
- The **predicate form** of `cv_.wait` handles spurious wakeups.

---

## 5.2 Thread Pool

### The Problem

Creating a `std::thread` per task is expensive and unbounded. A **thread pool**
reuses a fixed set of worker threads pulling tasks from a queue.

### Minimal Thread Pool

```cpp
class ThreadPool {
    std::vector<std::thread> workers_;
    ThreadSafeQueue<std::function<void()>> tasks_;   // from 5.1 (+ shutdown flag)
    std::atomic<bool> done_{false};
public:
    explicit ThreadPool(std::size_t n) {
        for (std::size_t i = 0; i < n; ++i)
            workers_.emplace_back([this]{ worker_loop(); });
    }
    ~ThreadPool() {
        done_ = true;
        // (real impl: push sentinel tasks / notify to wake workers)
        for (auto& t : workers_) if (t.joinable()) t.join();
    }

    // Submit a task and get a future for its result
    template <typename F>
    auto submit(F f) -> std::future<decltype(f())> {
        auto task = std::make_shared<std::packaged_task<decltype(f())()>>(std::move(f));
        std::future<decltype(f())> fut = task->get_future();
        tasks_.push([task]{ (*task)(); });   // type-erase into void()
        return fut;
    }
private:
    void worker_loop() {
        while (!done_) {
            std::function<void()> task;
            if (tasks_.try_pop(task)) task();
            else std::this_thread::yield();
        }
    }
};

// Usage
ThreadPool pool(4);
auto fut = pool.submit([]{ return 6 * 7; });
int answer = fut.get();    // 42
```

**`std::packaged_task`** wraps a callable so its result is delivered through a
`std::future` — the bridge between "run somewhere" and "get the result here."

> Production pools should wake workers via a condition variable rather than
> `yield()`-spinning, and drain the queue cleanly on shutdown.

---

## 5.3 Futures, `async`, and Continuations

### Composing Asynchronous Work

```cpp
// Fan-out: launch independent tasks, then combine results
auto f1 = std::async(std::launch::async, []{ return fetchA(); });
auto f2 = std::async(std::launch::async, []{ return fetchB(); });
auto combined = f1.get() + f2.get();      // waits for both
```

### The `std::async` Destructor Trap (Recap)

The future returned by `std::async(launch::async, ...)` **blocks in its
destructor** until the task finishes. So:

```cpp
std::async(std::launch::async, longTask);   // TEMPORARY future → runs synchronously!
auto f = std::async(std::launch::async, longTask);  // store it → truly async
```

> C++11/14/17 have no built-in `.then()` continuations. Chaining requires manual
> wiring (or libraries). C++20 coroutines + `std::future` extensions and
> executors address this.

---

## 5.4 Lock-Free Programming & Atomics

### What It Is

Coordinating threads **without mutexes**, using `std::atomic` operations that the
hardware guarantees are indivisible. Goal: avoid blocking/contention.

### Atomic Counter (Simple, Always Correct)

```cpp
std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);  // relaxed: just need atomicity
```

### Compare-And-Swap (CAS) — The Heart of Lock-Free

```cpp
std::atomic<int> value{0};

// Atomically: if value == expected, set it to desired; else reload expected
int expected = value.load();
while (!value.compare_exchange_weak(expected, expected + 1)) {
    // expected was updated with the current value; loop and retry
}
```

CAS loops are how lock-free stacks, queues, and counters are built.

### Memory Orderings (Expert Territory)

| Ordering | Meaning | Use |
|----------|---------|-----|
| `seq_cst` (default) | Total global order | Safe default; easiest to reason about |
| `acquire`/`release` | Pairwise synchronization | Producer/consumer handoff |
| `relaxed` | Atomicity only, no ordering | Independent counters/stats |

```cpp
// Release/acquire handshake: publish data, then a flag
data = compute();
ready.store(true, std::memory_order_release);   // publisher
// ---
if (ready.load(std::memory_order_acquire))      // consumer sees 'data' too
    use(data);
```

> **Warning:** lock-free code is **extremely hard to get right** (ABA problem,
> memory ordering, reclamation). Use proven libraries; hand-roll only with
> rigorous testing and benchmarks. Default to mutexes until profiling demands
> otherwise.

---

## 5.5 Synchronization Patterns & Hazards

### Common Hazards

| Hazard | Cause | Mitigation |
|--------|-------|------------|
| **Data race** | Unsynchronized shared access (≥1 write) | mutex / atomic |
| **Deadlock** | Locks acquired in different orders | Lock ordering / `std::lock` / `scoped_lock` |
| **Livelock** | Threads keep retrying, no progress | Backoff, fairness |
| **Priority inversion** | Low-prio thread holds a lock a high-prio needs | Priority inheritance |
| **False sharing** | Independent vars share a cache line | Pad/align to cache line |

### Deadlock Avoidance

```cpp
// C++17 scoped_lock locks multiple mutexes atomically (no deadlock)
std::scoped_lock lock(mutexA, mutexB);   // preferred over std::lock + adopt
```

### `std::call_once` — Run Exactly Once

```cpp
std::once_flag flag;
void init() { std::call_once(flag, []{ /* one-time setup */ }); }  // thread-safe
```

### Reader/Writer Lock (`std::shared_mutex`, C++17)

When reads vastly outnumber writes, allow **concurrent readers**, exclusive
writers.

```cpp
std::shared_mutex rw;

int read()  { std::shared_lock lk(rw);  return value; }   // many readers
void write(int v){ std::unique_lock lk(rw); value = v; }  // one writer
```

### False Sharing Fix

```cpp
struct alignas(64) PaddedCounter {   // align to a cache line (typically 64 bytes)
    std::atomic<int> count{0};
};                                   // avoids two counters sharing a cache line
```

---

## Chapter 5 Cheat Sheet

```cpp
// Thread-safe queue: mutex + condition_variable (predicate wait)
cv_.wait(lock, [this]{ return !queue_.empty(); });

// Thread pool: workers pull from a task queue; packaged_task → future
auto fut = pool.submit([]{ return work(); });

// async: STORE the future or it runs synchronously
auto f = std::async(std::launch::async, task);

// Lock-free: CAS loop
while (!a.compare_exchange_weak(expected, desired)) {}

// Deadlock-free multi-lock
std::scoped_lock lk(m1, m2);

// One-time init / reader-writer / false-sharing
std::call_once(flag, init);
std::shared_lock lk(rw);              // shared (read) lock
struct alignas(64) Padded { std::atomic<int> c; };
```

**Remember:**
- Producer/consumer = thread-safe queue + condition variable (predicate wait).
- Thread pools reuse threads; `packaged_task` bridges work → `future`.
- **Store** the `std::async` future or it blocks synchronously.
- Prefer mutexes; reach for lock-free only with proof and tests.
- `std::scoped_lock` (multi-lock), `call_once`, and `shared_mutex` solve common
  synchronization needs cleanly.

---

## Common Mistakes

1. `notify` while holding the lock unnecessarily, or `wait` without a predicate.
2. Letting a `std::async` future be a temporary (synchronous surprise).
3. Inconsistent lock ordering → deadlock (use `scoped_lock`).
4. Hand-rolling lock-free structures (ABA / ordering bugs).
5. False sharing silently destroying scalability (pad hot atomics).

## When NOT to Use

- **Lock-free** unless profiling proves mutex contention is the bottleneck.
- **A thread pool** for a handful of one-off tasks — `std::async` may suffice.
- **`shared_mutex`** when writes are frequent — its overhead can exceed a plain
  mutex.

---

*Next: [Chapter 6 — Move & Forwarding, In Depth](06_move_and_forwarding_deep.md)*
