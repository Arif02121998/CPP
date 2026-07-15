# Chapter 1 — Core Language Features (C++17)

> *Level: Basic → Advanced*
> Back to [Table of Contents](README.md)

---

## 1.1 Structured Bindings

### What It Is

Structured bindings let you **decompose** a `pair`, `tuple`, array, or simple
struct into **named variables** in one declaration.

```cpp
std::map<std::string, int> m;
auto [key, value] = *m.begin();   // key, value instead of .first/.second
```

### Why It Was Introduced

Accessing tuple/pair members via `.first`, `.second`, or `std::get<0>` was
**unreadable**, and `std::tie` required **pre-declaring** the variables.

### Before vs After

```cpp
// ─────────── C++14 ───────────
auto result = mymap.insert({key, val});
if (result.second)            // what is .second? (it's "inserted")
    use(result.first);        // what is .first? (it's the iterator)

// C++14 with std::tie — must pre-declare, can't use auto types
std::map<K,V>::iterator it; bool inserted;
std::tie(it, inserted) = mymap.insert({key, val});

// ─────────── C++17 ───────────
auto [it, inserted] = mymap.insert({key, val}); // self-documenting!
if (inserted) use(it);
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | Meaningful names instead of `.first`/`.second`/`get<N>` |
| **Conciseness** | One line; no pre-declaration like `std::tie` |
| **Safety** | Harder to mix up tuple positions |

### Works With

```cpp
// 1. Arrays
int arr[3] = {1, 2, 3};
auto [a, b, c] = arr;

// 2. Structs (public data members, in order)
struct Point { int x, y; };
Point p{10, 20};
auto [px, py] = p;

// 3. Tuples / pairs
auto [i, d, s] = std::make_tuple(1, 2.0, "x");

// 4. Range-for over a map (extremely common)
for (const auto& [name, score] : scores)
    std::cout << name << ": " << score << '\n';
```

### Reference & const Bindings

```cpp
auto [x, y] = p;    // copies the struct, then binds names to the copy
auto& [x, y] = p;   // binds names to p's members (modifies p)
const auto& [x, y] = p; // read-only references, no copy
```

### Pitfalls

- `auto [a, b] = ...;` **copies** the source. Use `auto&` to avoid the copy /
  to modify the original.
- The **number of names must match** the number of members exactly.
- Cannot **skip** members (no `_` placeholder; you must name all of them).
- Can't apply `constexpr` to a structured binding declaration.

---

## 1.2 `if` / `switch` with Initializer

### What It Is

You can declare a variable **inside** an `if`/`switch`, scoped to that statement
(and its `else`), separated by `;`.

```cpp
if (auto it = m.find(key); it != m.end()) {
    use(it->second);          // 'it' visible here
} else {
    // 'it' still visible here
}
// 'it' is OUT of scope here — no leakage
```

### Why It Was Introduced

Previously, to scope a variable to an `if` you wrapped it in an extra block, or
you let it **leak** into the surrounding scope.

### Before vs After

```cpp
// ─────────── C++14 ───────────
{
    auto it = m.find(key);    // extra braces just to scope 'it'
    if (it != m.end()) use(it->second);
}   // or worse: let 'it' leak into the whole function

// ─────────── C++17 ───────────
if (auto it = m.find(key); it != m.end())
    use(it->second);          // 'it' scoped exactly to the if/else
```

### How It Improves Things

- **Tighter scope** → fewer name collisions, clearer lifetime.
- **Readability** → the lookup and the test live together.

### Real-World Use Cases

- Map/set lookups (`find` + check).
- Locking + checking: `if (std::lock_guard lk(m); ready) { ... }`.
- Parsing: `if (auto [ok, val] = parse(s); ok) { ... }`.

---

## 1.3 `if constexpr` — Compile-Time Branching

### What It Is

`if constexpr (cond)` evaluates `cond` **at compile time** and **discards the
untaken branch entirely** — it isn't even compiled/instantiated.

```cpp
template <typename T>
auto to_string_generic(T value) {
    if constexpr (std::is_arithmetic_v<T>)
        return std::to_string(value);     // only compiled for arithmetic T
    else
        return std::string(value);        // only compiled otherwise
}
```

### Why It Was Introduced — The Big One

Before C++17, writing code that differed **based on a type trait** required
**tag dispatch** or **SFINAE** — both verbose and hard to read.

### Before vs After

```cpp
// ─────────── C++14 — tag dispatch ───────────
template <typename T>
std::string impl(T v, std::true_type)  { return std::to_string(v); }
template <typename T>
std::string impl(T v, std::false_type) { return std::string(v); }

