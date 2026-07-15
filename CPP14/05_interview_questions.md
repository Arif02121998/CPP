# Chapter 5 — Interview Preparation Bank (C++14)

> Back to [Table of Contents](README.md)
> Categories: **Beginner → Intermediate → Advanced**, plus **code-based** and
> **tricky edge-case** questions. Answers are concise but complete.

---

## 5.1 Beginner Questions (Conceptual)

**Q1. What kind of release is C++14?**
A minor/incremental release that polishes C++11. Headline items: relaxed
`constexpr`, generic lambdas, return type deduction, `make_unique`.

**Q2. What is `std::make_unique` and why was it added?**
A factory returning a `unique_ptr`. C++11 shipped `make_shared` but not
`make_unique`. It avoids raw `new`, names the type once, and is exception-safe in
multi-argument calls.

**Q3. What is a generic lambda?**
A lambda with `auto` parameters; the compiler turns `operator()` into a template,
so one lambda works for many types: `[](auto a, auto b){ return a + b; }`.

**Q4. What are binary literals and digit separators?**
`0b1010` writes numbers in base 2; `1'000'000` uses `'` to group digits for
readability (the `'` is ignored by the compiler).

**Q5. What does `[[deprecated]]` do?**
A standard attribute that makes the compiler emit a warning when a deprecated
entity is used, optionally with a message.

**Q6. Can a C++14 `constexpr` function contain loops?**
Yes. C++14 relaxed `constexpr` to allow loops, local variables, `if`/`switch`,
and mutation — unlike C++11's single-`return` restriction.

---

## 5.2 Intermediate Questions

**Q7. What restrictions did C++11 `constexpr` have that C++14 removed?**
C++11 allowed essentially a single `return` — no loops, no locals, no `if`/`switch`,
no mutation. C++14 allows all of those.

**Q8. Does `constexpr` guarantee compile-time evaluation?**
No. It makes the function *eligible*. Evaluation happens at compile time only in
a constant context (e.g., `static_assert`, array bound, `constexpr` variable).
Otherwise it may run at runtime.

**Q9. What is init capture and what problem does it solve?**
Init capture (`[x = expr]`) lets you introduce/initialize closure members. It
enables **capture by move** (e.g., moving a `unique_ptr` into a lambda), which
C++11 couldn't do, and lets you capture computed/renamed values.

**Q10. Difference between `auto` and `decltype(auto)` as return types?**
`auto` uses template deduction: strips top-level references and cv. `decltype(auto)`
uses `decltype` rules: preserves references and cv exactly — essential for
perfect-forwarding wrappers.

**Q11. What is a variable template? Give an example.**
A templated variable: `template<class T> constexpr T pi = T(3.14159...);`. Used
as `pi<double>`. Replaces the `Trait<T>::value` struct idiom.

**Q12. What is a transparent comparator?**
`std::less<>` (empty angle brackets) exposes `is_transparent`, enabling
heterogeneous lookup in associative containers — e.g., `set<string, less<>>::find`
with a `const char*` without constructing a temporary `string`.

**Q13. Why is `make_unique` more exception-safe than `unique_ptr<T>(new T)`?**
In `f(unique_ptr<A>(new A), unique_ptr<B>(new B))`, the compiler may run both
`new`s before either smart pointer is constructed; if the second throws, the
first allocation leaks. `make_unique` bundles allocation + ownership atomically.

**Q14. What is `std::exchange` used for?**
It assigns a new value and returns the old one — ideal for move constructors/
assignment: `data_ = std::exchange(other.data_, nullptr);`.

---

## 5.3 Advanced & Tricky Edge-Case Questions

**Q15. (Classic trap) What's wrong with this code?**
```cpp
decltype(auto) f() { int x = 0; return (x); }
```
The parentheses make `(x)` an lvalue expression, so `decltype((x))` is `int&`.
The function returns a **reference to a destroyed local** → undefined behavior.
Without the parentheses (`return x;`) it correctly returns `int`.

**Q16. What does each line deduce?**
```cpp
int i = 0; int& r = i; const int c = 0;
auto           a1 = r;   // ?
decltype(auto) a2 = r;   // ?
auto           a3 = c;   // ?
decltype(auto) a4 = c;   // ?
```
`a1` → `int` (auto strips ref). `a2` → `int&` (decltype preserves ref).
`a3` → `int` (auto strips top-level const). `a4` → `const int`.

