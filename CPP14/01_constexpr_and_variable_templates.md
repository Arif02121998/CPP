# Chapter 1 — `constexpr` Evolution & Variable Templates

> *Level: Basic → Intermediate*
> Back to [Table of Contents](README.md)

---

## 1.1 The C++11 `constexpr` Straitjacket

### What `constexpr` Is

`constexpr` marks a function or variable as **capable of being evaluated at
compile time**. When all inputs are compile-time constants, the compiler runs
the function during compilation and bakes the result into the binary — zero
runtime cost.

### Why It Was Introduced (C++11)

Before `constexpr`, compile-time computation relied on **template
metaprogramming** — verbose, unreadable recursion that abused the type system.

```cpp
// C++03 — compile-time factorial via template recursion (ugly)
template <int N> struct Factorial {
    static const int value = N * Factorial<N - 1>::value;
};
template <> struct Factorial<0> { static const int value = 1; };

int arr[Factorial<5>::value]; // 120
```

C++11 `constexpr` let you write **normal-looking functions** instead.

### The C++11 Limitation

C++11 `constexpr` functions were **brutally restricted**. The body could
essentially contain only a **single `return` statement**:

- ❌ No loops (`for`, `while`)
- ❌ No local variables
- ❌ No mutation of variables
- ❌ No `if`/`switch` statements (you had to use the ternary `?:`)
- ❌ No multiple statements

```cpp
// C++11 — factorial must be written recursively with a ternary
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1); // only a return allowed
}
```

This forced developers back into **recursion gymnastics** — exactly the pain
`constexpr` was meant to remove.

---

## 1.2 Relaxed `constexpr` in C++14

### What Changed

C++14 **lifts almost all body restrictions**. A `constexpr` function may now
contain:

- ✅ Local variables (must be initialized, non-`static`, non-`thread_local`)
- ✅ Loops: `for`, `while`, `do-while`, range-`for`
- ✅ Conditionals: `if`, `switch`
- ✅ Multiple statements
- ✅ Mutation of local variables
- ✅ Multiple `return` statements

### Before vs After

```cpp
// ──────────────── C++11 (forced recursion) ────────────────
constexpr int factorial11(int n) {
    return n <= 1 ? 1 : n * factorial11(n - 1);
}

// ──────────────── C++14 (natural iterative style) ────────────────
constexpr int factorial14(int n) {
    int result = 1;                 // local variable — OK in C++14
    for (int i = 2; i <= n; ++i)    // loop — OK in C++14
        result *= i;                // mutation — OK in C++14
    return result;
}

static_assert(factorial14(5) == 120, "compile-time check");
```

Both compute the same value at compile time, but the C++14 version reads like
ordinary code.

### Why This Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Readability** | Iterative loops instead of recursion gymnastics |
| **Maintainability** | `constexpr` functions look like normal functions |
| **Performance** | Compile-time evaluation → zero runtime cost; no recursion-depth limits |
| **Safety** | `static_assert` catches errors at build time, not runtime |

### Real-World Use Cases

- **Lookup tables** computed at compile time (CRC tables, sine tables).
- **Compile-time string hashing** for switch-on-string dispatch.
- **Dimensional/units computations** validated before the program runs.
- **Embedded systems** — push work to compile time to save flash/RAM/cycles.

```cpp
// Compile-time CRC-style table generation (sketch)
constexpr std::array<uint32_t, 256> make_table() {
    std::array<uint32_t, 256> t{};
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t c = i;
        for (int k = 0; k < 8; ++k)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        t[i] = c;
    }
    return t;
}
constexpr auto crc_table = make_table(); // built entirely at compile time
```

> Note: `std::array::operator[]` became `constexpr` in C++14 — another reason
> this pattern works in C++14 but not C++11.

---

## 1.3 Variable Templates

### What They Are

A **variable template** is a templated *variable* — a family of variables
parameterized by type (or value). New in C++14.

```cpp
template <typename T>
constexpr T pi = T(3.1415926535897932385L);

double d = pi<double>;   // 3.14159...
float  f = pi<float>;    // 3.14159f (correctly typed)
```

