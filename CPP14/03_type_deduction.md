# Chapter 3 — Type Deduction Upgrades

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

C++11 gave us `auto` (for variables) and `decltype` (to query an expression's
type). C++14 extends deduction into two new places:

1. **Function return type deduction** — `auto f() { ... }`
2. **`decltype(auto)`** — deduce while *preserving* references and cv-qualifiers.

---

## 3.1 Function Return Type Deduction

### What It Is

In C++14 a function can omit its return type and let the compiler deduce it from
the `return` statements, using plain `auto`:

```cpp
auto square(int x) { return x * x; }   // return type deduced as int
```

### Why It Was Introduced

In C++11, every function needed an **explicit return type**. For templates whose
return type depended on the arguments, you had to write a verbose **trailing
return type** with `decltype`.

### The C++11 Limitation — Before

```cpp
// C++11: must spell out the return type with a trailing decltype
template <typename A, typename B>
auto add(A a, B b) -> decltype(a + b) {   // trailing return required
    return a + b;
}
```

The `-> decltype(a + b)` clause **duplicates the expression** and adds noise.

### After (C++14)

```cpp
// C++14: just deduce it
template <typename A, typename B>
auto add(A a, B b) {
    return a + b;     // return type deduced — no trailing decltype
}
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | Removes redundant trailing `decltype` clauses |
| **Maintainability** | Change the body and the return type follows automatically |
| **DRY** | The return expression isn't written twice |

### Deduction Rules (Important!)

Return type deduction uses **template-argument deduction rules** — the same as
`auto x = expr;`. Crucially:

- **References are stripped.** `auto` deduces by value.
- **`const`/`volatile` (top-level) are stripped.**
- It deduces from the `return` expression's *decayed* type.

```cpp
int g();
auto f() { return g(); }     // returns int (by value)

int& ref();
auto h() { return ref(); }   // returns int (NOT int&!) — reference stripped
```

If you need the reference preserved, use `decltype(auto)` (next section).

### Multiple Returns Must Agree

All `return` statements must deduce to the **same type**:

```cpp
auto f(bool b) {
    if (b) return 1;        // int
    else   return 2.0;      // double  → ERROR: inconsistent deduction
}
```

### Recursion

A function with deduced return type can be recursive **only if** a return
statement is seen *before* the recursive call:

```cpp
auto fib(int n) {
    if (n < 2) return n;     // return type known here as int
    return fib(n - 1) + fib(n - 2); // OK — type already deduced
}
```

### Pitfalls

- **Return type is invisible at the declaration site.** For **public headers**,
  an explicit return type often documents the API better.
- **Requires the definition to be visible** to callers (can't forward-declare a
  deduced-return function and use it across TUs without the body).

---

## 3.2 `decltype(auto)`

### What It Is

`decltype(auto)` deduces a type using **`decltype` rules** instead of `auto`
rules. The key difference: it **preserves references and cv-qualifiers exactly**.

```cpp
int  x = 0;
int& rx = x;

auto           a = rx;   // int   (auto strips the reference)
decltype(auto) b = rx;   // int&  (decltype preserves the reference)
```

### Why It Was Introduced — The Forwarding Problem

Consider a generic wrapper that should return **exactly** what the underlying
call returns — value when it returns a value, reference when it returns a
reference. With plain `auto` you **lose references**; with a manual trailing
`decltype` you must repeat the (possibly huge) expression.

### Before vs After

```cpp
// ─────────── C++11 — repeat the expression in a trailing decltype ───────────
template <class Container, class Index>
auto element11(Container&& c, Index i)
    -> decltype(std::forward<Container>(c)[i])      // long, duplicated
{
    return std::forward<Container>(c)[i];           // written twice!
}

// ─────────── C++14 — decltype(auto) deduces & preserves exactly ───────────
template <class Container, class Index>
decltype(auto) element14(Container&& c, Index i) {
    return std::forward<Container>(c)[i];           // written once
}
```

If `c[i]` returns `int&`, `element14` returns `int&` (assignable). With plain
`auto` it would have returned `int` (a copy) — breaking `element14(v, 3) = 9;`.

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Correctness** | Preserves value category (lvalue/rvalue) and cv |
| **Generic code** | Essential for perfect-forwarding wrappers/accessors |
| **DRY** | No need to duplicate the return expression |

### The `(parentheses)` Gotcha — Classic Interview Trap

`decltype` treats a **bare name** and a **parenthesized name** differently:

```cpp
int x = 0;
decltype(x)    a;  // int   — x is a variable name
decltype((x))  b;  // int&  — (x) is an lvalue expression  → reference!
```

So in a function:

```cpp
decltype(auto) f1() { int x = 0; return x;   } // returns int  (fine)
decltype(auto) f2() { int x = 0; return (x); } // returns int& → DANGLING!
```

`f2` returns a **reference to a local** that is destroyed on return — undefined
behavior. This is the single most famous `decltype(auto)` pitfall.

### Real-World Use Cases

- **Perfect-forwarding accessors** (e.g., a `get()` that returns `T&`, `const T&`,
  or `T&&` matching the container).
- **Wrapper/decorator functions** that must transparently relay the wrapped
  function's exact return type.
- **`std::forward`-style relays** in library code.

---

## 3.3 `auto` vs `decltype(auto)` Decision Guide

```text
Need a copy / don't care about references?          → use auto
Need to PRESERVE references & cv exactly?           → use decltype(auto)
Returning a value computed locally?                 → use auto (safer)
Relaying another function's/container's result?     → use decltype(auto)
```

| Situation | `auto` | `decltype(auto)` |
|-----------|:------:|:----------------:|
| Strips top-level references | ✅ | ❌ (preserves) |
| Strips top-level cv | ✅ | ❌ (preserves) |
| Safe default for local results | ✅ | ⚠️ risk of dangling |
| Perfect-forwarding return | ❌ | ✅ |

---

## C++11 vs C++14 — Deduction Comparison

| Capability | C++11 | C++14 |
|-----------|:-----:|:-----:|
| `auto` variables | ✅ | ✅ |
| `decltype(expr)` | ✅ | ✅ |
| Trailing return type `-> decltype` | ✅ | ✅ |
| `auto` function return deduction | ❌ | ✅ |
| `decltype(auto)` | ❌ | ✅ |
| Deduced return in lambdas (multi-statement) | limited | ✅ |

---

## Chapter 3 Cheat Sheet

```cpp
// Return type deduction (auto = by value, strips refs/cv)
auto add(int a, int b) { return a + b; }

// Preserve exact type (refs + cv) for forwarding
template <class C, class I>
decltype(auto) at(C&& c, I i) { return std::forward<C>(c)[i]; }

// THE trap: parentheses change decltype's result
int x;
decltype(x)   // int
decltype((x)) // int&  → never `return (local);` from decltype(auto)!
```

**Remember:**
- `auto` return = **value semantics** (refs/cv stripped) — safe default.
- `decltype(auto)` = **exact type** (refs/cv preserved) — for forwarding.
- **Never** write `return (local);` in a `decltype(auto)` function.

---

## Common Mistakes (Type Deduction)

1. Returning `int` by accident from `auto f()` when you needed `int&`.
2. `return (x);` in a `decltype(auto)` function → dangling reference.
3. Inconsistent return types across multiple `return`s.
4. Hiding return types in **public headers** where an explicit type is clearer.

## When NOT to Use

- **Public API headers** — prefer explicit return types for documentation.
- **When a copy is what you want** — `decltype(auto)` may surprise you with a
  reference you didn't intend.

---

*Next: [Chapter 4 — Library & STL Additions](04_library_and_misc.md)*