**Q17. Why might heavy use of generic lambdas increase binary size?**
Each distinct set of argument types instantiates a separate `operator()`. In
hot, highly-polymorphic code this causes template instantiation bloat and longer
build times.

**Q18. Will this compile? Why/why not?**
```cpp
auto f(bool b) { if (b) return 1; else return 2.0; }
```
No. The two `return` statements deduce different types (`int` vs `double`).
Return type deduction requires all returns to agree.

**Q19. Is this recursion legal under return type deduction?**
```cpp
auto fact(int n) { return n < 2 ? 1 : n * fact(n - 1); }
```
Yes — a non-recursive return (`1`) is encountered first via the ternary, so the
type is known (`int`) before the recursive call. (If the first statement were the
recursive call, it would be ill-formed.)

**Q20. What state is a variable in after being moved into a lambda?**
Valid but unspecified. For `unique_ptr`, it becomes `nullptr`. Reading its
*value* afterward (beyond reassignment/destruction) is a logic bug.

**Q21. Why is an init-captured value `const` by default, and how do you change it?**
The closure's `operator()` is `const` unless the lambda is `mutable`. Add
`mutable` to allow modifying captured members:
`[n = 0]() mutable { return ++n; }`.

**Q22. Does `make_unique` support custom deleters?**
No. Use the explicit constructor: `std::unique_ptr<T, D>(new T, D{})`.

**Q23. What is the difference in initialization between**
`std::make_unique<int>()` **and** `std::unique_ptr<int>(new int)`?
`make_unique<int>()` value-initializes (zero). `new int` (no parens in the
expression sense) default-initializes → indeterminate value.

---

## 5.4 Code-Based Challenges

**C1. Implement a compile-time `power` function (C++14 style).**
```cpp
constexpr long ipow(long base, int exp) {
    long result = 1;
    for (int i = 0; i < exp; ++i) result *= base;
    return result;
}
static_assert(ipow(2, 10) == 1024, "");
```

**C2. Write a perfect-forwarding accessor that preserves references.**
```cpp
template <class Container, class Index>
decltype(auto) at(Container&& c, Index i) {
    return std::forward<Container>(c)[i];   // returns T& / const T& exactly
}
```

**C3. Move a `unique_ptr` into a deferred task.**
```cpp
auto data = std::make_unique<std::vector<int>>(1000);
auto task = [d = std::move(data)] { return d->size(); };
// 'data' is now nullptr; 'task' owns the vector
```

**C4. A generic, type-correct math constant.**
```cpp
template <class T> constexpr T tau = T(6.283185307179586L);
float  f = tau<float>;    // 6.2831855f
double d = tau<double>;   // full precision
```

**C5. Clean move constructor with `std::exchange`.**
```cpp
struct Buf {
    int* p; std::size_t n;
    Buf(Buf&& o) noexcept
        : p(std::exchange(o.p, nullptr)),
          n(std::exchange(o.n, 0)) {}
};
```

---

## 5.5 Rapid-Fire Cheat Sheet (Interview)

| Question | One-line answer |
|----------|-----------------|
| C++14 in one phrase? | "Polish release that completes C++11." |
| Killer C++14 features? | Relaxed `constexpr`, generic lambdas, return type deduction, `make_unique`. |
| `auto` vs `decltype(auto)` return? | `auto` strips refs/cv; `decltype(auto)` preserves them. |
| Famous `decltype(auto)` bug? | `return (local);` returns a dangling reference. |
| Capture a `unique_ptr` in a lambda? | Init capture: `[p = std::move(ptr)]`. |
| Force compile-time eval? | Use in a constant context (`static_assert`, array bound). |
| Allocation-free map lookup? | Transparent comparator `std::less<>`. |
| Why `make_unique`? | Exception safety + no raw `new`. |
| Variable template syntax? | `template<class T> constexpr T x = ...;` |
| Modify init-captured value? | Mark the lambda `mutable`. |

---

## Final C++14 Recap Table

| Theme | C++11 Pain | C++14 Fix |
|-------|-----------|-----------|
| Compile-time code | Single-`return` `constexpr` | Loops, locals, branches |
| Generic callables | Monomorphic lambdas | `auto` parameters |
| Move into closures | Impossible | Init capture |
| Return types | Trailing `decltype` boilerplate | `auto` / `decltype(auto)` |
| Smart pointers | No `make_unique` | `std::make_unique` |
| Map lookups | Temporary keys | Transparent comparators |

---

*End of the C++14 book. Continue to the [C++17 book »](../CPP17/README.md)*