template <typename T>
std::string to_str(T v) {
    return impl(v, std::is_arithmetic<T>{}); // dispatch on a tag type
}

// ─────────── C++17 — if constexpr ───────────
template <typename T>
std::string to_str(T v) {
    if constexpr (std::is_arithmetic_v<T>)
        return std::to_string(v);
    else
        return std::string(v);
}
```

### Why the "Discarded Branch" Matters

In a normal `if`, **both** branches must compile for every `T`. With
`if constexpr`, the false branch is **discarded**, so it may contain code that
would be **ill-formed** for that `T`:

```cpp
template <typename T>
void process(T t) {
    if constexpr (std::is_pointer_v<T>)
        use(*t);          // dereference — only valid when T is a pointer
    else
        use(t);           // valid for non-pointers
}
```
A regular `if` would fail to compile `*t` when `T` is not a pointer.

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | Linear code replaces tag dispatch / SFINAE |
| **Maintainability** | All logic in one function |
| **Compile size** | Untaken branch not instantiated → less bloat |

### Pitfalls

- The condition must be a **compile-time constant** (`constexpr`).
- In a **non-template** context, both branches must still be valid (the
  type-dependent magic only applies inside templates).
- Each branch's `return` can yield a **different type** (unlike runtime `if`).

---

## 1.4 Fold Expressions

### What It Is

A concise way to **apply a binary operator across a parameter pack** — no
recursion needed.

```cpp
template <typename... Ts>
auto sum(Ts... xs) { return (xs + ...); }   // (x1 + (x2 + (x3 + ...)))

sum(1, 2, 3, 4);   // 10
```

### Why It Was Introduced

C++11/14 variadic packs required **recursive template instantiation** with a
base case — verbose and slow to compile.

### Before vs After

```cpp
// ─────────── C++14 — recursive variadic ───────────
auto sum() { return 0; }                       // base case
template <typename T, typename... Rest>
auto sum(T first, Rest... rest) {
    return first + sum(rest...);               // recursion
}

// ─────────── C++17 — fold expression ───────────
template <typename... Ts>
auto sum(Ts... xs) { return (xs + ...); }      // one line, no base case
```

### The Four Fold Forms

```cpp
(pack op ...)         // unary right fold:  (x1 op (x2 op (... op xn)))
(... op pack)         // unary left  fold:  (((x1 op x2) op ...) op xn)
(pack op ... op init) // binary right fold (with init value)
(init op ... op pack) // binary left  fold (with init value)
```

### Useful Patterns

```cpp
// Print all args (comma operator fold)
template <class... Ts>
void print(Ts... xs) { ((std::cout << xs << ' '), ...); }

// Logical AND over a pack
template <class... Ts>
bool all_true(Ts... xs) { return (xs && ...); }

// Push many items into a vector
template <class... Ts>
void push_all(std::vector<int>& v, Ts... xs) { (v.push_back(xs), ...); }
```

### Pitfall — Empty Packs

Only `&&` (→ `true`), `||` (→ `false`), and `,` (→ `void()`) have defined
results for an **empty** pack. Other operators on an empty pack are ill-formed,
so use the **binary** form with an explicit init value:

```cpp
template <class... Ts>
auto sum(Ts... xs) { return (0 + ... + xs); } // safe even with zero args
```

---

## 1.5 Inline Variables

### What It Is

`inline` can now apply to **variables**, allowing a single definition of a
global/`static` member to live in a **header** without ODR violations.

```cpp
// In a header — included by many .cpp files, but ONE shared instance
struct Config {
    inline static int instances = 0;     // define in-class, no .cpp needed
};

inline constexpr double kPi = 3.14159265358979;  // header-only constant
```

### Why It Was Introduced

Before C++17, a `static` data member needed a **separate definition in one .cpp**
file; header-only libraries had to use awkward workarounds.

### Before vs After

```cpp
// ─────────── C++14 ───────────
// widget.h
struct Widget { static int count; };
// widget.cpp  (REQUIRED separate definition)
int Widget::count = 0;

