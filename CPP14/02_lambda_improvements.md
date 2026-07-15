# Chapter 2 — Lambda Improvements

> *Level: Intermediate*
> Back to [Table of Contents](README.md)

C++11 introduced lambdas, but with two big inconveniences:
1. Parameters required **explicit types** (no `auto`).
2. You could only **capture by copy or by reference** — never **by move**.

C++14 fixes both.

---

## 2.1 Generic (Polymorphic) Lambdas

### What It Is

A **generic lambda** uses `auto` for one or more parameters. The compiler turns
each parameter into a template parameter of the lambda's hidden `operator()`.

```cpp
auto add = [](auto a, auto b) { return a + b; };
add(1, 2);            // int + int     -> 3
add(1.5, 2.5);        // double        -> 4.0
add(std::string("a"), "b"); // string + const char* -> "ab"
```

### Why It Was Introduced

In C++11, a lambda's parameter types were **fixed at the point of definition**.
To write a "works for any type" callable, you had to either:
- write a **hand-rolled function object (functor) with a template `operator()`**, or
- duplicate the lambda for each type.

### The C++11 Limitation — Before

```cpp
// C++11: a lambda is monomorphic; this only works for int
auto addInt = [](int a, int b) { return a + b; };

// To be generic in C++11 you needed a full functor:
struct Adder {
    template <typename A, typename B>
    auto operator()(A a, B b) const -> decltype(a + b) { return a + b; }
};
Adder add;            // verbose boilerplate
```

### After (C++14)

```cpp
auto add = [](auto a, auto b) { return a + b; }; // one line, fully generic
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | One-liner replaces a multi-line functor |
| **Maintainability** | No duplicated lambdas per type |
| **Reusability** | Same lambda works across the STL with any element type |

### Real-World Use Cases

- **STL algorithms over containers of varying type:**
  ```cpp
  std::sort(v.begin(), v.end(),
            [](const auto& a, const auto& b) { return a.id < b.id; });
  ```
- **Generic visitors** (precursor to `std::visit` overload sets in C++17).
- **`for_each` style transformations** without caring about the element type.

### Perfect Forwarding in Generic Lambdas

Combine `auto&&` with `std::forward` to forward arguments perfectly:

```cpp
auto invoke_log = [](auto&& f, auto&&... args) {
    std::cout << "calling...\n";
    return std::forward<decltype(f)>(f)(
        std::forward<decltype(args)>(args)...);
};
```

> `decltype(args)` recovers the exact deduced type so `std::forward` preserves
> value category (lvalue/rvalue).

### Pitfalls

- **Each distinct argument-type combination instantiates a new `operator()`** →
  potential **code bloat** in hot generic code.
- **Overload-resolution surprises:** `auto` accepts *anything*, so a generic
  lambda can silently swallow a type you didn't intend.
- Error messages can be **template-style verbose**.

---

## 2.2 Generalized Lambda Capture (Init Capture)

### What It Is

C++14 lets you **introduce new variables in the capture list** and initialize
them with an arbitrary expression. This is often called **init capture** and it
enables **capture by move**.

```cpp
auto ptr = std::make_unique<int>(42);
auto f = [p = std::move(ptr)]() { return *p; }; // move into the lambda
// ptr is now empty; the lambda owns the int
```

### Why It Was Introduced

C++11 capture lists could only **name existing variables**, captured by copy
(`[x]`) or by reference (`[&x]`). There was **no way to move** a variable into a
lambda. This was a serious problem for **move-only types** like
`std::unique_ptr` and `std::thread`.

### The C++11 Limitation — Before

```cpp
// C++11: cannot move a unique_ptr into a lambda
auto ptr = std::make_unique<int>(42);

// [ptr]  -> ERROR: tries to COPY unique_ptr (deleted copy ctor)
// [&ptr] -> dangerous: lambda holds a dangling reference if ptr dies
auto bad = [ptr] { return *ptr; }; // does NOT compile

