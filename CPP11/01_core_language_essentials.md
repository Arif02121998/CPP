# Chapter 1 — Core Language Essentials (C++11)

> *Level: Basic → Intermediate*
> Back to [Table of Contents](README.md)

This chapter covers the everyday syntax features that immediately make C++ code
cleaner and safer.

---

## 1.1 `auto` — Type Inference

### What It Is

`auto` tells the compiler to **deduce a variable's type** from its initializer.

```cpp
auto i = 42;                 // int
auto d = 3.14;               // double
auto s = std::string("hi");  // std::string
auto it = vec.begin();       // the iterator type — whatever it is
```

### Why It Was Introduced

C++03 type names were often **long and redundant**, especially iterators and
template-heavy types.

### Before vs After

```cpp
// ─────────── C++03 ───────────
std::map<std::string, std::vector<int>>::const_iterator it = m.begin();

// ─────────── C++11 ───────────
auto it = m.begin();         // same type, far less noise
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | Eliminates long, repeated type names |
| **Maintainability** | Change the container, the variable type follows |
| **Correctness** | Avoids accidental narrowing/implicit conversions |

### Deduction Rules (Important!)

`auto` uses **template-argument deduction**: it **strips top-level `const` and
references**.

```cpp
const int c = 0;
int& r = ...;
auto a = c;      // int  (const stripped)
auto b = r;      // int  (reference stripped)
auto& a2 = c;    // const int&  (explicitly request a reference)
const auto& a3 = something; // const ref — no copy
```

### Pitfalls

1. **Strips `const`/references** unless you add `&`/`const`.
2. **`auto` with braces:** `auto x{1};` deduces `int` (C++17 rule), but
   `auto x = {1};` deduces `std::initializer_list<int>` — a classic surprise.
3. Can **hide the type** where it matters for the reader — don't overuse in APIs.

---

## 1.2 `decltype` & Trailing Return Types

### What It Is

`decltype(expr)` yields the **exact declared type** of an expression, preserving
references and `const`.

```cpp
int x = 0;
decltype(x)   a;   // int
decltype((x)) b = x; // int&  (parenthesized lvalue → reference)
```

### Trailing Return Type

Lets a function's return type **depend on its parameters**, written after `->`.

```cpp
template <typename A, typename B>
auto add(A a, B b) -> decltype(a + b) {   // return type depends on a + b
    return a + b;
}
```

### Why It Was Introduced

In C++03 you couldn't express "the return type is whatever `a + b` produces"
because the parameters weren't in scope before the return type.

> C++14 later removed the need for the trailing `decltype` via return type
> deduction — see the [C++14 book](../CPP14/03_type_deduction.md).

---

## 1.3 `nullptr` — A Real Null Pointer

### What It Is

A keyword of type `std::nullptr_t` representing a null pointer — unambiguously
**not** an integer.

```cpp
int* p = nullptr;       // clearly a null pointer
```

### Why It Was Introduced — The `NULL` Problem

In C++03, `NULL` was just `0` (an `int`), causing **overload ambiguity**:

```cpp
void f(int);
void f(char*);

f(NULL);      // C++03: calls f(int)! — NULL is 0, not a pointer (surprise/bug)
f(nullptr);   // C++11: calls f(char*) — correct
```

### How It Improves Things

- **Removes ambiguity** between integer and pointer overloads.
- **Type-safe:** `nullptr` converts to any pointer type but **not** to `int`.
- **Template-friendly:** deduces as `std::nullptr_t`, not `int`.

---

## 1.4 Range-Based `for` Loop

### What It Is

Iterates directly over the elements of a range (container, array,
`initializer_list`) without explicit iterators or indices.

```cpp
std::vector<int> v{1, 2, 3};
for (int x : v)           std::cout << x;     // by value (copy)
for (const auto& x : v)   std::cout << x;     // by const ref (no copy)
for (auto& x : v)         x *= 2;             // by ref (modify in place)
```

### Before vs After

```cpp
// ─────────── C++03 ───────────
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it;

// ─────────── C++11 ───────────
for (auto x : v) std::cout << x;
```

### Pitfalls

- **`for (auto x : v)` copies** each element. Use `const auto&` to avoid copies.
- Modifying the container's size during iteration is undefined behavior.

---

## 1.5 Uniform / Brace Initialization

### What It Is

A single `{}` syntax to initialize **anything** — scalars, aggregates,
containers, and class types.

```cpp
int a{5};
std::vector<int> v{1, 2, 3};
std::map<int, std::string> m{{1, "a"}, {2, "b"}};
struct P { int x, y; };  P p{10, 20};
```

### Why It Was Introduced

C++03 had **inconsistent** initialization syntaxes (`()`, `=`, aggregate `{}`)
with confusing rules (e.g., the "most vexing parse").

### Key Benefits

- **Uniform** — one syntax everywhere.
- **Prevents narrowing** — `int x{3.14};` is an **error** (no silent truncation).
- **Solves the most vexing parse:** `Widget w{};` is an object, while
  `Widget w();` declares a function.

### Pitfall — `initializer_list` Hijacking

When a type has an `initializer_list` constructor, braces **prefer it**:

```cpp
std::vector<int> a(3);   // 3 elements, value 0
std::vector<int> b{3};   // ONE element, value 3   ← classic trap
std::vector<int> c(3, 5);// 3 elements, value 5
std::vector<int> d{3, 5};// TWO elements: 3 and 5
```

---

## 1.6 `enum class` — Scoped Enumerations

### What It Is

A strongly-typed, scoped enumeration that doesn't implicitly convert to `int`
and doesn't leak its enumerators into the surrounding scope.

```cpp
enum class Color { Red, Green, Blue };
Color c = Color::Red;       // must qualify with Color::

