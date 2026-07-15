# Chapter 2 — Templates & `constexpr` Improvements (C++17)

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

---

## 2.1 Class Template Argument Deduction (CTAD)

### What It Is

CTAD lets the compiler **deduce a class template's type arguments from the
constructor arguments**, so you can omit the `<...>`.

```cpp
std::pair p{1, 2.5};            // deduces std::pair<int, double>
std::vector v{1, 2, 3};         // deduces std::vector<int>
std::lock_guard lk{mtx};        // deduces std::lock_guard<std::mutex>
std::tuple t{1, 'a', 3.14};     // std::tuple<int, char, double>
```

### Why It Was Introduced

Function templates always deduced their arguments, but **class templates did
not**. This forced redundant **helper factory functions** like `std::make_pair`,
`std::make_tuple`, and verbose explicit type lists.

### Before vs After

```cpp
// ─────────── C++14 ───────────
auto p = std::make_pair(1, 2.5);        // helper needed
std::pair<int, double> q{1, 2.5};       // or spell out the types
std::lock_guard<std::mutex> lk{mtx};    // type repeated

// ─────────── C++17 ───────────
std::pair p{1, 2.5};                    // deduced
std::lock_guard lk{mtx};                // deduced
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | No redundant type lists |
| **Maintainability** | `make_*` helpers become unnecessary |
| **Consistency** | Class templates behave like function templates |

### Deduction Guides

When deduction isn't obvious, authors provide **deduction guides** to tell the
compiler how to map constructor args to template params:

```cpp
template <class T> struct Box { T value; };

// Deduction guide: Box(x) deduces Box<decltype(x)>
template <class T> Box(T) -> Box<T>;

Box b{42};   // Box<int> thanks to the guide
```

The standard library ships guides so, e.g., `std::vector v(first, last)` deduces
the element type from the iterators.

### Pitfalls

- CTAD uses **constructors** (and guides) — it can be **ambiguous** or surprising
  (`std::vector v{5}` is a 1-element vector `{5}`, not 5 default ints).
- Partial explicit args are **not** allowed: you can't write `std::pair<int>{...}`.
- May reduce clarity in public APIs where an explicit type documents intent.

---

## 2.2 `auto` Non-Type Template Parameters

### What It Is

A non-type template parameter (NTTP) can be declared `auto`, letting the
compiler **deduce its type** from the argument.

```cpp
template <auto N>
struct Constant { static constexpr auto value = N; };

Constant<42>     a;   // N is int
Constant<'x'>    b;   // N is char
Constant<10ULL>  c;   // N is unsigned long long
```

### Why It Was Introduced

Before C++17 you had to **specify the NTTP's type explicitly**, often with two
parameters (type + value):

```cpp
// ─────────── C++14 ───────────
template <typename T, T N>          // must pass the type AND the value
struct Constant { static constexpr T value = N; };
Constant<int, 42> a;                // redundant 'int'

// ─────────── C++17 ───────────
template <auto N>
struct Constant { static constexpr auto value = N; };
Constant<42> a;                     // type deduced
```

### Real-World Use Cases

- **Compile-time tag/dispatch types** parameterized by heterogeneous constants.
- **Type-safe enums as template parameters** without naming the enum type.
- Building **integral-constant-like** utilities generically.

---

## 2.3 `constexpr` Lambdas

### What It Is

In C++17, lambdas are **implicitly `constexpr`** when their body satisfies
`constexpr` rules, and you can mark them explicitly. They can be used in
constant expressions.

```cpp
auto square = [](int n) { return n * n; };       // implicitly constexpr
static_assert(square(5) == 25);                  // usable at compile time

constexpr auto cube = [](int n) { return n * n * n; }; // explicit
static_assert(cube(3) == 27);
```

### Why It Matters

Previously a lambda could **never** appear in a constant expression — you had to
write a named `constexpr` function. Now lambdas work uniformly in compile-time
and runtime contexts, composing nicely with `if constexpr`.

### Pitfall

The lambda only qualifies as `constexpr` if **everything it does** is valid in a
constant expression (no runtime-only calls, no static locals, etc.).

---

## 2.4 `std::void_t` & Detection Idiom

### What It Is

`std::void_t<...>` maps any well-formed type list to `void`. It powers the
**detection idiom** — checking whether an expression is valid for a type via
SFINAE, in a readable way.

```cpp
// Primary template: assume "no member type 'type'"
template <typename, typename = std::void_t<>>
struct has_value_type : std::false_type {};

// Specialization matches only if T::value_type is valid
template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>>
    : std::true_type {};

static_assert(has_value_type<std::vector<int>>::value);
static_assert(!has_value_type<int>::value);
```

### Why It Matters

Before `void_t`, trait detection required intricate SFINAE boilerplate. `void_t`
made the **detection idiom** the standard, teachable pattern (and paved the way
for C++20 concepts).

> In C++17 you'd often combine `if constexpr` + a `void_t`-based trait to write
> clean, type-aware generic code.

---

## 2.5 Other Template/`constexpr` Niceties

- **Fold expressions** (Chapter 1) are a template feature too.
- **`constexpr` for more of the standard library** — many algorithms and
  utilities became usable at compile time.
- **`template <auto>` + CTAD** together dramatically reduce boilerplate in
  metaprogramming.

---

## C++14 vs C++17 — Templates & `constexpr` Comparison

| Feature | C++14 | C++17 |
|---------|:-----:|:-----:|
| Class template arg deduction (CTAD) | ❌ (need `make_*`) | ✅ |
| `auto` non-type template params | ❌ | ✅ |
| `constexpr` lambdas | ❌ | ✅ |
| `std::void_t` / detection idiom | ❌ | ✅ |
| Deduction guides | ❌ | ✅ |
| Fold expressions | ❌ | ✅ |

---

## Chapter 2 Cheat Sheet

```cpp
std::pair p{1, 2.5};                  // CTAD: pair<int,double>
std::lock_guard lk{m};                // CTAD: lock_guard<mutex>

template <auto N> struct C { };       // auto NTTP
C<42> a; C<'x'> b;                     // type deduced per use

constexpr auto sq = [](int n){ return n*n; };
static_assert(sq(5) == 25);           // constexpr lambda

template <class, class = std::void_t<>>
struct has_x : std::false_type {};    // detection idiom skeleton
```

**Remember:**
- CTAD removes `make_pair`/`make_tuple`-style helpers.
- `template <auto>` deduces the constant's type — no more `<T, N>` pairs.
- Lambdas are now first-class in constant expressions.
- `std::void_t` is the standard tool for "does this type have member X?".

---

## Common Mistakes

1. `std::vector v{5}` → a 1-element vector, **not** five elements (CTAD + init list).
2. Trying `std::pair<int>{...}` — partial explicit args aren't allowed with CTAD.
3. Assuming any lambda is `constexpr` — it must obey `constexpr` rules.
4. Hand-rolling SFINAE when `std::void_t` would be clearer.

## When NOT to Use

- **CTAD** in public headers where the explicit type documents the API contract.
- **`auto` NTTP** when a single fixed type is clearer and self-documenting.

---

*Next: [Chapter 3 — Vocabulary Types](03_vocabulary_types.md)*
