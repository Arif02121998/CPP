# Chapter 1 — Advanced Templates & Metaprogramming

> *Level: Advanced*
> Back to [Table of Contents](README.md)
> Prerequisite: function & basic class templates.
> Builds on [C++11 templates](../CPP11/05_templates_and_constexpr.md) and
> [C++17 `if constexpr`/`void_t`](../CPP17/02_templates_and_constexpr.md).

This is where templates stop being "generic functions" and become a
**compile-time programming language**.

---

## 1.1 Template Specialization

### What It Is

Specialization lets you provide a **different implementation** of a template for
specific types — overriding the generic version.

### Full (Explicit) Specialization

```cpp
// Primary (generic) template
template <typename T>
struct Serializer {
    static std::string to_string(const T& v) { return std::to_string(v); }
};

// Full specialization for std::string (a completely different implementation)
template <>
struct Serializer<std::string> {
    static std::string to_string(const std::string& v) { return "\"" + v + "\""; }
};

Serializer<int>::to_string(42);          // "42"
Serializer<std::string>::to_string("x"); // "\"x\""
```

### Partial Specialization (Classes Only)

You can specialize for a **family** of types (e.g., "any pointer", "any
`vector<T>`"). **Only class templates** can be partially specialized — functions
cannot (use overloading instead).

```cpp
template <typename T>
struct TypeInfo { static constexpr bool is_pointer = false; };

// Partial specialization: matches ANY pointer type T*
template <typename T>
struct TypeInfo<T*> { static constexpr bool is_pointer = true; };

TypeInfo<int>::is_pointer;   // false
TypeInfo<int*>::is_pointer;  // true
```

### Why It Matters

| Use | Example |
|-----|---------|
| Optimize for a specific type | A faster algorithm for `bool` |
| Handle a type that breaks the generic code | Pointers, references, arrays |
| Compile-time type classification | The basis of `<type_traits>` |

### Pitfall

- **Function templates can't be partially specialized.** Overload instead:
  ```cpp
  template <class T> void f(T);       // generic
  template <class T> void f(T*);      // overload for pointers (NOT specialization)
  ```

---

## 1.2 Non-Type Template Parameters (NTTP)

### What It Is

Template parameters that are **values**, not types.

```cpp
template <typename T, std::size_t N>   // N is a value
class FixedArray {
    T data_[N];
public:
    constexpr std::size_t size() const { return N; }
};

FixedArray<int, 10> a;   // N = 10 baked in at compile time
```

`std::array<T, N>` works exactly this way. C++17 added `template <auto N>` to
deduce the constant's type (see
[C++17 Ch.2](../CPP17/02_templates_and_constexpr.md)).

---

## 1.3 SFINAE & `std::enable_if`

### What It Is

**SFINAE** = "Substitution Failure Is Not An Error." When the compiler
substitutes template arguments and the result is **ill-formed in the immediate
context**, that candidate is silently **removed from overload resolution**
instead of causing a hard error.

### Why It Was Introduced — Constraining Templates

Before concepts (C++20), SFINAE was the way to say "this template only applies to
*these* types."

### `std::enable_if`

```cpp
#include <type_traits>

// Enable only for integral types
template <typename T,
          std::enable_if_t<std::is_integral_v<T>, int> = 0>
T half(T x) { return x / 2; }

// A separate overload for floating-point types
template <typename T,
          std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
T half(T x) { return x * 0.5; }

half(10);    // integral version → 5
half(10.0);  // floating version → 5.0
// half("s"); // neither enabled → compile error (no matching function)
```

### How `enable_if` Works

`std::enable_if<true, T>::type` is `T`; `std::enable_if<false, T>` has **no**
`::type`. Referencing the missing `::type` is a substitution failure → that
overload drops out (SFINAE).

### The Modern Replacement

