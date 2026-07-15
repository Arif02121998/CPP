<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 7</samp>
</p>

<h1 align="center">📘 C++14 — Polishing the Revolution</h1>

<p align="center"><em>"C++14 is what C++11 should have been."<br/>It didn't add radical features — it refined and completed the C++11 vision.</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Generic Lambdas | [§1](#-1-generic-lambdas) |
| 2 | Return Type Deduction | [§2](#-2-return-type-deduction) |
| 3 | Variable Templates | [§3](#-3-variable-templates) |
| 4 | Relaxed `constexpr` | [§4](#-4-relaxed-constexpr) |
| 5 | `std::make_unique` | [§5](#-5-stdmake_unique) |
| 6 | Binary Literals & Digit Separators | [§6](#-6-binary-literals--digit-separators) |
| 7 | `[[deprecated]]` | [§7](#-7-deprecated) |

> *📄 See full code → `01_cpp14_features.cpp`*
>
> ```bash
> g++ -std=c++14 -o output 01_cpp14_features.cpp && ./output
> ```

---

## 🔹 1. Generic Lambdas

### The Concept

In C++11, lambda parameters needed explicit types: `[](int a, int b)`. C++14 lets you use **`auto`** — making the lambda work with **any type**, just like a template function.

### Example

```cpp
auto add = [](auto a, auto b) { return a + b; };

std::cout << add(3, 4)          << '\n';   // 7     (int + int)
std::cout << add(3.14, 2.71)    << '\n';   // 5.85  (double + double)
std::cout << add(std::string("Hello"), std::string(" World")) << '\n';
// "Hello World"  (string + string)
```

### How This Works

The compiler treats each `auto` parameter like a template parameter. Behind the scenes, `add` becomes a callable object with a templated `operator()`:

```cpp
struct __lambda {
    template<typename T, typename U>
    auto operator()(T a, U b) const { return a + b; }
};
```

One lambda, infinite type combinations. This is the foundation for writing highly reusable lambdas.

---

## 🔹 2. Return Type Deduction

### The Concept

C++14 lets you use `auto` as a function's return type. The compiler looks at the `return` statement and deduces the type.

### Before & After

```cpp
// ── C++11: Must specify or use trailing return type ──
auto multiply(double a, double b) -> double {
    return a * b;
}

// ── C++14: Just use auto ──
auto multiply(double a, double b) {
    return a * b;    // Compiler deduces: double
}

auto makeGreeting(const std::string& name) {
    return "Hello, " + name + "!";    // Deduces: std::string
}

std::cout << multiply(3.0, 4.0) << '\n';        // 12
std::cout << makeGreeting("Alice") << '\n';      // Hello, Alice!
```

> ⚠️ **Caveat:** All `return` statements in the function must return the same type. You can't have one path return `int` and another return `std::string`.

---

## 🔹 3. Variable Templates

### The Concept

C++11 gave us function templates and class templates. C++14 adds **variable templates** — a variable whose type or value depends on a template parameter.

### Example

```cpp
#include <iostream>

template<typename T>
constexpr T pi = T(3.14159265358979323846L);

int main() {
    std::cout << "float pi:  " << pi<float>       << '\n';   // 3.14159
    std::cout << "double pi: " << pi<double>      << '\n';   // 3.14159265358979
    std::cout << "int pi:    " << pi<int>         << '\n';   // 3

    // Useful for type-dependent constants
    float area_f  = pi<float> * 5.0f * 5.0f;
    double area_d = pi<double> * 5.0 * 5.0;
}
```

Before C++14, you'd need a static member of a template class or a function to achieve this. Variable templates are much cleaner.

---

## 🔹 4. Relaxed `constexpr`

### The Concept

C++11 `constexpr` functions were severely limited: only a single `return` statement, no loops, no `if/else`. C++14 **lifts these restrictions** — you can write normal-looking code that runs at compile time.

### Example

```cpp
// ── C++11: Only one return statement allowed ──
constexpr int factorial_11(int n) {
    return (n <= 1) ? 1 : n * factorial_11(n - 1);   // Must use ternary + recursion
}

// ── C++14: Loops, variables, if/else — all allowed! ──
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

constexpr int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// Computed entirely at COMPILE TIME — zero runtime cost
constexpr int fact10 = factorial(10);      // 3628800
constexpr int fib15  = fibonacci(15);      // 610

static_assert(factorial(5) == 120, "Math is broken");     // ✅ Verified at compile time
static_assert(fibonacci(10) == 55, "Fibonacci is wrong"); // ✅
```

### Why This Matters

The values `fact10` and `fib15` are baked into the executable as constants. No function is called at runtime. This is free performance — complex computations with zero cost.

---

## 🔹 5. `std::make_unique`

### The Concept

C++11 introduced `unique_ptr` but forgot to add `make_unique` (it had `make_shared` though). C++14 fills this gap.

### Example

```cpp
#include <memory>
#include <iostream>

struct Widget {
    int id;
    Widget(int id) : id(id) { std::cout << "Widget " << id << " created\n"; }
    ~Widget()                { std::cout << "Widget " << id << " destroyed\n"; }
};

int main() {
    // ── C++11 way (verbose, potential issues) ──
    std::unique_ptr<Widget> w1(new Widget(1));

    // ── C++14 way (clean, safe) ──
    auto w2 = std::make_unique<Widget>(2);

    // ── Array version ──
    auto arr = std::make_unique<int[]>(5);
    arr[0] = 42;

    w2->id;   // Use like a regular pointer
}   // Both destroyed automatically
```

`make_unique` is preferred because:
1. **Cleaner syntax** — no `new` keyword
2. **Exception safe** — avoids a subtle leak when `new` and another expression are evaluated in the same statement
3. **Consistency** — `make_unique` + `make_shared` = no raw `new` anywhere

---

## 🔹 6. Binary Literals & Digit Separators

### The Concept

C++14 adds two readability improvements for numeric literals.

### Example

```cpp
// ── Binary literals (prefix 0b) ──
int flags   = 0b1010'1100;        // Much clearer than 0xAC or 172
int mask    = 0b0000'1111;

// ── Digit separators (apostrophe) ──
int million  = 1'000'000;         // Readable! Compare: 1000000
long long big = 1'000'000'000'000LL;
double sci   = 1'234.567'89;

// ── Combine them ──
int byte_pattern = 0b1111'0000'1010'0101;

std::cout << "flags: "   << flags   << '\n';   // 172
std::cout << "million: " << million << '\n';    // 1000000
```

The `'` separator is purely visual — the compiler ignores it completely. Place them anywhere between digits for readability.

---

## 🔹 7. `[[deprecated]]`

### The Concept

Mark functions, classes, or variables as **deprecated** — the compiler warns users to stop using them.

### Example

```cpp
[[deprecated("Use newFunction() instead")]]
void oldFunction() {
    std::cout << "I'm old and tired\n";
}

[[deprecated]]
int legacyGlobal = 42;

int main() {
    oldFunction();     // ⚠️ Compiler warning: 'oldFunction' is deprecated
}
```

This is how libraries gently push users toward newer APIs without breaking existing code immediately.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 8: C++17 — Practical Powerups</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
