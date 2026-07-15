<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 6</samp>
</p>

<h1 align="center">📘 C++11 — The Modern Revolution</h1>

<p align="center"><em>"C++11 felt like a new language."<br/>— Bjarne Stroustrup</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | `auto` & `decltype` | [§1](#-1-auto--decltype) |
| 2 | Lambda Expressions | [§2](#-2-lambda-expressions) |
| 3 | Smart Pointers | [§3](#-3-smart-pointers) |
| 4 | Move Semantics | [§4](#-4-move-semantics) |
| 5 | Modern Syntax | [§5](#-5-modern-syntax) |
| 6 | Threads | [§6](#-6-threads) |

> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. `auto` & `decltype`

> *📄 See full code → `01_auto_decltype.cpp`*

### The Concept

**`auto`** tells the compiler: *"Figure out the type from the right-hand side."* It saves you from typing long type names and makes code more readable.

**`decltype`** asks: *"What is the type of this expression?"* — without evaluating it.

### Example

```cpp
#include <iostream>
#include <vector>
#include <map>
#include <string>

int main() {
    // ── auto deduces the type ──
    auto i = 42;                      // int
    auto d = 3.14;                    // double
    auto s = std::string("Hello");    // std::string
    auto b = true;                    // bool

    // ── Most useful: complex types ──
    std::map<std::string, std::vector<int>> data;
    auto it = data.begin();
    // Without auto: std::map<std::string, std::vector<int>>::iterator it = data.begin();

    // ── decltype: query the type of an expression ──
    int x = 10;
    decltype(x) y = 20;              // y is int (same type as x)
    decltype(x + d) z = 1.5;         // z is double (int + double → double)

    std::cout << "i=" << i << " d=" << d << " s=" << s << '\n';
}
```

### How This Works

`auto` performs the same type deduction the compiler already does for template parameters. When you write `auto x = 42;`, your intention is clear and the compiler fills in `int` for you. No runtime cost — the type is fully resolved at compile time.

**When to use `auto`:**
- ✅ Iterator types: `auto it = map.begin();`
- ✅ Lambda return types: `auto fn = [](int x) { return x * 2; };`
- ✅ Long template types
- ❌ When the type isn't obvious: `auto result = compute();` — what is it? Prefer explicit types when clarity matters.

---

## 🔹 2. Lambda Expressions

> *📄 See full code → `02_lambda.cpp`*

### The Concept

A **lambda** is a function you define **right where you need it**, without giving it a name. Think of it as a **Post-it note with instructions** — you write them on the spot and hand them to someone (an algorithm, a thread).

### Syntax Anatomy

```
[capture](parameters) -> return_type { body }
  │          │              │           │
  │          │              │           └── The code
  │          │              └── Optional (compiler deduces)
  │          └── Inputs, like a normal function
  └── Which outside variables can this lambda see?
```

### Example

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // ── Basic lambda ──
    auto greet = []() { std::cout << "Hello from lambda!\n"; };
    greet();                           // Hello from lambda!

    auto add = [](int a, int b) { return a + b; };
    std::cout << add(3, 4) << '\n';    // 7

    // ── Captures: accessing outside variables ──
    int x = 10, y = 20;

    auto byVal  = [x, y]() { std::cout << x + y << '\n'; };   // Copies x, y
    auto byRef  = [&x, &y]() { x += 5; y += 5; };            // References x, y
    auto allVal = [=]() { return x + y; };                     // Copy ALL
    auto allRef = [&]() { x = 99; };                           // Reference ALL

    byRef();
    std::cout << "x=" << x << " y=" << y << '\n';   // x=15 y=25

    // ── mutable: modify a captured copy ──
    auto counter = [count = 0]() mutable {
        return ++count;
    };
    std::cout << counter() << ' ' << counter() << ' ' << counter() << '\n';
    // 1 2 3

    // ── Lambdas with STL algorithms ──
    std::vector<int> nums = {5, 3, 8, 1, 9, 2};

    std::sort(nums.begin(), nums.end(), [](int a, int b) {
        return a > b;    // Sort descending
    });
    // nums = {9, 8, 5, 3, 2, 1}

    int count = std::count_if(nums.begin(), nums.end(),
        [](int n) { return n > 5; });
    std::cout << count << " elements > 5\n";   // 3

    // ── IIFE: Immediately Invoked Function Expression ──
    const int result = [](int n) {
        int sum = 0;
        for (int i = 1; i <= n; i++) sum += i;
        return sum;
    }(100);    // Called immediately with n=100
    std::cout << "Sum 1..100 = " << result << '\n';   // 5050
}
```

### Capture Cheat Sheet

| Capture | Meaning |
|:-------:|---------|
| `[]` | Captures nothing |
| `[x]` | Copy `x` |
| `[&x]` | Reference to `x` |
| `[=]` | Copy all used variables |
| `[&]` | Reference all used variables |
| `[=, &x]` | Copy all, except `x` by reference |
| `[this]` | Capture the `this` pointer in a class |

---

## 🔹 3. Smart Pointers

> *📄 See full code → `03_smart_pointers.cpp`*

### The Concept

Smart pointers are **RAII wrappers** for heap memory. They automatically free memory when no longer needed — no `delete` required, no leaks possible.

### The Three Smart Pointers

```
  ┌─────────────────────┐
  │    unique_ptr        │   ONE owner. Freed when owner dies.
  │    ┌─────┐           │   Cannot be copied. Can be moved.
  │    │ ptr ├──► Object │
  │    └─────┘           │
  └─────────────────────┘

  ┌─────────────────────┐
  │    shared_ptr        │   MULTIPLE owners. Freed when LAST owner dies.
  │    ┌─────┐           │   Reference counted.
  │    │ ptr ├──► Object │ ◄── ptr2
  │    └─────┘  count=2  │
  └─────────────────────┘

  ┌─────────────────────┐
  │    weak_ptr          │   NON-OWNING observer.
  │    ┌─────┐           │   Must .lock() to use. Doesn't prevent deletion.
  │    │ ptr ├ ─ ─►Object│   Breaks circular references.
  │    └─────┘           │
  └─────────────────────┘
```

### Example

```cpp
#include <iostream>
#include <memory>

class Sensor {
    std::string name_;
public:
    Sensor(const std::string& n) : name_(n) {
        std::cout << "  Sensor " << name_ << " created\n";
    }
    ~Sensor() {
        std::cout << "  Sensor " << name_ << " destroyed\n";
    }
    void read() const { std::cout << "  Reading " << name_ << '\n'; }
};

int main() {
    // ── unique_ptr: exclusive ownership ──
    {
        auto temp = std::make_unique<Sensor>("Temperature");
        temp->read();
        // auto copy = temp;                 // ❌ Can't copy!
        auto moved = std::move(temp);        // ✅ Transfer ownership
        moved->read();
    }   // ← Sensor destroyed here

    // ── shared_ptr: shared ownership ──
    {
        auto pressure = std::make_shared<Sensor>("Pressure");
        std::cout << "  Owners: " << pressure.use_count() << '\n';   // 1

        auto alias = pressure;              // Share ownership
        std::cout << "  Owners: " << pressure.use_count() << '\n';   // 2

        alias.reset();                      // Give up ownership
        std::cout << "  Owners: " << pressure.use_count() << '\n';   // 1
    }   // ← Sensor destroyed (last owner gone)

    // ── weak_ptr: non-owning observer ──
    std::weak_ptr<Sensor> observer;
    {
        auto humidity = std::make_shared<Sensor>("Humidity");
        observer = humidity;                // Observe without owning

        if (auto locked = observer.lock()) {
            locked->read();                 // Safe to use
        }
    }   // ← Humidity destroyed (shared_ptr gone)

    if (observer.expired()) {
        std::cout << "  Sensor no longer exists\n";
    }
}
```

### When to Use Which?

| Smart Pointer | Ownership | Use When |
|:---:|:---:|---|
| `unique_ptr` | Exclusive | **Default choice.** One clear owner. |
| `shared_ptr` | Shared | Multiple owners need the same object. |
| `weak_ptr` | None | Observing without preventing destruction. |

> 💡 **Golden Rule:** Use `std::make_unique` and `std::make_shared` — never raw `new`.

---

## 🔹 4. Move Semantics

> *📄 See full code → `04_move_semantics.cpp`*

### The Concept

Before C++11, passing large objects around meant **copying** — duplicating all the data. Move semantics let you **steal** resources from an object that's about to die, like handing your suitcase to the next person instead of making a copy.

### Example

```cpp
#include <iostream>
#include <vector>
#include <string>

class Buffer {
    int* data_;
    size_t size_;

public:
    Buffer(size_t size) : data_(new int[size]{}), size_(size) {
        std::cout << "  Constructed " << size << " ints\n";
    }

    // Copy constructor (expensive — allocates + copies)
    Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  COPIED " << size_ << " ints\n";
    }

    // Move constructor (cheap — just steal the pointer)
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "  MOVED (no allocation!)\n";
    }

    ~Buffer() { delete[] data_; }
};

int main() {
    Buffer a(1000000);             // Constructed 1000000 ints

    Buffer b = a;                  // COPIED 1000000 ints (expensive!)
    Buffer c = std::move(a);       // MOVED (no allocation!)  (fast!)

    // a is now empty (moved-from state) — don't use it without reassigning
}
```

### How This Works — Visually

```
  COPY (deep):                      MOVE (steal):
  ┌─────┐    ┌─────┐               ┌─────┐    ┌─────┐
  │  a  │    │  b  │               │  a  │    │  c  │
  │ ptr─┼─►  │ ptr─┼─►             │ null│    │ ptr─┼─►
  └─────┘    └─────┘               └─────┘    └─────┘
    │ [data]   │ [copy of data]      (empty)     │ [a's data!]
    ▼          ▼
  Allocate + Copy every element     Just swap two pointers. Done.
```

`std::move(a)` doesn't actually move — it **casts** `a` to an rvalue reference (`Buffer&&`), giving **permission** to steal. The move constructor then takes the pointer and nullifies `a`.

---

## 🔹 5. Modern Syntax

> *📄 See full code → `05_modern_syntax.cpp`*

### Key C++11 Syntax Improvements

```cpp
#include <iostream>
#include <vector>
#include <string>

int main() {
    // ── Uniform initialization ──
    int x{42};
    std::vector<int> v{1, 2, 3, 4, 5};
    std::string s{"Hello"};

    // ── Range-based for ──
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << '\n';

    // ── nullptr (replaces NULL) ──
    int* p = nullptr;       // Type-safe null pointer

    // ── enum class (scoped enums) ──
    enum class Color { Red, Green, Blue };
    Color c = Color::Red;   // Must scope with Color::

    // ── using (modern typedef) ──
    using StringVec = std::vector<std::string>;
    StringVec names = {"Alice", "Bob"};

    // ── constexpr (compile-time computation) ──
    constexpr int square(int n) { return n * n; }
    constexpr int val = square(5);   // Computed at compile time: 25

    // ── static_assert (compile-time check) ──
    static_assert(sizeof(int) == 4, "int must be 4 bytes");
}
```

---

## 🔹 6. Threads

> *📄 See full code → `06_threads.cpp`*

### The Concept

C++11 added built-in threading support. A **thread** is a separate flow of execution — like having two cooks in a kitchen working on different dishes simultaneously.

### Example

```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;                        // Protects shared data

void printMessage(const std::string& msg, int count) {
    for (int i = 0; i < count; i++) {
        std::lock_guard<std::mutex> lock(mtx);    // Auto-locks/unlocks
        std::cout << msg << " #" << i << '\n';
    }
}

int main() {
    std::thread t1(printMessage, "Thread-A", 3);
    std::thread t2(printMessage, "Thread-B", 3);

    t1.join();     // Wait for t1 to finish
    t2.join();     // Wait for t2 to finish

    std::cout << "Both threads done.\n";
}
```

### 🖥️ Possible Output

```
Thread-A #0
Thread-A #1
Thread-B #0
Thread-A #2
Thread-B #1
Thread-B #2
Both threads done.
```

### How This Works

| Part | Purpose |
|------|---------|
| `std::thread t1(func, args...)` | Launches a new thread running `func` |
| `t1.join()` | Blocks until `t1` finishes — **must** be called (or `detach()`) |
| `std::mutex` | A lock. Only one thread can hold it at a time |
| `std::lock_guard<std::mutex>` | RAII wrapper — locks in constructor, unlocks in destructor |

Without the mutex, both threads printing simultaneously would interleave characters, producing garbled output. The `lock_guard` ensures only one thread prints at a time.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 7: C++14 — Refinements</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
