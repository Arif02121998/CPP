# Chapter 6 — Move & Forwarding, In Depth

> *Level: Advanced*
> Back to [Table of Contents](README.md)
> Builds on [C++11 move semantics](../CPP11/02_move_semantics.md) and
> [C++17 guaranteed copy elision](../CPP17/01_core_language_features.md).

The C++11 book introduced move semantics. This chapter goes **under the hood**:
the full value-category model, exactly how forwarding references and reference
collapsing work, and where perfect forwarding breaks.

---

## 6.1 Value Categories — The Complete Picture

Every C++ expression has both a **type** and a **value category**. C++11
refined the model into a hierarchy:

```text
            expression
           /          \
      glvalue        rvalue
      /     \       /     \
 lvalue   xvalue        prvalue
            \   /
          (xvalue is both glvalue and rvalue)
```

### The Three Primary Categories

| Category | Stands For | Has identity? | Movable? | Examples |
|----------|-----------|:-------------:|:--------:|----------|
| **lvalue** | locator value | ✅ yes | ❌ no | `x`, `*p`, `arr[i]`, a named variable |
| **prvalue** | pure rvalue | ❌ no | ✅ yes | `42`, `a + b`, `f()` returning by value |
| **xvalue** | "eXpiring" value | ✅ yes | ✅ yes | `std::move(x)`, `static_cast<T&&>(x)` |

### Composite Categories

- **glvalue** ("generalized lvalue") = lvalue **or** xvalue → has identity.
- **rvalue** = prvalue **or** xvalue → can be moved from.

### Why This Matters

