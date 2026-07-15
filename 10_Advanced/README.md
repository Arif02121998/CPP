<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 1 0</samp>
</p>

<h1 align="center">📘 Advanced C++ — Expert Patterns & Techniques</h1>

<p align="center"><em>"This is where intermediate ends and expert begins."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Exception Handling & Safety | [§1](#-1-exception-handling--safety) |
| 2 | Design Patterns | [§2](#-2-design-patterns) |
| 3 | Type Traits & Metaprogramming | [§3](#-3-type-traits--metaprogramming) |
| 4 | Multithreading Patterns | [§4](#-4-multithreading-patterns) |
| 5 | Move & Forward — Deep Dive | [§5](#-5-move--forward--deep-dive) |

> ```bash
> g++ -std=c++20 -pthread -o output filename.cpp && ./output
> ```

---

## 🔹 1. Exception Handling & Safety

> *📄 See full code → `01_exception_handling.cpp`*

### The Concept

When something goes wrong at runtime — a file doesn't exist, a network call fails, someone divides by zero — your program needs to handle it gracefully instead of crashing.

**Exceptions** are C++'s structured error-handling mechanism. When an error occurs, you `throw` an exception. It propagates up the call stack until someone `catch`es it. If nobody catches it, the program terminates.

> Think of it as a **fire alarm system**. Someone pulls the alarm (`throw`). The alarm propagates up through floors (function calls). The first floor with firefighters (`catch`) handles it. Intermediate floors don't need to know about the fire.

### Example

```cpp
#include <iostream>
#include <stdexcept>
#include <string>

// ── Custom exception ──
class NetworkError : public std::runtime_error {
    int code_;
public:
    NetworkError(const std::string& msg, int code)
        : std::runtime_error(msg), code_(code) {}

    int code() const noexcept { return code_; }
};

// ── A function that can fail ──
double safeDivide(double a, double b) {
    if (b == 0.0)
        throw std::invalid_argument("Division by zero");
    return a / b;
}

int main() {
    // ── Catching exceptions ──
    try {
        std::cout << safeDivide(10, 3) << '\n';    // 3.33333
        std::cout << safeDivide(10, 0) << '\n';    // throws!
    }
    catch (const std::invalid_argument& e) {
        std::cout << "Invalid: " << e.what() << '\n';
    }
    catch (const std::exception& e) {              // Base class — catches all std exceptions
        std::cout << "Error: " << e.what() << '\n';
    }
    catch (...) {                                  // Catch-all
        std::cout << "Unknown exception\n";
    }

    // ── Custom exception ──
    try {
        throw NetworkError("Connection refused", 503);
    }
    catch (const NetworkError& e) {
        std::cout << "Network error " << e.code() << ": " << e.what() << '\n';
    }
}
```

### 🖥️ Output

```
3.33333
Invalid: Division by zero
Network error 503: Connection refused
```

### The Three Exception Safety Guarantees

Every function you write provides one of these levels:

| Level | Guarantee | Example |
|:-----:|-----------|---------|
| **No-throw** | Never throws. Marked `noexcept`. | Destructors, `swap`, move operations |
| **Strong** | If it throws, state rolls back completely — as if the operation never happened. | Copy-and-swap assignment |
| **Basic** | If it throws, no resources leak and the object is in a valid (but possibly changed) state. | Most standard library operations |

### Why `noexcept` Matters for Performance

```cpp
// Move constructor — should ALWAYS be noexcept
Buffer(Buffer&& other) noexcept;
```

`std::vector` uses move during reallocation — but **only** if the move constructor is `noexcept`. If it's not `noexcept`, vector falls back to **copying** (much slower) because a throwing move halfway through reallocation leaves some elements moved and some not — unrecoverable.

---

## 🔹 2. Design Patterns

> *📄 See full code → `02_design_patterns.cpp`*

### The Concept

Design patterns are **battle-tested solutions** to recurring software design problems. They're not copy-paste code — they're strategies and structures that experienced developers reach for when they recognize a familiar problem.

---

### 2a. Singleton — One and Only One

**Problem:** You need exactly one instance of a class globally accessible (logger, config manager, connection pool).

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger inst;                // Created once, lives forever
        return inst;                       // Thread-safe since C++11
    }

    void log(const std::string& msg) {
        std::cout << "[LOG] " << msg << '\n';
    }

    Logger(const Logger&) = delete;            // No copying
    Logger& operator=(const Logger&) = delete;

private:
    Logger() {}                                // Private constructor
};

// Usage:
Logger::instance().log("Server started");
Logger::instance().log("Connection received");
```

**How it works:** The constructor is private — nobody can create instances with `new`. The only access is through `instance()`, which creates the object on first call (static local) and returns the same reference forever after.

---

### 2b. Observer — Publish / Subscribe

**Problem:** When one object changes, several others need to be notified, but you don't want tight coupling.

> Analogy: YouTube subscriptions. You subscribe to a channel. When they post, all subscribers get notified.

```cpp
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

class EventBus {
    std::unordered_map<std::string,
        std::vector<std::function<void(const std::string&)>>> listeners_;

public:
    void subscribe(const std::string& event,
                   std::function<void(const std::string&)> callback) {
        listeners_[event].push_back(std::move(callback));
    }

    void publish(const std::string& event, const std::string& data) {
        if (auto it = listeners_.find(event); it != listeners_.end()) {
            for (auto& cb : it->second) cb(data);
        }
    }
};

// Usage:
EventBus bus;
bus.subscribe("user.login", [](const std::string& user) {
    std::cout << "Welcome, " << user << "!\n";
});
bus.subscribe("user.login", [](const std::string& user) {
    std::cout << "Sending login email to " << user << '\n';
});

bus.publish("user.login", "Alice");
// Output:
// Welcome, Alice!
// Sending login email to Alice
```

---

### 2c. Strategy — Swappable Algorithms

**Problem:** You have a class that needs to do something, but the *how* should be changeable at runtime.

> Analogy: A GPS app that can switch between "fastest route," "shortest route," and "avoid highways" — same destination, different strategy.

```cpp
#include <functional>
#include <vector>
#include <algorithm>

class Sorter {
    std::function<bool(int, int)> strategy_;

public:
    Sorter(std::function<bool(int, int)> s) : strategy_(std::move(s)) {}

    void setStrategy(std::function<bool(int, int)> s) {
        strategy_ = std::move(s);
    }

    void sort(std::vector<int>& data) {
        std::sort(data.begin(), data.end(), strategy_);
    }
};

// Usage:
std::vector<int> data = {5, 3, 8, 1, 9};

Sorter sorter([](int a, int b) { return a < b; });    // Ascending
sorter.sort(data);           // {1, 3, 5, 8, 9}

sorter.setStrategy([](int a, int b) { return a > b; });  // Switch to descending
sorter.sort(data);           // {9, 8, 5, 3, 1}
```

---

### 2d. Factory — Create Without Knowing Concrete Types

**Problem:** You want to create objects based on runtime data (a config file, user input) without `if/else` chains.

```cpp
#include <memory>
#include <unordered_map>

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Circle : public Shape {
    double r_;
public:
    Circle(double r) : r_(r) {}
    void draw() const override { std::cout << "○ Circle (r=" << r_ << ")\n"; }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    void draw() const override { std::cout << "▭ Rectangle (" << w_ << "x" << h_ << ")\n"; }
};

std::unique_ptr<Shape> createShape(const std::string& type, double a, double b = 0) {
    if (type == "circle")    return std::make_unique<Circle>(a);
    if (type == "rectangle") return std::make_unique<Rectangle>(a, b);
    return nullptr;
}

// Usage:
auto s1 = createShape("circle", 5);
auto s2 = createShape("rectangle", 4, 6);
s1->draw();    // ○ Circle (r=5)
s2->draw();    // ▭ Rectangle (4x6)
```

---

### 2e. CRTP — Static Polymorphism

**Problem:** You want polymorphism but can't afford the runtime cost of virtual function calls (hot loops, embedded systems).

```cpp
template<typename Derived>
class Printable {
public:
    void print() const {
        static_cast<const Derived*>(this)->toString();  // Compile-time dispatch
    }
};

class Dog : public Printable<Dog> {
public:
    void toString() const { std::cout << "I'm a Dog\n"; }
};

class Cat : public Printable<Cat> {
public:
    void toString() const { std::cout << "I'm a Cat\n"; }
};

Dog d; d.print();    // I'm a Dog  — no vtable, no virtual, zero overhead
Cat c; c.print();    // I'm a Cat
```

---

### 2f. Pimpl — Compilation Firewall

**Problem:** Changing private members of a class forces all files that include its header to recompile.

```cpp
// ── widget.h ──
class Widget {
    struct Impl;                          // Forward declaration only
    std::unique_ptr<Impl> pimpl_;         // Pointer to hidden implementation

public:
    Widget();
    ~Widget();
    void doWork();
};

// ── widget.cpp ──
struct Widget::Impl {
    int data_ = 42;                      // Full implementation here
    void internal() { /* ... */ }
};

Widget::Widget() : pimpl_(std::make_unique<Impl>()) {}
Widget::~Widget() = default;
void Widget::doWork() { pimpl_->internal(); }
```

Change `Impl` all you want — only `widget.cpp` recompiles. Files that `#include "widget.h"` are unaffected. This dramatically speeds up builds in large projects.

---

## 🔹 3. Type Traits & Metaprogramming

> *📄 See full code → `03_type_traits_metaprogramming.cpp`*

### The Concept

Normal code operates on **values** (add two numbers, sort a list). **Metaprogramming** operates on **types** — asking questions about types and generating different code based on the answers, all at compile time.

### Asking Questions About Types

```cpp
#include <type_traits>
#include <iostream>

template<typename T>
void inspect() {
    std::cout << std::boolalpha;
    std::cout << "  is_integral:       " << std::is_integral_v<T>       << '\n';
    std::cout << "  is_floating_point: " << std::is_floating_point_v<T> << '\n';
    std::cout << "  is_pointer:        " << std::is_pointer_v<T>        << '\n';
    std::cout << "  is_class:          " << std::is_class_v<T>          << '\n';
    std::cout << "  is_const:          " << std::is_const_v<T>          << '\n';
}

int main() {
    std::cout << "int:\n";              inspect<int>();
    std::cout << "double:\n";           inspect<double>();
    std::cout << "const std::string:\n"; inspect<const std::string>();
}
```

### Transforming Types

```cpp
using T1 = std::remove_const_t<const int>;       // int
using T2 = std::remove_reference_t<int&>;        // int
using T3 = std::add_pointer_t<double>;           // double*
using T4 = std::decay_t<const int&>;             // int  (strips ref + const)
using T5 = std::conditional_t<true, int, double>; // int  (compile-time ternary)
```

### SFINAE vs. Concepts — Old Way vs. New Way

```cpp
// ── Old way: SFINAE (C++11/14) — cryptic but works ──
template<typename T>
std::enable_if_t<std::is_integral_v<T>, std::string>
classify(T) { return "integer"; }

template<typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::string>
classify(T) { return "floating point"; }

// ── New way: Concepts (C++20) — clean and readable ──
template<std::integral T>
std::string classify(T) { return "integer"; }

template<std::floating_point T>
std::string classify(T) { return "floating point"; }
```

### Compile-Time Computation

```cpp
constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

static_assert(fibonacci(10) == 55);     // ✅ Verified at compile time
constexpr auto fib20 = fibonacci(20);   // 6765, computed at compile time
```

---

## 🔹 4. Multithreading Patterns

> *📄 See full code → `04_multithreading_patterns.cpp`*

### The Concept

When multiple threads access shared data without coordination, **race conditions** occur — unpredictable bugs that appear randomly. This section shows production-ready patterns for safe concurrent programming.

---

### 4a. Thread-Safe Queue (Producer-Consumer)

> Analogy: A **factory conveyor belt**. Workers at one end place items on the belt (produce). Workers at the other end pick items off (consume). The belt itself ensures safe handoff.

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;

public:
    void push(T value) {
        {
            std::lock_guard lock(mutex_);
            queue_.push(std::move(value));
        }
        cv_.notify_one();    // Wake up one waiting consumer
    }

    T pop() {
        std::unique_lock lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); });  // Sleep until data
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }
};