// ─────────── C++17 ───────────
// widget.h only — no .cpp needed
struct Widget { inline static int count = 0; };
```

This is a big win for **header-only libraries**.

---

## 1.6 Guaranteed Copy Elision

### What It Is

C++17 **mandates** elision of the copy/move when returning a **prvalue** (a
temporary). It's no longer an optimization — the object is constructed directly
in its destination.

```cpp
struct NonMovable {
    NonMovable() = default;
    NonMovable(const NonMovable&) = delete;
    NonMovable(NonMovable&&)      = delete;
};

NonMovable make() { return NonMovable{}; } // OK in C++17, ill-formed pre-C++17
NonMovable x = make();                      // no copy/move required at all
```

### Why It Matters

- You can **return non-copyable, non-movable types by value**.
- `factory()` patterns are guaranteed zero-overhead.
- Pre-C++17 this relied on the optional RVO; now it's part of the language.

---

## 1.7 Smaller but Handy Features

### Nested Namespace Definition

```cpp
// C++14
namespace a { namespace b { namespace c { /* ... */ } } }
// C++17
namespace a::b::c { /* ... */ }
```

### `[[nodiscard]]`

Warns if a function's return value is ignored — great for error codes and
factory functions.

```cpp
[[nodiscard]] bool did_succeed();
did_succeed();          // warning: return value ignored
```

### `[[maybe_unused]]`

Suppresses "unused variable/parameter" warnings (e.g., used only in asserts).

```cpp
[[maybe_unused]] int debug_id = compute();
```

### `[[fallthrough]]`

Documents an intentional `switch` fall-through and silences the warning.

```cpp
switch (x) {
    case 1: do1(); [[fallthrough]];
    case 2: do2(); break;
}
```

### `__has_include`

Preprocessor check for header availability — portable feature detection.

```cpp
#if __has_include(<optional>)
  #include <optional>
#endif
```

---

## C++14 vs C++17 — Core Language Comparison

| Feature | C++14 | C++17 |
|---------|:-----:|:-----:|
| Structured bindings | ❌ | ✅ |
| `if`/`switch` with init | ❌ | ✅ |
| `if constexpr` | ❌ (tag dispatch/SFINAE) | ✅ |
| Fold expressions | ❌ (recursion) | ✅ |
| Inline variables | ❌ | ✅ |
| Guaranteed copy elision | ❌ (optional RVO) | ✅ |
| `namespace a::b::c` | ❌ | ✅ |
| `[[nodiscard]]`/`[[maybe_unused]]`/`[[fallthrough]]` | ❌ | ✅ |

---

## Chapter 1 Cheat Sheet

```cpp
auto [a, b] = pair;                       // structured binding (copies)
for (auto& [k, v] : map) {}               // bind references in range-for

if (auto it = m.find(k); it != m.end()) {}// if with initializer

if constexpr (std::is_integral_v<T>) {}   // compile-time branch (discards other)

auto s = (xs + ...);                      // fold expression
auto safe = (0 + ... + xs);               // binary fold (empty-pack safe)

inline constexpr int kMax = 100;          // header-only variable

namespace a::b::c {}                       // nested namespace
[[nodiscard]] int must_check();            // warn if ignored
```

**Remember:**
- `auto [..]` copies; use `auto&` for references.
- `if constexpr` discards the untaken branch — enables type-specific code.
- Fold expressions replace recursive variadic templates.
- Inline variables make header-only constants/statics trivial.

---

## Common Mistakes (Core Language)

1. Expecting `auto [a,b]` to reference the source (it copies).
2. Using a runtime `if` where `if constexpr` is needed for type-dependent code.
3. Folding over an empty pack with `+`/`*` (use the binary form with init).
4. Forgetting that `if`-init variables are scoped to the whole `if/else`.

## When NOT to Use

- **Structured bindings** for types with many members where positional names
  hurt clarity — a named accessor may read better.
- **`if constexpr`** when a plain runtime `if` is sufficient — don't over-template.

---

*Next: [Chapter 2 — Templates & `constexpr` Improvements](02_templates_and_constexpr.md)*