```cpp
// C++17: if constexpr — much cleaner than two SFINAE overloads
template <typename T>
T half(T x) {
    if constexpr (std::is_integral_v<T>) return x / 2;
    else                                 return x * 0.5;
}

// C++20: concepts — cleaner still
template <std::integral T> T half(T x) { return x / 2; }
```

> **Interview point:** SFINAE was the pre-C++20 tool to constrain templates;
> `if constexpr` (C++17) and **concepts** (C++20) largely replace it for
> readability — but you must still recognize SFINAE in existing code.

### Pitfalls

- SFINAE only applies to failures in the **immediate context** (the signature),
  not errors deep inside a function body (those are hard errors).
- Notoriously **cryptic error messages**.

---

## 1.4 Type Traits & the Detection Idiom

### Type Traits Recap

`<type_traits>` provides compile-time type queries/transformations
(`is_integral`, `is_pointer`, `remove_reference`, `decay`, ...). See
[C++11 Ch.5](../CPP11/05_templates_and_constexpr.md).

### Writing Your Own Trait

```cpp
// Does T have a member type 'value_type'?  (detection idiom with void_t)
template <typename, typename = std::void_t<>>
struct has_value_type : std::false_type {};

template <typename T>
struct has_value_type<T, std::void_t<typename T::value_type>>
    : std::true_type {};

static_assert(has_value_type<std::vector<int>>::value);
static_assert(!has_value_type<int>::value);
```

`std::void_t` (C++17) maps any valid type list to `void`; if the expression
inside is ill-formed, the specialization doesn't match → trait is `false`. This
is the standard, teachable detection idiom.

---

## 1.5 CRTP — Curiously Recurring Template Pattern

### What It Is

A class derives from a template **instantiated with itself**:

```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();  // dispatch to derived
    }
};

class Widget : public Base<Widget> {        // ← derives from Base<Widget>
public:
    void implementation() { std::cout << "Widget impl\n"; }
};
```

### Why It Was Introduced — Static Polymorphism

CRTP gives **polymorphism without virtual functions** — the dispatch is resolved
**at compile time**, so there's **no vtable lookup** and calls can be inlined.

### Virtual vs CRTP

| | Virtual functions | CRTP |
|--|-------------------|------|
| Dispatch | Runtime (vtable) | **Compile time** |
| Cost | Indirect call, no inlining | Zero overhead, inlinable |
| Flexibility | Runtime polymorphism (containers of `Base*`) | Type known at compile time |
| Use when | Heterogeneous runtime types | Hot paths, mixins, static interfaces |

### Real-World Use Cases

- **Mixins / static interfaces** (add behavior without virtuals).
- **`enable_shared_from_this`** uses CRTP.
- Expression templates, policy injection, performance-critical hierarchies.

```cpp
// Mixin example: add comparison operators from a single operator<
template <typename Derived>
struct Comparable {
    friend bool operator>(const Derived& a, const Derived& b)  { return b < a; }
    friend bool operator==(const Derived& a, const Derived& b) { return !(a < b) && !(b < a); }
};
struct Money : Comparable<Money> {
    int cents;
    friend bool operator<(const Money& a, const Money& b){ return a.cents < b.cents; }
};
```

### Pitfall

- The base can't be used polymorphically at runtime (each `Base<Derived>` is a
  distinct type). CRTP is **static**, not a `Base*` container substitute.

---

## 1.6 Tag Dispatch

### What It Is

Select an implementation by passing a **tag type** as an extra argument, letting
**overload resolution** pick the right version at compile time.

```cpp
// Choose an algorithm based on iterator category
template <typename Iter>
void advance_impl(Iter& it, int n, std::random_access_iterator_tag) {
    it += n;                              // O(1) for random-access iterators
}
template <typename Iter>
void advance_impl(Iter& it, int n, std::input_iterator_tag) {
    while (n--) ++it;                     // O(n) fallback
}

template <typename Iter>
void my_advance(Iter& it, int n) {
    advance_impl(it, n,
        typename std::iterator_traits<Iter>::iterator_category{}); // dispatch tag
}
```

