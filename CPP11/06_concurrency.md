# Chapter 6 — Concurrency & the Memory Model (C++11)

> *Level: Advanced*
> Back to [Table of Contents](README.md)

Before C++11, the language had **no notion of threads at all**. C++11 introduced
a complete, portable concurrency library *and* a formal **memory model** — one of
its most significant achievements.

---

## 6.1 The Problem Before C++11

```cpp
// C++03: no standard threads — you used platform-specific APIs
#ifdef _WIN32
    CreateThread(...);     // Win32
#else
    pthread_create(...);   // POSIX
#endif
```

There was also **no memory model**: the standard didn't define how threads
interacted through memory, so multithreaded C++ was technically *undefined* and
relied entirely on compiler/platform guarantees.

---

## 6.2 `std::thread` — Standard Threads

### What It Is

A portable thread of execution. You give it a callable; it runs concurrently.

```cpp
#include <thread>

void work(int id) { std::cout << "thread " << id << '\n'; }

std::thread t1(work, 1);                 // start a thread running work(1)
std::thread t2([]{ std::cout << "lambda thread\n"; });

t1.join();   // wait for t1 to finish
t2.join();   // wait for t2 to finish
```

### `join` vs `detach` — Critical Rule

Every `std::thread` **must** be either `join`ed (wait for it) or `detach`ed (let
it run independently) **before it is destroyed**. Otherwise the destructor calls
`std::terminate()`.

```cpp
std::thread t(work, 1);
// ... if t goes out of scope here without join/detach → std::terminate()!
t.join();           // correct
```

### Pitfalls

- A thread is **not copyable** (only movable) — it represents a unique resource.
- Passing arguments **copies** them by default; use `std::ref` to pass by
  reference deliberately.
- Reference-capturing lambdas in threads can dangle if the launching scope exits.

---

## 6.3 Mutexes & Locks — Protecting Shared Data

### The Data Race Problem

```cpp
int counter = 0;
// Two threads doing counter++ concurrently → DATA RACE (undefined behavior)
// counter++ is read-modify-write, not atomic
```

### `std::mutex` + RAII Locks

```cpp
#include <mutex>

std::mutex m;
int counter = 0;

void increment() {
    std::lock_guard<std::mutex> lock(m);  // RAII: locks now, unlocks at scope end
    ++counter;                            // protected critical section
}   // mutex automatically released — even if an exception is thrown
```

### `lock_guard` vs `unique_lock`

| Lock | Flexibility | Use For |
|------|-------------|---------|
| `std::lock_guard` | Minimal (lock in ctor, unlock in dtor) | Simple scoped locking |
| `std::unique_lock` | Can unlock/relock, defer, transfer, timed | Condition variables, complex scopes |

```cpp
std::unique_lock<std::mutex> lk(m);   // more flexible
lk.unlock();                          // can release early
// ... do non-critical work ...
lk.lock();                            // and re-acquire
```

### Avoiding Deadlock — `std::lock`

Locking multiple mutexes in different orders causes **deadlock**. `std::lock`
acquires several mutexes atomically without deadlock:

```cpp
std::mutex m1, m2;
std::lock(m1, m2);                                  // no deadlock
std::lock_guard<std::mutex> g1(m1, std::adopt_lock);
std::lock_guard<std::mutex> g2(m2, std::adopt_lock);
```

---

## 6.4 `std::atomic` — Lock-Free Operations

### What It Is

A template providing **atomic** (indivisible, race-free) operations on a value,
often without a mutex.

```cpp
#include <atomic>

std::atomic<int> counter{0};

void increment() {
    ++counter;                 // atomic — safe from multiple threads, no mutex
    counter.fetch_add(1);      // explicit atomic add
}

std::atomic<bool> ready{false};
ready.store(true);             // atomic write
bool r = ready.load();         // atomic read
```

### Why It Matters

- **Faster than a mutex** for simple counters/flags (no blocking).
- The building block for lock-free data structures.
- `std::atomic_flag` is the only guaranteed always-lock-free type.

---

## 6.5 Futures, Promises & `std::async`

### What It Is — Getting a Result From a Thread

`std::thread` can't easily return a value. Futures provide an **asynchronous
result channel**.

```cpp
#include <future>

// std::async: run a task, get a future for its result
std::future<int> fut = std::async(std::launch::async, []{
    return expensive_computation();
});

// ... do other work concurrently ...

int result = fut.get();    // blocks until the result is ready, then returns it
```

### `promise`/`future` — Manual Hand-off