- **lvalue** → bind to `T&`; copy from it (it persists).
- **rvalue** (prvalue/xvalue) → bind to `T&&`; **move** from it (it's expiring).
- `std::move(x)` turns an lvalue into an **xvalue** so a move can occur — `x`
  still exists afterward (valid but unspecified).

```cpp
int x = 5;
int y = x;              // x is an lvalue → copy
int z = x + 1;          // (x + 1) is a prvalue → no object to take address of
int w = std::move(x);   // std::move(x) is an xvalue → enables move (x persists)
```

---

## 6.2 Reference Binding Rules

```cpp
void f(int&);        // lvalue reference
void f(const int&);  // const lvalue reference — binds to almost anything
void f(int&&);       // rvalue reference

int a = 1; const int b = 2;

f(a);            // int&        (a is a non-const lvalue)
f(b);            // const int&  (b is const)
f(3);            // int&&       (3 is a prvalue) — if no int&&, falls to const int&
f(std::move(a)); // int&&       (xvalue)
```

Key rule: a **non-const lvalue reference cannot bind to an rvalue**; a **const
lvalue reference can bind to everything** (which is why `const T&` parameters
accept temporaries).

---

## 6.3 Forwarding (Universal) References & Reference Collapsing

### Forwarding Reference — Definition

`T&&` is a **forwarding reference** *only* when `T` is a **deduced template
parameter** (or `auto&&`). It binds to **both** lvalues and rvalues, capturing
which one it received.

```cpp
template <typename T>
void g(T&& x);          // forwarding reference (T is deduced)

void h(std::string&& x);// NOT forwarding — concrete type → plain rvalue reference

int n = 0;
g(n);   // T = int&,  x : int&   (lvalue)
g(5);   // T = int,   x : int&&  (rvalue)
```

### How Deduction Works Here

- Pass an **lvalue** of type `U` → `T` deduces to `U&`.
- Pass an **rvalue** of type `U` → `T` deduces to `U`.

### Reference Collapsing

When references-to-references arise during substitution, they "collapse":

| You write | Collapses to |
|-----------|:------------:|
| `T& &` | `T&` |
| `T& &&` | `T&` |
| `T&& &` | `T&` |
| `T&& &&` | `T&&` |

**Mnemonic:** *"If either is an lvalue reference, the result is an lvalue
reference. Only `&& &&` yields `&&`."*

```cpp
// g(n): T = int&  → parameter type int& && → collapses to int&
// g(5): T = int   → parameter type int&&    → stays int&&
```

---

## 6.4 `std::move` vs `std::forward` — Internals

### `std::move` — Unconditional Cast to Rvalue

```cpp
template <typename T>
constexpr std::remove_reference_t<T>&& move(T&& x) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(x);
}
```

It **always** casts to an rvalue reference — use it when you **own** a value and
want to relinquish it.

### `std::forward` — Conditional Cast

```cpp
template <typename T>
constexpr T&& forward(std::remove_reference_t<T>& x) noexcept {
    return static_cast<T&&>(x);   // collapses to T& for lvalues, T&& for rvalues
}
```

It casts to rvalue **only if** the original argument was an rvalue — use it on a
**forwarding reference** to preserve the caller's value category.

### Side-by-Side

| | `std::move` | `std::forward<T>` |
|--|-------------|-------------------|
| Cast | Always to rvalue | Conditional (rvalue iff original was) |
| Use on | A value you own | A forwarding reference parameter |
| Purpose | Enable a move | Preserve value category (perfect forwarding) |

```cpp
template <typename T>
void wrapper(T&& arg) {
    // std::move(arg) would ALWAYS move — wrong if caller passed an lvalue!
    inner(std::forward<T>(arg));   // correct: lvalues stay lvalues, rvalues move
}
```

---

## 6.5 Perfect Forwarding

### The Goal

A wrapper that relays its arguments to another function **without changing their
value category, constness, or type** — so the target sees exactly what the
original caller passed.

```cpp
template <typename... Args>
std::unique_ptr<Widget> makeWidget(Args&&... args) {
    return std::unique_ptr<Widget>(new Widget(std::forward<Args>(args)...));
}
```

This is how `std::make_unique`, `std::make_shared`, `emplace_back`, and
`std::thread`'s constructor relay arguments to the real constructor.

### Why Both `&&` and `forward` Are Required

- `Args&&...` — forwarding references **capture** the value category.
- `std::forward<Args>(args)...` — **restores** it at the call site.

Drop either and you lose move-ness (forwarding refs without `forward` always pass
lvalues; `forward` without forwarding refs has nothing correct to forward).

---

## 6.6 Perfect Forwarding Failure Cases

Perfect forwarding is *almost* perfect. Known cases where it breaks:

1. **Braced initializers** — `{1, 2, 3}` has no type, so `T` can't deduce.
   ```cpp
   template <class T> void f(T&&);
   f({1, 2, 3});           // ERROR: cannot deduce T from a braced-init-list
   std::vector<int> v{1,2,3}; f(v);   // OK — give it a named object
   ```

2. **`0` / `NULL` as null pointers** — deduced as integer types, not pointers
   (use `nullptr`).

3. **Overloaded function names / function templates** — ambiguous; `T` can't pick
   one. Cast to the desired signature first.

4. **`static const` integral members without a definition** — odr-use through a
   forwarding reference may need an out-of-line definition (pre-C++17).

5. **Bitfields** — can't bind a non-const reference to a bitfield.

---

## 6.7 Copy Elision & Guaranteed Elision (C++17)

### What It Is

The compiler constructs a returned temporary **directly** in the caller's storage,
skipping the copy/move entirely.

```cpp
std::string make() { return std::string("hi"); }
std::string s = make();   // no copy, no move — built in place
```

- **Pre-C++17:** an *optional* optimization (RVO/NRVO) — the copy/move ctor still
  had to be **accessible**.
- **C++17:** **guaranteed** for prvalues — you can return **non-movable**,
  **non-copyable** types by value. See
  [C++17 Ch.1](../CPP17/01_core_language_features.md).

### Why `return std::move(local)` Is an Anti-Pattern

```cpp
std::string bad() {
    std::string s = "hi";
    return std::move(s);   // ← DISABLES NRVO; forces a move instead of elision
}
std::string good() {
    std::string s = "hi";
    return s;              // ← NRVO may elide entirely (or move) — let the compiler
}
```

Adding `std::move` on the return turns a potential **zero-cost elision** into a
mandatory move. **Just `return local;`.**

---

## Chapter 6 Cheat Sheet

```cpp
// Value categories
//   lvalue  : named, has identity, not movable     (x, *p, arr[i])
//   prvalue : temporary, movable                    (42, a+b, f())
//   xvalue  : expiring, has identity + movable       (std::move(x))

// Forwarding reference (T deduced) binds to both:
template <class T> void g(T&& x);   // lvalue → T=U&,  rvalue → T=U

// Reference collapsing: only && && → &&; anything with & → &

// move = unconditional rvalue cast (own value)
// forward<T> = conditional cast (forwarding reference) → preserve category
template <class... A> auto make(A&&... a){ return T(std::forward<A>(a)...); }

// Just `return local;` — never `return std::move(local);` (kills elision)
```

**Remember:**
- 3 primary categories: **lvalue / prvalue / xvalue**; rvalue = prvalue ∪ xvalue.
- `T&&` is a forwarding reference **only** when `T` is deduced.
- Collapsing: any `&` involved → `&`; only `&& &&` → `&&`.
- `move` = always; `forward` = conditional; use `forward` for forwarding refs.
- Perfect forwarding fails on braced lists, `0`/`NULL`, overload names, bitfields.
- `return local;` (not `std::move`) to keep guaranteed/named elision.

---

## Common Mistakes

1. Using `std::move` on a forwarding-reference parameter (always moves — wrong
   for lvalue callers). Use `std::forward`.
2. `return std::move(local);` → defeats copy elision.
3. Forwarding `{1,2,3}` (no deducible type).
4. Thinking `std::string&&` (concrete) is a forwarding reference (it isn't).
5. Reading a moved-from object's value (valid but unspecified).

## When NOT to Use

- **Forwarding references** for parameters you don't actually relay — they greedily
  match everything and can hijack overload resolution; a `const T&` is often
  clearer.
- **`std::move`** on a value you still need, or on a `const` object (copies).

---

*Next: [Chapter 7 — Interview Preparation Bank](07_interview_questions.md)*