int n = c;                  // ERROR: no implicit conversion (safety!)
int n2 = static_cast<int>(c); // explicit conversion required
```

### Why It Was Introduced — Problems With Plain `enum`

```cpp
// ─────────── C++03 plain enum ───────────
enum Color { Red, Green, Blue };
enum Fruit { Apple, Banana, Red };   // ERROR: 'Red' already defined (name clash)

Color c = Red;
if (c == 1) {}              // compiles — enum implicitly converts to int (unsafe)
```

### How It Improves Things

- **Scoped:** no name clashes (`Color::Red` vs `TrafficLight::Red`).
- **Type-safe:** no accidental integer comparisons/conversions.
- **Explicit underlying type:** `enum class Status : uint8_t { ... };`.

---

## 1.7 Other Core Improvements

### `using` Type Aliases (and Alias Templates)

```cpp
// C++03 typedef
typedef std::map<std::string, int> StringIntMap;

// C++11 using — reads left-to-right, and supports templates
using StringIntMap = std::map<std::string, int>;

template <typename T>             // alias TEMPLATE — impossible with typedef
using Vec = std::vector<T>;
Vec<int> v;                       // std::vector<int>
```

### `override` and `final`

```cpp
struct Base { virtual void f(); virtual void g(); };
struct Derived : Base {
    void f() override;     // compiler verifies it really overrides (catches typos)
    void g() final;        // cannot be overridden further
};
struct Sealed final {};    // class cannot be inherited from
```

`override` catches a whole class of bugs where a signature mismatch silently
creates a *new* function instead of overriding.

### `= default` and `= delete`

```cpp
struct A {
    A() = default;                 // ask compiler for the default ctor
    A(const A&) = delete;          // forbid copying (e.g., non-copyable type)
    A& operator=(const A&) = delete;
};
```

### Raw and Unicode String Literals

```cpp
auto path = R"(C:\temp\new\file.txt)";   // raw string — no escaping backslashes
auto json = R"({"key": "value"})";       // great for regex/JSON/paths
auto u8   = u8"UTF-8 text";
auto u16  = u"UTF-16 text";
```

### `long long`, `static_assert`, `constexpr` (intro)

```cpp
long long big = 9'000'000'000LL;          // guaranteed ≥ 64-bit integer
static_assert(sizeof(void*) == 8, "64-bit only"); // compile-time assertion
constexpr int Max = 100;                  // compile-time constant (see Ch.5)
```

---

## C++03 vs C++11 — Core Language Comparison

| Feature | C++03 | C++11 |
|---------|:-----:|:-----:|
| Type inference (`auto`) | ❌ | ✅ |
| `decltype` / trailing return | ❌ | ✅ |
| `nullptr` | ❌ (`NULL`/`0`) | ✅ |
| Range-based `for` | ❌ | ✅ |
| Uniform init `{}` | partial (aggregates) | ✅ everywhere |
| `enum class` | ❌ | ✅ |
| Alias templates (`using`) | ❌ (`typedef` only) | ✅ |
| `override` / `final` | ❌ | ✅ |
| `= default` / `= delete` | ❌ | ✅ |
| Raw string literals | ❌ | ✅ |

---

## Chapter 1 Cheat Sheet

```cpp
auto it = m.begin();                 // type inference (strips const/&)
const auto& r = big;                 // no-copy reference
int* p = nullptr;                    // type-safe null
for (const auto& e : v) {}           // range-for, no copy
std::vector<int> v{1, 2, 3};         // uniform init (watch {n} trap)
enum class Color { Red, Green };     // scoped, type-safe enum
using Vec = std::vector<int>;        // modern alias (+ alias templates)
void f() override;                   // verified override
A(const A&) = delete;                // forbid an operation
auto s = R"(no\escapes)";            // raw string literal
```

**Remember:**
- `auto` strips top-level `const`/refs — add them back explicitly.
- `{n}` for a `vector` is **one** element, not `n` — beware initializer_list.
- `nullptr`, `enum class`, and `override` are pure safety wins — use them always.

---

## Common Mistakes (Core Language)

1. `std::vector<int> v{3};` expecting 3 elements (it's one element `3`).
2. Forgetting `auto` strips references → unintended copies.
3. Using `NULL`/`0` instead of `nullptr` in overloaded/template code.
4. `for (auto x : bigVec)` copying every element (use `const auto&`).

## When NOT to Use

- **`auto`** where an explicit type prevents a subtle wrong deduction or
  documents an API return type.
- **Brace init** with types that have an `initializer_list` constructor when you
  actually wanted the sized/other constructor — use `()` there.

---

*Next: [Chapter 2 — Move Semantics & Rvalue References](02_move_semantics.md)*