// Usage:
ThreadSafeQueue<int> queue;

// Producer thread
std::jthread producer([&queue] {
    for (int i = 0; i < 10; i++) {
        queue.push(i);
    }
});

// Consumer thread
std::jthread consumer([&queue] {
    for (int i = 0; i < 10; i++) {
        int val = queue.pop();
        std::cout << "Got: " << val << '\n';
    }
});
```

**How it works:**
- `push()` locks the mutex, adds data, unlocks, then wakes one waiting thread.
- `pop()` locks the mutex. If the queue is empty, `cv_.wait()` **releases the lock and sleeps** until a producer calls `notify_one()`. This avoids busy-waiting (spinning in a loop wasting CPU).

---

### 4b. Atomics — Lock-Free Counters

When you only need to increment/read a simple value, a **mutex is overkill**. Atomics provide thread-safe operations at the hardware level — a single CPU instruction that can't be interrupted.

```cpp
#include <atomic>
#include <thread>
#include <vector>

std::atomic<int> counter{0};

void increment(int times) {
    for (int i = 0; i < times; i++) {
        counter++;    // Atomic increment — safe without mutex
    }
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(increment, 1000);
    }
    for (auto& t : threads) t.join();

    std::cout << "Counter: " << counter << '\n';   // Always exactly 10000
}
```

---

### 4c. `async` / `future` — High-Level Parallelism

```cpp
#include <future>
#include <numeric>
#include <vector>