```cpp
std::promise<int> prom;
std::future<int> fut = prom.get_future();

std::thread producer([&prom]{
    prom.set_value(42);            // fulfill the promise
});

int value = fut.get();             // consumer receives 42
producer.join();
```

### Launch Policies

```cpp
std::async(std::launch::async, f);     // run on a new thread, now
std::async(std::launch::deferred, f);  // run lazily on the calling thread at .get()
std::async(f);                         // implementation chooses (either)
```

> **Pitfall:** the `std::future` returned by `std::async` **blocks in its
> destructor** until the task completes. An unnamed `std::async(...)` temporary
> therefore runs synchronously — a common surprise.

---

## 6.6 `condition_variable` — Waiting for a Condition

Lets threads **wait efficiently** until notified, instead of busy-looping.

```cpp
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
bool ready = false;

// Consumer
void consumer() {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{ return ready; });   // sleeps until ready == true
    // ... proceed ...
}

// Producer
void producer() {
    { std::lock_guard<std::mutex> lk(m); ready = true; }
    cv.notify_one();                    // wake a waiting thread
}
```

> Always use the **predicate form** `cv.wait(lk, pred)` to guard against
> **spurious wakeups** (a thread can wake without a notification).

---

## 6.7 `thread_local` Storage

Each thread gets its **own independent copy** of a `thread_local` variable.

```cpp
thread_local int tls_counter = 0;   // separate per thread

void f() { ++tls_counter; }         // no synchronization needed — each thread's own
```

Useful for per-thread caches, RNG state, error contexts, etc.

---

## 6.8 The C++11 Memory Model (Conceptual)

C++11 defined, for the first time, **how memory operations behave across
threads**:

- A program with a **data race** (two unsynchronized accesses, at least one a
  write) has **undefined behavior**.
- **Atomics and mutexes establish "happens-before" ordering**, making writes by
  one thread visible to another.
- `std::atomic` supports **memory orderings** (`memory_order_seq_cst` default,
  plus `acquire`/`release`/`relaxed`) to trade safety for performance in
  lock-free code.

```cpp
std::atomic<int> x{0};
x.store(1, std::memory_order_release);    // advanced: relax ordering for speed
int v = x.load(std::memory_order_acquire);
```

> For most code, stick with the **default sequential consistency**; relaxed
> orderings are an expert-only optimization.

---

## C++03 vs C++11 — Concurrency Comparison

| Capability | C++03 | C++11 |
|-----------|:-----:|:-----:|
| Standard threads | ❌ (platform APIs) | ✅ `std::thread` |
| Mutex / RAII locks | ❌ | ✅ `mutex`, `lock_guard`, `unique_lock` |
| Atomics | ❌ | ✅ `std::atomic` |
| Futures / async | ❌ | ✅ `future`, `promise`, `async` |
| Condition variables | ❌ | ✅ |
| `thread_local` | ❌ | ✅ |
| Defined memory model | ❌ | ✅ |

---

## Chapter 6 Cheat Sheet

```cpp
std::thread t(func, arg);  t.join();        // always join or detach!

std::mutex m;
{ std::lock_guard<std::mutex> lk(m); /*critical*/ } // RAII lock

std::atomic<int> n{0};  ++n;                 // lock-free counter

auto fut = std::async(std::launch::async, work);
auto r = fut.get();                          // get async result

std::condition_variable cv;
cv.wait(lk, []{ return ready; });            // predicate form (spurious-wakeup safe)
cv.notify_one();

thread_local int perThread = 0;              // per-thread storage
```

**Remember:**
- Every `std::thread` must be `join`ed or `detach`ed before destruction.
- Use `lock_guard` for simple scopes; `unique_lock` with condition variables.
- Prefer `std::atomic` for simple counters/flags; mutexes for compound state.
- Always use the **predicate form** of `cv.wait`.
- A data race = **undefined behavior**; synchronize with atomics/mutexes.

---

## Common Mistakes (Concurrency)

1. Destroying a `std::thread` without `join`/`detach` → `std::terminate`.
2. Forgetting that `std::async`'s future **blocks in its destructor**.
3. `cv.wait` without a predicate → missed/spurious wakeups.
4. Locking multiple mutexes in inconsistent order → deadlock (use `std::lock`).
5. Sharing a reference-capturing lambda across threads that outlive the scope.

## When NOT to Use

- **Raw `std::thread`** when `std::async`/a task model expresses intent better.
- **Relaxed memory orderings** unless you are an expert with benchmarks — the
  default sequential consistency is correct and clear.
- **Atomics** for compound invariants spanning multiple variables — use a mutex.

---

*Next: [Chapter 7 — STL & Library Additions](07_stl_and_library.md)*
