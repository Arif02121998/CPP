# Chapter 5 — Templates & Compile-Time Programming (C++11)

> *Level: Advanced*
> Back to [Table of Contents](README.md)

C++11 turned templates into a far more powerful, type-safe compile-time
programming tool.

---

## 5.1 Variadic Templates

### What It Is

A template that accepts **any number of arguments of any types** via a
**parameter pack** (`typename... Ts`).

```cpp
template <typename... Ts>          // Ts is a template parameter pack
void print(Ts... args);            // args is a function parameter pack

print(1, "hello", 3.14, 'x');      // any count, any types
```

### Why It Was Introduced

C++03 had no clean way to write functions/classes accepting a variable number of
**typed** arguments. The options were terrible:

### The Problem Before (C++03)

```cpp
// Option A: C-style varargs — NO type safety, error-prone
void log(const char* fmt, ...);    // printf-style; UB if types mismatch

// Option B: dozens of overloads for 1, 2, 3, ... arguments — explosion
template <class A> void f(A);
template <class A, class B> void f(A, B);
template <class A, class B, class C> void f(A, B, C);
// ...repeat forever...
```

### After (C++11)

```cpp
// One variadic template handles them all, fully type-safe
template <typename T>
void print(T value) { std::cout << value << '\n'; }          // base case

template <typename First, typename... Rest>
void print(First first, Rest... rest) {
    std::cout << first << ' ';
    print(rest...);                                          // recurse on the tail
}

print(1, "two", 3.0);   // prints: 1 two 3
```

### Key Tools

```cpp
sizeof...(Ts)      // number of types in the pack
sizeof...(args)    // number of function arguments
Ts...              // pack expansion (expand the pack here)
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Type safety** | Fully typed (unlike C varargs) |
| **Generality** | One definition for any arity |
| **Foundation** | Powers `make_shared`, `tuple`, `emplace`, `std::thread`, etc. |

### Real-World Use Cases

- `std::make_shared<T>(args...)` / `emplace_back(args...)` — forward constructor
  arguments.
- `std::tuple<Ts...>` — heterogeneous fixed-size collections.
- Type-safe variadic logging/formatting.

> C++17's **fold expressions** (see the [C++17 book](../CPP17/01_core_language_features.md))
> later removed the need for the recursive base-case pattern shown above.

---

## 5.2 Alias Templates (`using`)

### What It Is

A templated type alias — something `typedef` **could not** express.

```cpp
template <typename T>
using Vec = std::vector<T>;       // alias TEMPLATE

Vec<int> v;                       // = std::vector<int>

// Simplify scary types
template <typename T>
using StringMap = std::map<std::string, T>;
StringMap<int> counts;            // std::map<std::string, int>
```

### Before vs After

```cpp
// ─────────── C++03 — typedef can't be templated; needs a wrapper struct ───────────
template <typename T>
struct Vec { typedef std::vector<T> type; };
Vec<int>::type v;                 // clunky ::type

// ─────────── C++11 ───────────
template <typename T> using Vec = std::vector<T>;
Vec<int> v;                       // clean
```

This is also why the standard library added `_t` helpers
(`std::enable_if_t`, `std::remove_reference_t`, ...) — they're alias templates.

---

## 5.3 `constexpr` (C++11)

### What It Is

Marks functions/variables usable in **compile-time constant expressions**.

```cpp
constexpr int square(int x) { return x * x; }   // compile-time capable
constexpr int n = square(5);                    // computed at compile time (25)
int arr[square(4)];                             // 16 — used as an array bound
```

### Why It Was Introduced

C++03 forced **template metaprogramming** or **enum hacks** for compile-time
values — unreadable and limited.

### The C++11 Limitation (Important!)

C++11 `constexpr` functions are **severely restricted**: essentially a **single
`return`** — no loops, no local variables, no `if`/`switch`.

```cpp
// C++11: must be written recursively with a ternary
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);   // single return only
}
```

> **C++14 relaxed this dramatically** (loops, locals, branches). See the
> [C++14 book, Chapter 1](../CPP14/01_constexpr_and_variable_templates.md).

### How It Improves Things

- **Performance:** computation moves to compile time → zero runtime cost.
- **Safety:** results usable where the language requires constants (array sizes,
  template args, `static_assert`).

---

## 5.4 `static_assert` — Compile-Time Assertions

### What It Is

Checks a compile-time condition and **fails the build** with a custom message if
it's false.

```cpp
static_assert(sizeof(int) == 4, "This code assumes 32-bit int");