// C++11 workaround: wrap in shared_ptr (extra heap + atomics) — ugh
auto sp = std::shared_ptr<int>(std::move(ptr));
auto ok = [sp] { return *sp; };    // works but heavier than needed
```

### After (C++14)

```cpp
auto ptr = std::make_unique<int>(42);
auto f = [p = std::move(ptr)] { return *p; }; // clean move capture
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Performance** | Avoids the shared_ptr workaround (no extra heap/atomics) |
| **Safety** | Lambda *owns* the resource → no dangling references |
| **Expressiveness** | Capture a *computed value*, rename, or transform on capture |

### More Than Just Move — Computed Captures

Init capture can hold **any expression**, not just moves:

```cpp
int x = 10;
auto f = [y = x + 1] { return y; };       // capture a computed value (11)

auto g = [n = compute_expensive()] { ... }; // capture once, reuse n
```

This is handy to **hoist an expensive computation out of the lambda body** so it
runs once at capture time rather than on every call.

### Real-World Use Cases

- **Moving `std::unique_ptr` / `std::thread` into a callback or task.**
- **Capturing the result of an expensive call once.**
- **Renaming captured members** for clarity, e.g. `[self = shared_from_this()]`
  to keep an object alive during async work (a *very* common async idiom).

```cpp
// Classic async lifetime-extension idiom
void Session::start() {
    socket.async_read(buffer,
        [self = shared_from_this()](auto ec, std::size_t n) {
            self->on_read(ec, n);   // 'self' keeps the Session alive
        });
}
```

### Pitfalls

1. **`[x = x]` shadows but does not modify the outer `x`.** The inner name is a
   *new* member of the closure.
2. **Init-captured members are `const` by default** (like copy captures). Add
   `mutable` to modify them:
   ```cpp
   auto counter = [n = 0]() mutable { return ++n; }; // need mutable
   ```
3. **A moved-from variable is left in a valid-but-unspecified state** — don't
   read it afterward.
4. **Be careful capturing `this` by value of a member** — `[self = *this]` copies
   the whole object; `[this]` captures the pointer (lifetime risk).

---

## 2.3 Combining Both Features

```cpp
// Generic + move capture together
auto make_logger = [](auto resource) {
    return [r = std::move(resource)](auto&&... args) {
        // use r and forward args generically
        (std::cout << ... << args); // (fold is C++17; shown for flavor)
    };
};
```

---

## C++11 vs C++14 — Lambda Comparison

| Capability | C++11 | C++14 |
|-----------|:-----:|:-----:|
| Capture by copy `[x]` | ✅ | ✅ |
| Capture by reference `[&x]` | ✅ | ✅ |
| `auto` parameters (generic) | ❌ | ✅ |
| Init capture `[y = expr]` | ❌ | ✅ |
| Capture by move | ❌ | ✅ |
| Computed/renamed captures | ❌ | ✅ |

---

## Chapter 2 Cheat Sheet

```cpp
// Generic lambda
auto add = [](auto a, auto b) { return a + b; };

// Perfect-forwarding generic lambda
auto fwd = [](auto&& f, auto&&... a) {
    return std::forward<decltype(f)>(f)(std::forward<decltype(a)>(a)...);
};

// Move capture
auto own = [p = std::move(uptr)] { return *p; };

// Computed capture (run once)
auto c = [v = expensive()] { return v; };

// Mutable init capture (stateful counter)
auto next = [n = 0]() mutable { return ++n; };
```

**Remember:**
- `auto` params → **one lambda, many types** (but watch code bloat).
- Init capture → **move-only types & computed captures**; default `const`,
  add `mutable` to modify.
- The `[self = shared_from_this()]` idiom keeps objects alive across async ops.

---

## Common Mistakes (Lambdas)

1. Forgetting `mutable` when you need to modify an init-captured value.
2. Capturing `[&]` everything and returning a lambda that **outlives** the locals.
3. Using a generic `auto` parameter where a concrete type would catch bugs.
4. Reading a variable after `std::move`-ing it into a capture.

## When NOT to Use

- **Don't use a generic lambda** if a single concrete type is expected — the
  explicit type documents intent and avoids accidental conversions.
- **Don't capture by reference** for anything that escapes the current scope
  (callbacks, threads, async) — prefer value or move capture.

---

*Next: [Chapter 3 — Type Deduction Upgrades](03_type_deduction.md)*