### Why It Was Introduced

Before C++14, "templated constants" had to be wrapped in a **class/struct** with
a static member, or expressed via functions.

### The Limitation Before C++14

```cpp
// C++11 idiom 1 — struct wrapper (verbose)
template <typename T>
struct Pi { static constexpr T value = T(3.14159265358979L); };
double d = Pi<double>::value;   // clunky ::value everywhere

// C++11 idiom 2 — function (needs call syntax + ())
template <typename T>
constexpr T pi_fn() { return T(3.14159265358979L); }
double d2 = pi_fn<double>();    // trailing () noise
```

### Before vs After

```cpp
// Before (C++11): Pi<double>::value
// After  (C++14): pi<double>
```

The C++14 form is **shorter, intention-revealing, and type-correct** — `pi<float>`
yields a `float`, avoiding the precision/truncation surprises of a single
`double` constant.

### Real-World Use Cases

- **Math constants** with correct precision per type (`pi<T>`, `e<T>`).
- **Type traits shorthand** — the standard later added `_v` helpers using this:
  ```cpp
  template <class T>
  constexpr bool is_integral_v = std::is_integral<T>::value; // C++17 std helper
  ```
- **Tunable compile-time configuration** parameterized by type or tag.

### Pitfalls

- A variable template is **one entity per instantiation** — odr-use rules apply;
  prefer `constexpr` (internal linkage in headers) to avoid ODR violations.
- Don't overuse: if a plain `constexpr` constant works, use that.

---

## 1.4 Edge Cases & Pitfalls (`constexpr`)

1. **`constexpr` ≠ guaranteed compile-time.** It means *can* be evaluated at
   compile time. Used in a runtime context, it may run at runtime.
   ```cpp
   constexpr int sq(int x) { return x * x; }
   int n = read_from_user();
   int a = sq(5);   // compile-time (constant context could force it)
   int b = sq(n);   // runtime — perfectly legal
   ```
   To **force** compile-time evaluation, use it in a constant context:
   ```cpp
   constexpr int a = sq(5);     // forced
   static_assert(sq(5) == 25);  // forced
   std::array<int, sq(5)> arr;  // forced (array bound)
   ```

2. **No `static`/`thread_local` locals** inside a C++14 `constexpr` function.

3. **Everything called must itself be `constexpr`-usable.** Calling a
   non-`constexpr` function poisons compile-time evaluation.

4. **Undefined behavior is not allowed in a constant expression.** If the
   compiler detects UB (e.g., signed overflow, out-of-bounds) during constant
   evaluation, it's a **hard compile error** — a hidden safety bonus.

5. **`constexpr` implies `inline`** for functions — safe to define in headers.

---

## C++11 vs C++14 — `constexpr` Comparison

| Capability | C++11 | C++14 |
|-----------|:-----:|:-----:|
| Single `return` body | ✅ | ✅ |
| Local variables | ❌ | ✅ |
| Loops (`for`/`while`) | ❌ | ✅ |
| `if` / `switch` | ❌ | ✅ |
| Mutating locals | ❌ | ✅ |
| Multiple statements | ❌ | ✅ |
| Variable templates | ❌ | ✅ |
| `std::array::operator[]` constexpr | ❌ | ✅ |

---

## Chapter 1 Cheat Sheet

```cpp
// Relaxed constexpr: write normal iterative code
constexpr int sum_to(int n) {
    int s = 0;
    for (int i = 1; i <= n; ++i) s += i;   // loops + locals OK (C++14)
    return s;
}

// Force compile-time evaluation
static_assert(sum_to(100) == 5050);

// Variable template: type-correct constants
template <class T> constexpr T half = T(0.5);
auto x = half<float>;   // 0.5f

// Pitfall: constexpr enables, does not force, compile-time eval
```

**Remember:**
- C++14 `constexpr` = *write functions normally, run them at compile time*.
- `constexpr` **enables**, the **context forces** compile-time evaluation.
- Variable templates replace `Trait<T>::value` clunkiness with `value<T>`.

---

*Next: [Chapter 2 — Lambda Improvements](02_lambda_improvements.md)*