template <typename T>
void process(T) {
    static_assert(std::is_integral<T>::value,
                  "process() requires an integral type");
}
```

### Why It Matters

| Aspect | Improvement |
|--------|-------------|
| **Early errors** | Fails at **compile time**, not runtime |
| **Clear messages** | Custom text explains the requirement |
| **Constraints** | Enforces template type requirements (pre-concepts) |

Before C++11, people used obscure tricks (negative-size arrays, `BOOST_STATIC_ASSERT`)
to fake this.

---

## 5.5 `<type_traits>` — Compile-Time Type Information

### What It Is

A standard library of templates to **query and transform types** at compile time.

```cpp
#include <type_traits>

std::is_integral<int>::value;          // true
std::is_pointer<int*>::value;          // true
std::is_same<int, int>::value;         // true
std::is_base_of<Base, Derived>::value; // true

// Type transformations
std::remove_reference<int&>::type      // int
std::remove_const<const int>::type     // int
std::add_pointer<int>::type            // int*
```

### Use With `static_assert` and SFINAE

```cpp
template <typename T>
T half(T x) {
    static_assert(std::is_arithmetic<T>::value, "needs a number");
    return x / 2;
}

// SFINAE: enable a function only for integral types
template <typename T,
          typename = typename std::enable_if<std::is_integral<T>::value>::type>
void only_for_ints(T) { /* ... */ }
```

> Type traits + `static_assert` + `enable_if` were the C++11 way to constrain
> templates. C++17's `if constexpr` and C++20's **concepts** later made this far
> cleaner.

---

## 5.6 Other Compile-Time / Template Niceties

- **Default and deleted functions** (`= default` / `= delete`) — Chapter 1.
- **Extern templates** (`extern template class X<int>;`) — suppress redundant
  instantiations across translation units to cut build time.
- **Right-angle brackets** — `std::vector<std::vector<int>>` no longer needs a
  space between the `>>` (a C++03 parsing annoyance, now fixed).

---

## C++03 vs C++11 — Templates & constexpr Comparison

| Capability | C++03 | C++11 |
|-----------|:-----:|:-----:|
| Variadic templates | ❌ (varargs/overloads) | ✅ |
| Alias templates (`using`) | ❌ (`typedef` only) | ✅ |
| `constexpr` | ❌ (TMP/enum hacks) | ✅ (restricted) |
| `static_assert` | ❌ (tricks) | ✅ |
| `<type_traits>` | ❌ (Boost/TR1) | ✅ |
| `>>` without space | ❌ | ✅ |

---

## Chapter 5 Cheat Sheet

```cpp
// Variadic template (recursive style in C++11)
template <class T> void print(T x) { std::cout << x; }
template <class T, class... R> void print(T x, R... r){ std::cout<<x<<' '; print(r...); }
sizeof...(R);                          // pack size

// Alias template
template <class T> using Vec = std::vector<T>;

// constexpr (single-return in C++11)
constexpr int sq(int x) { return x * x; }

// static_assert + type_traits
static_assert(std::is_integral<T>::value, "need integral");
std::remove_reference<T>::type;        // strip the reference
```

**Remember:**
- Variadic templates = type-safe "any number of args"; foundation of `tuple`,
  `make_shared`, `emplace`.
- Alias templates beat `typedef` and enabled the standard `_t` helpers.
- C++11 `constexpr` is **single-return only** — C++14 relaxes it.
- `static_assert` + `<type_traits>` = the C++11 way to constrain templates.

---

## Common Mistakes

1. Forgetting the **base case** in a recursive variadic template (infinite/ill-formed).
2. Writing loops/locals in a C++11 `constexpr` function (not allowed until C++14).
3. Reaching for C-style varargs instead of variadic templates (loses type safety).
4. Overusing deep SFINAE where a `static_assert` message would be clearer.

## When NOT to Use

- **Variadic templates** when a simple overload or a container parameter is clearer.
- **`constexpr`** for logic that is inherently runtime — it adds restrictions for
  no benefit.
- **Heavy template metaprogramming** when readability matters more than the last
  bit of compile-time cleverness.

---

*Next: [Chapter 6 — Concurrency & the Memory Model](06_concurrency.md)*
