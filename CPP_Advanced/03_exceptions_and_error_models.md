# Chapter 3 — Exception Handling & Error Models

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)
> Builds on [C++11 smart pointers/RAII](../CPP11/04_smart_pointers.md) and
> [C++17 `std::optional`](../CPP17/03_vocabulary_types.md).

How you report and handle errors shapes the safety and clarity of an entire
codebase. This chapter covers C++'s exception machinery **and** the alternatives.

---

## 3.1 Exception Mechanics

### The Basics

```cpp
double divide(int a, int b) {
    if (b == 0) throw std::runtime_error("division by zero");  // throw
    return static_cast<double>(a) / b;
}

try {
    auto r = divide(10, 0);
} catch (const std::runtime_error& e) {   // catch by const reference
    std::cerr << "error: " << e.what() << '\n';
} catch (const std::exception& e) {        // base class catches more
    std::cerr << "other: " << e.what() << '\n';
}
```

### Stack Unwinding

When an exception is thrown, the runtime **unwinds the stack**: it destroys all
local objects between the `throw` and the matching `catch`, calling their
destructors in reverse order. **This is why RAII works** — resources are released
automatically during unwinding.

```cpp
void f() {
    std::lock_guard<std::mutex> lk(m);     // destructor runs during unwinding
    auto p = std::make_unique<Data>();     // freed during unwinding
    risky();                               // if this throws, lk & p clean up
}
```

### The Exception Hierarchy

```text
std::exception
├── std::logic_error          (bugs: precondition violations)
│   ├── std::invalid_argument
│   ├── std::out_of_range      (e.g., vector::at, string::at)
│   └── std::length_error
└── std::runtime_error        (runtime conditions)
    ├── std::overflow_error
    ├── std::range_error
    └── std::system_error      (errno-based, filesystem, threads)
```

Always **catch by `const` reference** (`catch (const std::exception& e)`) to avoid
slicing and unnecessary copies.

---

## 3.2 The Three Exception-Safety Guarantees

This is the **most important interview topic** in this chapter. Every function
provides one of these guarantees:

### 1. No-throw Guarantee (Strongest)

The operation **never throws**. Required for destructors, `swap`, and move
operations. Mark with `noexcept`.

```cpp
void swap(T& a, T& b) noexcept;   // must not throw
~Resource() noexcept;             // destructors are implicitly noexcept
```

### 2. Strong Guarantee (Commit-or-Rollback)

If the operation throws, the program state is **exactly as it was before** —
transactional. Either it fully succeeds or has **no effect**.

```cpp
// std::vector::push_back gives the strong guarantee:
// if reallocation/copy throws, the vector is unchanged.
v.push_back(x);   // either x is added, or v is untouched
```

### 3. Basic Guarantee (Weakest Useful)

If the operation throws, the program remains in a **valid (but unspecified)**
state — no leaks, no corruption, all invariants hold, but values may have changed.

### No Guarantee (Unacceptable)

Throwing may leak resources or corrupt invariants. Modern C++ should never do this.

### Achieving the Strong Guarantee — Copy-and-Swap

```cpp
class Widget {
    std::vector<int> data_;
public:
    // Strong guarantee: do all the throwing work on a copy, then swap (noexcept)
    Widget& operator=(Widget other) {   // 'other' is a copy (may throw HERE, safely)
        swap(*this, other);             // swap never throws → commit point
        return *this;
    }                                   // old data destroyed with 'other'
    friend void swap(Widget& a, Widget& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
    }
};
```

If the copy throws, `*this` is untouched (strong guarantee). The swap is the
atomic "commit."

---

## 3.3 `noexcept` — Promising Not to Throw

### What It Is

`noexcept` declares that a function will not throw. If it does anyway,
`std::terminate()` is called immediately.

```cpp
void cleanup() noexcept;                 // promises not to throw
bool f() noexcept(condition);            // conditional noexcept
```

### Why It Matters — Performance & Correctness

1. **Move optimization:** `std::vector` only **moves** elements during
   reallocation if the move constructor is `noexcept`; otherwise it **copies** (to
   preserve the strong guarantee). See
   [C++11 move semantics](../CPP11/02_move_semantics.md).
2. **Optimization:** the compiler can omit unwinding machinery.
3. **Contract:** documents and enforces no-throw intent.