The real `std::advance` works exactly like this. C++17's `if constexpr` can
replace tag dispatch in many cases.

---

## 1.7 Template Metaprogramming (TMP) & `constexpr`

### Classic TMP (Recursive Templates)

```cpp
// Compile-time factorial via template recursion (old style)
template <int N> struct Factorial { static constexpr int value = N * Factorial<N-1>::value; };
template <>      struct Factorial<0> { static constexpr int value = 1; };

constexpr int f = Factorial<5>::value;   // 120, computed at compile time
```

### Modern Style — Prefer `constexpr` Functions

```cpp
// Same thing, far more readable (C++14 relaxed constexpr)
constexpr int factorial(int n) {
    int r = 1;
    for (int i = 2; i <= n; ++i) r *= i;
    return r;
}
constexpr int f = factorial(5);          // 120
```

> **Guideline:** use `constexpr` functions for compile-time *computation*; reserve
> template specialization/traits for compile-time *type* logic. See
> [C++14 constexpr](../CPP14/01_constexpr_and_variable_templates.md).

---

## 1.8 Policy-Based Design

### What It Is

Compose a class's behavior from interchangeable **policy** template parameters —
a powerful alternative to inheritance.

```cpp
// Threading policy injected at compile time
struct SingleThreaded { void lock() {} void unlock() {} };
struct MultiThreaded  { std::mutex m; void lock(){ m.lock(); } void unlock(){ m.unlock(); } };

template <typename ThreadingPolicy>
class Cache : private ThreadingPolicy {
public:
    void put(int k, int v) {
        this->lock();
        /* ... */
        this->unlock();
    }
};

Cache<SingleThreaded> fast;   // no locking overhead
Cache<MultiThreaded>  safe;   // thread-safe
```

### Why It Matters

- **Zero-overhead configuration** — policies are resolved at compile time.
- **Combinatorial flexibility** without deep inheritance trees.
- Used heavily in the standard library (allocators, traits, comparators).

---

## Chapter 1 Cheat Sheet

```cpp
// Full / partial specialization
template <> struct S<int> { ... };          // full
template <class T> struct S<T*> { ... };    // partial (classes only)

// SFINAE constrain
template <class T, std::enable_if_t<cond, int> = 0> void f(T);
// modern: if constexpr (C++17) / concepts (C++20)

// Detection idiom
template <class,class=std::void_t<>> struct has_x: std::false_type{};
template <class T> struct has_x<T, std::void_t<typename T::x>>: std::true_type{};

// CRTP — static polymorphism
template <class D> struct Base { void f(){ static_cast<D*>(this)->impl(); } };

// Tag dispatch
f(args, typename traits<T>::category{});

// Policy-based design
template <class Policy> class C : private Policy { ... };
```

**Remember:**
- Functions: **overload**, don't partially specialize. Classes: specialize freely.
- SFINAE constrains templates; `if constexpr`/concepts are the modern, readable
  successors.
- CRTP = polymorphism with **zero runtime cost** (compile-time dispatch).
- Prefer `constexpr` functions over recursive-template TMP for computation.

---

## Common Mistakes

1. Trying to **partially specialize a function template** (not allowed — overload).
2. Expecting SFINAE to catch errors in a function **body** (only signature-context).
3. Using CRTP where you actually need **runtime** polymorphism (`Base*` containers).
4. Writing recursive TMP for arithmetic that a `constexpr` function does clearly.

## When NOT to Use

- **Heavy TMP/SFINAE** when `if constexpr` (C++17) or concepts (C++20) are
  available — they're far more readable.
- **CRTP** when you need heterogeneous runtime collections — use virtuals.
- **Policy-based design** when a simple constructor parameter or strategy object
  suffices.

---

*Next: [Chapter 2 — The STL, In Depth](02_stl_deep_dive.md)*
