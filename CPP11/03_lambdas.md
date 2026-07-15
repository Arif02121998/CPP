# Chapter 3 — Lambda Expressions (C++11)

> *Level: Intermediate*
> Back to [Table of Contents](README.md)

---

## 3.1 What a Lambda Is

A **lambda expression** is an inline, anonymous function object (closure) you can
define right where you use it.

```cpp
auto greet = [] { std::cout << "hello\n"; };
greet();                          // calls it

auto add = [](int a, int b) { return a + b; };
int s = add(3, 4);                // 7
```

### Anatomy

```cpp
[capture](parameters) mutable -> return_type { body }
//  │         │          │          │           │
//  │         │          │          │           └─ statements
//  │         │          │          └─ optional explicit return type
//  │         │          └─ optional 'mutable' (modify captured copies)
//  │         └─ function parameters
//  └─ capture list (which outer variables the lambda can access)
```

---

## 3.2 Why Lambdas Were Introduced

### The C++03 Problem — Functors Everywhere

To pass custom behavior to an STL algorithm in C++03, you had to write a
**separate named function or functor struct**, often far from where it's used.

```cpp
// ─────────── C++03 ───────────
struct GreaterThan {
    int threshold;
    GreaterThan(int t) : threshold(t) {}
    bool operator()(int x) const { return x > threshold; }
};

int count = std::count_if(v.begin(), v.end(), GreaterThan(5));
// The logic lives in a struct elsewhere — verbose and disconnected.

// ─────────── C++11 ───────────
int count = std::count_if(v.begin(), v.end(),
                          [](int x) { return x > 5; }); // logic in place
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | Behavior defined exactly where it's used |
| **Locality** | No scattered functor structs/functions |
| **Capture** | Easily use surrounding variables (impossible with free functions) |

---

## 3.3 Captures — Using Outer Variables

The **capture list** controls how the lambda accesses variables from the
enclosing scope.

```cpp
int x = 10, y = 20;

[x]       { return x; };          // capture x BY VALUE (copy)
[&x]      { x = 5; };             // capture x BY REFERENCE
[x, &y]   { return x + y; };      // mix: x by value, y by reference
[=]       { return x + y; };      // capture ALL used vars by value
[&]       { x = y; };             // capture ALL used vars by reference
[=, &y]   { return x + y; };      // all by value, EXCEPT y by reference
[this]    { return member; };     // capture the enclosing object's 'this'
```

### By Value vs By Reference

```cpp
int counter = 0;

auto byVal = [counter] { return counter; };  // snapshots counter NOW (0)
auto byRef = [&counter] { return counter; }; // reads counter LATER (live)

counter = 99;
byVal();   // 0  — captured the old copy
byRef();   // 99 — reads the current value
```

---

## 3.4 `mutable` Lambdas

By default, the lambda's `operator()` is `const`, so **value-captured variables
are read-only** inside the body. `mutable` lets you modify the *copies*.

```cpp
int n = 0;
auto counter = [n]() mutable { return ++n; };  // modifies the captured copy
counter();  // 1
counter();  // 2
// the outer 'n' is still 0 — only the lambda's copy changed
```

---

## 3.5 Closures & `std::function`

Each lambda has a **unique, unnamable compiler-generated type**. Store it with
`auto`, or type-erase it into `std::function` when you need a uniform type
(e.g., a container of callbacks).

```cpp
auto f = [](int x) { return x * x; };     // best: exact type, zero overhead

std::function<int(int)> g = f;            // type-erased wrapper (small overhead)
std::vector<std::function<void()>> callbacks;
callbacks.push_back([]{ std::cout << "cb\n"; });
```

> Prefer `auto` for storing a single lambda. Use `std::function` only when you
> need a common type or to store heterogeneous callables together.

---

## 3.6 Real-World Use Cases

```cpp
// 1. STL algorithms
std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
auto n = std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; });

// 2. Custom deleters for smart pointers
std::unique_ptr<FILE, void(*)(FILE*)> fp(
    std::fopen("f.txt", "r"),
    [](FILE* f){ if (f) std::fclose(f); });

// 3. Callbacks / event handlers
button.onClick([&]{ std::cout << "clicked\n"; });

// 4. Scoped one-off helpers (avoid polluting the namespace)
auto clamp = [](int v, int lo, int hi){ return std::max(lo, std::min(v, hi)); };
```

---

## 3.7 Pitfalls

1. **Dangling captures with `[&]`.** A reference-capturing lambda that **outlives**
   the captured locals reads freed memory:
   ```cpp
   std::function<int()> makeBad() {
       int local = 42;
       return [&]{ return local; };   // DANGLING: local dies on return!
   }
   ```
   Capture by value (`[=]` / `[local]`) for anything that escapes the scope.

2. **`[=]` capturing `this` implicitly.** Inside a member function, `[=]`
   captures the **`this` pointer** (not a copy of the object). The lambda then
   depends on the object's lifetime. (C++11 has no `[*this]` copy — that's C++17.)

3. **Forgetting `mutable`** when you need to modify a value-captured variable.

4. **`std::function` overhead** — it may heap-allocate and adds an indirect call;
   don't use it where a plain lambda/`auto` suffices.

5. **C++11 lambdas are monomorphic** — parameters need explicit types.
   `auto` parameters (generic lambdas) and move-capture arrived in **C++14**
   (see the [C++14 book](../CPP14/02_lambda_improvements.md)).

---

## C++03 vs C++11 — Lambda Comparison

| Capability | C++03 | C++11 |
|-----------|:-----:|:-----:|
| Inline anonymous callables | ❌ (functors) | ✅ |
| Capture surrounding variables | ❌ | ✅ |
| Use directly in STL algorithms | ❌ (separate functor) | ✅ |
| `std::function` type erasure | ❌ | ✅ |
| Generic (`auto`) params | ❌ | ❌ (C++14) |
| Move capture | ❌ | ❌ (C++14) |

---

## Chapter 3 Cheat Sheet

```cpp
auto f = [](int x){ return x * x; };       // basic lambda

[x]    { };   // capture by value (copy/snapshot)
[&x]   { };   // capture by reference (live)
[=]    { };   // all used vars by value
[&]    { };   // all used vars by reference
[this] { };   // capture enclosing object pointer

auto c = [n = ...]() mutable { ++n; };     // (init capture is C++14)
auto g = [n]() mutable { return ++n; };    // mutable: modify captured copy

std::function<int(int)> fn = f;            // type-erased storage
```

**Remember:**
- `[=]` snapshots; `[&]` stays live (and risks dangling if it escapes).
- Add `mutable` to modify value-captured variables.
- Prefer `auto` to store a lambda; `std::function` only when you need one type.
- Generic lambdas & move capture are **C++14**, not C++11.

---

## Common Mistakes (Lambdas)

1. Returning a `[&]` lambda that captures locals by reference (dangling).
2. Expecting `[=]` inside a method to copy the object (it copies `this` pointer).
3. Forgetting `mutable` and getting a "cannot assign" error.
4. Overusing `std::function` where `auto` is faster and simpler.

## When NOT to Use

- **`[&]` capture** for callbacks/threads/async that outlive the current scope.
- **`std::function`** in performance-critical inner loops — prefer the concrete
  lambda type via `auto` or a template parameter.

---

*Next: [Chapter 4 — Smart Pointers & Memory Management](04_smart_pointers.md)*
