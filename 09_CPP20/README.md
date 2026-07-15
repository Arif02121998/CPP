<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 9</samp>
</p>

<h1 align="center">📘 C++20 — The Big Four</h1>

<p align="center"><em>"C++20 is as big a change as C++11."<br/>Concepts, Ranges, Coroutines, and Modules redefine how we write C++.</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Concepts — Constrained Templates | [§1](#-1-concepts--constrained-templates) |
| 2 | Ranges — Composable Algorithms | [§2](#-2-ranges--composable-algorithms) |
| 3 | Coroutines — Pausable Functions | [§3](#-3-coroutines--pausable-functions) |
| 4 | The Spaceship Operator `<=>` | [§4](#-4-the-spaceship-operator-) |
| 5 | Other C++20 Features | [§5](#-5-other-c20-features) |

> ```bash
> g++ -std=c++20 -o output filename.cpp && ./output
> ```

---

## 🔹 1. Concepts — Constrained Templates

> *📄 See full code → `01_concepts.cpp`*

### The Concept

Templates are powerful but their error messages are **terrible**. If you pass a type that doesn't support `operator<`, you get pages of incomprehensible errors deep inside the template implementation.

**Concepts** fix this. A concept is a **named set of requirements** on a type. If a type doesn't meet the requirements, the compiler says so clearly — at the call site, not buried in template internals.

> Think of a concept as a **bouncer at a club door**: "You must be sortable to enter this function."

### Example

```cpp
#include <iostream>
#include <concepts>
#include <string>

// ── Using a standard concept ──
template<typename T>
requires std::integral<T>
T gcd(T a, T b) {
    while (b != 0) {
        T temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// gcd(3.14, 2.71);   // ❌ Clear error: "double does not satisfy integral"

// ── Define your own concept ──
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;       // a + b must exist and return T
};

template<Addable T>
T add(T a, T b) {
    return a + b;
}

// ── Shorthand: constrained auto ──
void printNumber(std::integral auto val) {
    std::cout << "Integer: " << val << '\n';
}

// ── Concept composition ──
template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template<Number T>
T square(T val) { return val * val; }

int main() {
    std::cout << gcd(12, 8)  << '\n';                 // 4
    std::cout << add(3, 4)   << '\n';                 // 7
    std::cout << add(std::string("Hi"), std::string(" there")) << '\n';  // Hi there

    printNumber(42);          // Integer: 42
    // printNumber(3.14);     // ❌ Error: double is not integral

    std::cout << square(5)   << '\n';    // 25
    std::cout << square(2.5) << '\n';    // 6.25
}
```

### Four Ways to Constrain Templates

```cpp
// 1. requires clause
template<typename T>
requires std::integral<T>
T foo(T x);

// 2. Concept as template parameter
template<std::integral T>
T foo(T x);

// 3. Constrained auto (shortest)
auto foo(std::integral auto x);

// 4. Trailing requires
template<typename T>
T foo(T x) requires std::integral<T>;
```

All four are equivalent. Choose based on readability.

---

## 🔹 2. Ranges — Composable Algorithms

> *📄 See full code → `02_ranges.cpp`*

### The Concept

STL algorithms require you to pass `begin()` and `end()` iterators everywhere. **Ranges** eliminate this — you pass the container directly. Even better, **views** let you chain operations like a pipeline without creating intermediate copies.

> Think of views as an **assembly line**. Raw materials (data) pass through stations (filter → transform → take) — each station modifies the view without copying the data.

### Example

```cpp
#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>

int main() {
    std::vector<int> v{5, 3, 1, 4, 2, 8, 7, 6, 9, 10};

    // ── Range algorithms (no more begin/end!) ──
    std::ranges::sort(v);
    // v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}

    auto it = std::ranges::find(v, 7);
    if (it != v.end()) std::cout << "Found: " << *it << '\n';

    // ── Views: lazy, composable, zero-copy ──
    std::cout << "Even squares (first 3): ";
    for (auto x : v
            | std::views::filter([](int x) { return x % 2 == 0; })    // Keep evens
            | std::views::transform([](int x) { return x * x; })      // Square them
            | std::views::take(3))                                     // First 3 only
    {
        std::cout << x << " ";
    }
    std::cout << '\n';    // 4 16 36

    // ── iota: generate a sequence ──
    std::cout << "1 to 10: ";
    for (auto x : std::views::iota(1, 11)) {
        std::cout << x << " ";
    }
    std::cout << '\n';    // 1 2 3 4 5 6 7 8 9 10

    // ── FizzBuzz in one pipeline ──
    std::cout << "FizzBuzz: ";
    for (auto i : std::views::iota(1, 16)) {
        if (i % 15 == 0) std::cout << "FizzBuzz ";
        else if (i % 3 == 0) std::cout << "Fizz ";
        else if (i % 5 == 0) std::cout << "Buzz ";
        else std::cout << i << " ";
    }
    std::cout << '\n';
}
```

### 🖥️ Output

```
Found: 7
Even squares (first 3): 4 16 36
1 to 10: 1 2 3 4 5 6 7 8 9 10
FizzBuzz: 1 2 Fizz 4 Buzz Fizz 7 8 Fizz Buzz 11 Fizz 13 14 FizzBuzz
```

### How Views Work

Views are **lazy** — they don't process data until you iterate. And they don't allocate memory. The pipeline `filter | transform | take` builds a lightweight chain of iterators that evaluate one element at a time.

| Old STL Way | Ranges Way |
|---|---|
| `std::sort(v.begin(), v.end())` | `std::ranges::sort(v)` |
| `std::find(v.begin(), v.end(), x)` | `std::ranges::find(v, x)` |
| Create temp vector for each step | Chain views — zero copies |

---

## 🔹 3. Coroutines — Pausable Functions

> *📄 See full code → `03_coroutines.cpp`*

### The Concept

A normal function runs start-to-finish. A **coroutine** can **pause** (`co_yield` / `co_await`), return a value, and **resume** later from where it left off.

> Think of an elevator. A normal function goes from floor 1 to floor 10 without stopping. A coroutine stops at each floor, lets passengers off (`co_yield`), and then continues to the next floor when called again.

### Example — A Generator

```cpp
#include <iostream>
#include <coroutine>
#include <optional>

// ── Minimal Generator template ──
template<typename T>
class Generator {
public:
    struct promise_type {
        std::optional<T> current_value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

private:
    std::coroutine_handle<promise_type> handle_;

public:
    explicit Generator(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Generator() { if (handle_) handle_.destroy(); }

    bool next() {
        handle_.resume();
        return !handle_.done();
    }

    T value() const { return *handle_.promise().current_value; }
};

// ── A coroutine that generates Fibonacci numbers forever ──
Generator<uint64_t> fibonacci() {
    uint64_t a = 0, b = 1;
    while (true) {
        co_yield a;            // Pause here, emit 'a'
        auto next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    auto fib = fibonacci();

    std::cout << "Fibonacci: ";
    for (int i = 0; i < 15 && fib.next(); i++) {
        std::cout << fib.value() << " ";
    }
    std::cout << '\n';
}
```

### 🖥️ Output

```
Fibonacci: 0 1 1 2 3 5 8 13 21 34 55 89 144 233 377
```

### How This Works

1. `fibonacci()` is called → creates a coroutine frame on the heap
2. It runs until `co_yield a` → pauses, stores `a` as the current value
3. `fib.next()` resumes execution → the loop continues until the next `co_yield`
4. This repeats until we stop calling `next()`
5. The coroutine produces values **on demand** — it never computes all of them at once

The Generator template is boilerplate that C++23 simplifies with `std::generator<T>`. The coroutine itself (`fibonacci()`) is elegant.

---

## 🔹 4. The Spaceship Operator `<=>`

> *📄 See full code → `04_spaceship_operator.cpp`*

### The Concept

Before C++20, to make a class comparable you had to write **six operators**: `==`, `!=`, `<`, `>`, `<=`, `>=`. The **three-way comparison operator** (`<=>`, called "spaceship") lets you write **one** and get all six for free.

### Example

```cpp
#include <iostream>
#include <compare>
#include <string>

// ── Default: compiler generates everything ──
struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;
    // That's it. You now have ==, !=, <, >, <=, >= — all six!
};

// ── Custom: define your own comparison logic ──
class Version {
    int major_, minor_, patch_;

public:
    Version(int ma, int mi, int pa) : major_(ma), minor_(mi), patch_(pa) {}

    std::strong_ordering operator<=>(const Version& other) const {
        if (auto cmp = major_ <=> other.major_; cmp != 0) return cmp;
        if (auto cmp = minor_ <=> other.minor_; cmp != 0) return cmp;
        return patch_ <=> other.patch_;
    }

    bool operator==(const Version& other) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Version& v) {
        return os << v.major_ << '.' << v.minor_ << '.' << v.patch_;
    }
};

int main() {
    // ── Points ──
    Point p1{1, 2}, p2{1, 3}, p3{1, 2};

    std::cout << std::boolalpha;
    std::cout << "p1 == p3: " << (p1 == p3) << '\n';   // true
    std::cout << "p1 <  p2: " << (p1 <  p2) << '\n';   // true
    std::cout << "p1 != p2: " << (p1 != p2) << '\n';   // true

    // ── Versions ──
    Version v1{2, 0, 0}, v2{1, 9, 9}, v3{2, 0, 0};

    std::cout << v1 << " > "  << v2 << ": " << (v1 > v2)  << '\n';  // true
    std::cout << v1 << " == " << v3 << ": " << (v1 == v3) << '\n';  // true
}
```

### The Three Return Types

| Return Type | Meaning | Example |
|---|---|---|
| `std::strong_ordering` | Every pair of values is either equal, less, or greater | Integers, strings |
| `std::weak_ordering` | "Equivalent" but not necessarily "equal" | Case-insensitive strings |
| `std::partial_ordering` | Some values are incomparable | Floating-point (NaN ≠ anything) |

---

## 🔹 5. Other C++20 Features

> *📄 See full code → `05_other_cpp20.cpp`*

### `std::span` — A Non-Owning View of Contiguous Data

```cpp
#include <span>
#include <iostream>
#include <vector>

// One function that works with C arrays, vectors, arrays, spans...
void printData(std::span<const int> data) {
    for (auto x : data) std::cout << x << " ";
    std::cout << '\n';
}

int main() {
    int c_arr[] = {1, 2, 3, 4, 5};
    std::vector<int> vec{10, 20, 30};

    printData(c_arr);                            // 1 2 3 4 5
    printData(vec);                              // 10 20 30
    printData(std::span(c_arr).subspan(1, 3));   // 2 3 4
}
```

`span` is to arrays what `string_view` is to strings — a lightweight, non-owning view.

### Designated Initializers

```cpp
struct Config {
    int width = 800;
    int height = 600;
    bool fullscreen = false;
    std::string title = "App";
};

Config cfg{
    .width = 1920,
    .height = 1080,
    .title = "My Game"        // Skip .fullscreen — keeps default
};
```

Self-documenting initialization. You see the field names right at the call site.

### `consteval` — Guaranteed Compile-Time

```cpp
consteval int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return result;
}

constexpr int f5 = factorial(5);      // ✅ Always compile-time
// int n = 5; factorial(n);           // ❌ Error: n is not a constant expression
```

`constexpr` = *may* run at compile time. `consteval` = *must* run at compile time. No exceptions.

### `std::source_location` — Better Than `__FILE__` and `__LINE__`

```cpp
#include <source_location>

void log(const std::string& msg,
         const std::source_location& loc = std::source_location::current()) {
    std::cout << loc.file_name() << ":" << loc.line()
              << " [" << loc.function_name() << "] " << msg << '\n';
}

log("Something happened");
// main.cpp:42 [main] Something happened
```

### `starts_with` / `ends_with` / `contains`

```cpp
std::string s = "Hello World";
s.starts_with("Hello");    // true
s.ends_with("World");      // true

std::map<std::string, int> m = {{"key", 1}};
m.contains("key");         // true (cleaner than m.count("key") > 0)
```

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 10: Advanced Topics</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