```cpp
class Buffer {
public:
    Buffer(Buffer&&) noexcept;            // ← noexcept enables vector to move
    Buffer& operator=(Buffer&&) noexcept;
};
```

> **Rule:** mark move constructors, move assignment, destructors, and `swap` as
> `noexcept` whenever they genuinely don't throw.

### Pitfall

Declaring `noexcept` and then throwing (directly or via a called function) calls
`std::terminate` — there's no recovery.

---

## 3.4 Exceptions vs Other Error Models

Exceptions are **not** the only — or always the best — way to report errors.

### Model Comparison

| Model | Best For | Pros | Cons |
|-------|----------|------|------|
| **Exceptions** | Rare, exceptional failures | Clean happy path; can't be ignored; carries context | Cost when thrown; control-flow can hide; banned in some domains |
| **Error codes** (`int`/`enum`/`std::error_code`) | Expected, frequent failures; C interop | Explicit; cheap; no unwinding | Easy to ignore; clutters signatures; manual propagation |
| **`std::optional<T>`** | "Value or nothing" (no error detail) | Type-safe; zero alloc; self-documenting | No failure *reason* |
| **`std::expected<T,E>`** (C++23) | "Value or error reason" | Explicit, composable, carries error | Newer; verbose without helpers |

### Examples

```cpp
// Exceptions — exceptional/unrecoverable
Config load(const std::string& path);  // throws on missing/corrupt file

// optional — simple presence (see C++17 book)
std::optional<int> to_int(std::string_view s);   // empty on failure

// error_code — expected, frequent, no exceptions
std::error_code ec;
auto size = std::filesystem::file_size(path, ec);
if (ec) { /* handle */ }

// expected (C++23) — value or rich error
std::expected<Config, ParseError> parse(std::string_view text);
```

### When to Choose Which

```text
Is failure rare AND unrecoverable here?        → exception
Is failure common / expected control flow?     → error_code / expected
Do you only need "value or nothing"?           → optional
Do you need the failure REASON, frequently?    → expected (or error_code)
Interfacing with C / no-exceptions build?      → error_code
```

> **Guideline:** use exceptions for *exceptional* conditions (truly unexpected,
> often unrecoverable locally). Use value-based errors (`optional`/`expected`/
> `error_code`) for **expected** outcomes that callers routinely handle.

---

## 3.5 Best Practices

```cpp
// 1. Throw by value, catch by const reference
throw std::runtime_error("msg");
catch (const std::exception& e) { }

// 2. Derive custom exceptions from std::exception
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& m) : std::runtime_error(m) {}
};

// 3. Never let exceptions escape a destructor
~Resource() noexcept {           // implicitly noexcept
    try { flush(); } catch (...) { /* log, swallow — never rethrow */ }
}

// 4. Use RAII for ALL resources so unwinding cleans up automatically
```

---

## Chapter 3 Cheat Sheet

```cpp
throw std::runtime_error("...");          // throw by value
catch (const std::exception& e) { }       // catch by const ref

// Three guarantees: no-throw (noexcept) > strong (commit/rollback) > basic (valid)
T& operator=(T other){ swap(*this,other); return *this; }  // copy-and-swap = strong

void move_op(T&&) noexcept;               // noexcept → containers move, not copy

// Error model selection
//   exception : rare/unrecoverable
//   error_code: frequent/expected, C interop
//   optional  : value-or-nothing
//   expected  : value-or-reason (C++23)
```

**Remember:**
- Stack unwinding + RAII = automatic cleanup on exceptions.
- Know the **three guarantees**; aim for strong via copy-and-swap where it matters.
- Mark moves/`swap`/destructors `noexcept` — it enables container optimizations.
- Exceptions for the *exceptional*; value-based errors for the *expected*.

---

## Common Mistakes

1. Catching by value (slicing) instead of `const&`.
2. Throwing from a destructor → `std::terminate` during unwinding.
3. Forgetting `noexcept` on moves → containers silently copy.
4. Using exceptions for ordinary, expected control flow (slow & unclear).
5. Manual `new`/`delete` around code that can throw → leaks (use RAII).

## When NOT to Use Exceptions

- **Hot loops / expected failures** — the throw path is costly; use return values.
- **Real-time / embedded / no-exceptions builds** — use `error_code`/`expected`.
- **Across ABI / C boundaries** — exceptions don't cross them safely.

---

*Next: [Chapter 4 — Design Patterns in Modern C++](04_design_patterns.md)*