long long parallelSum(const std::vector<int>& data) {
    auto mid = data.begin() + data.size() / 2;

    // Launch first half in a separate thread
    auto future = std::async(std::launch::async, [&] {
        return std::accumulate(data.begin(), mid, 0LL);
    });

    // Compute second half in this thread
    long long secondHalf = std::accumulate(mid, data.end(), 0LL);

    return future.get() + secondHalf;    // .get() waits for result
}
```

`std::async` launches work and returns a `future`. Call `.get()` when you need the result — it blocks until the work is done.

---

### 4d. `std::jthread` (C++20) — Auto-Joining + Cooperative Cancellation

```cpp
#include <thread>
#include <iostream>

void worker(std::stop_token stoken) {
    int i = 0;
    while (!stoken.stop_requested()) {    // Check if told to stop
        std::cout << "Working... " << i++ << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Worker stopped gracefully.\n";
}

int main() {
    std::jthread t(worker);                // Start worker

    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    t.request_stop();                      // Ask worker to stop

    // No need to call t.join() — jthread's destructor does it!
}
```

---

## 🔹 5. Move & Forward — Deep Dive

> *📄 See full code → `05_move_forward_deep.cpp`*

### The Concept

This section connects all the dots about lvalues, rvalues, universal references, and perfect forwarding — the most confusing (and most powerful) part of modern C++.

### 5a. Value Categories — Lvalue vs. Rvalue

```cpp
int x = 42;          // x is an LVALUE — has a name, has an address
int y = x + 1;       // (x + 1) is an RVALUE — temporary, about to die

std::string s = "Hi";
// s              → lvalue (persists)
// std::move(s)   → rvalue (we're saying "treat s as temporary")
// std::string()  → rvalue (anonymous temporary)
```

**Key insight:** Inside a function, every named parameter is an **lvalue** — even if it was passed as an rvalue:

```cpp
void foo(std::string&& s) {
    // s is an LVALUE here! It has a name.
    // To treat it as an rvalue again: std::move(s)
}
```

### 5b. Reference Collapsing Rules

When templates create "references to references," the compiler collapses them:

```
  T&   &   →  T&      (lvalue wins)
  T&   &&  →  T&      (lvalue wins)
  T&&  &   →  T&      (lvalue wins)
  T&&  &&  →  T&&     (only rvalue + rvalue → rvalue)
```

> **Rule:** If there's an `&` anywhere, the result is `&`. Only `&& &&` stays `&&`.

### 5c. Universal References (`T&&` where T is deduced)

```cpp
template<typename T>
void wrapper(T&& arg) {     // ← Universal reference (T is deduced!)
    // If called with lvalue:  T = string&,  arg = string& && = string&
    // If called with rvalue:  T = string,   arg = string&&
}

void other(std::string&& arg) {   // ← NOT universal. Regular rvalue reference.
    // Only accepts rvalues
}
```

The magic: `T&&` in a template with deduced `T` can bind to **both** lvalues and rvalues.

### 5d. Perfect Forwarding — The Problem & Solution

```cpp
#include <iostream>
#include <string>

class Heavy {
public:
    Heavy(const std::string& s) { std::cout << "  COPIED\n"; }
    Heavy(std::string&& s)      { std::cout << "  MOVED\n"; }
};

// ❌ BAD: Always copies, even when passed an rvalue
template<typename T>
Heavy makeBad(T&& arg) {
    return Heavy(arg);                    // arg has a name → lvalue → calls copy!
}

// ✅ GOOD: Preserves the original value category
template<typename T>
Heavy makeGood(T&& arg) {
    return Heavy(std::forward<T>(arg));   // Forward as lvalue or rvalue
}

int main() {
    std::string s = "data";

    std::cout << "makeBad(lvalue):  "; makeBad(s);              // COPIED ✓
    std::cout << "makeBad(rvalue):  "; makeBad(std::move(s));   // COPIED ✗ (should move!)

    s = "data";
    std::cout << "makeGood(lvalue): "; makeGood(s);             // COPIED ✓
    std::cout << "makeGood(rvalue): "; makeGood(std::move(s));  // MOVED ✓ (correct!)
}
```

### 🖥️ Output

```
makeBad(lvalue):    COPIED
makeBad(rvalue):    COPIED    ← Wrong! We lost the rvalue-ness
makeGood(lvalue):   COPIED
makeGood(rvalue):   MOVED     ← Correct! Forward preserved it
```

### How `std::forward` Works

`std::forward<T>(arg)` is a conditional cast:
- If `T` is `string&` (came from lvalue) → returns `string&` (lvalue)
- If `T` is `string` (came from rvalue) → returns `string&&` (rvalue)

It "forwards" the value exactly as the caller sent it. This is the key to writing efficient wrapper functions, factory methods, and `emplace_back`-style APIs.

### The Practical Pattern: Variadic Perfect Forwarding

```cpp
template<typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Forwards ALL arguments perfectly to T's constructor
auto p = make<std::string>(5, 'x');    // Constructs string("xxxxx") in-place
```

This is exactly how `std::make_unique`, `emplace_back`, and `emplace` work internally.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 You've reached the end of the C++ study material.</em><br/>
  <em>From Hello World to Perfect Forwarding — well done.</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
