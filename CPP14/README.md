# Mastering C++14 — A Book-Style Study & Interview Guide

> A progressive, industry-level reference for the C++14 standard.
> Designed for self-study, revision, and interview preparation.
> Format: Markdown (PDF/book ready).

---

## How to Use This Book

- **Read progressively.** Chapters move from *basic → intermediate → advanced*.
- **Every feature is explained with the same template:**
  1. **What** it is
  2. **Why** it was introduced (the C++11 problem it solves)
  3. **Limitations** that existed before
  4. **How** it improves performance / readability / maintainability / safety
  5. **Real-world use cases**
  6. **Before vs After** code
  7. **Edge cases & pitfalls**
- **Cheat sheets** end every chapter for quick revision.
- **Chapter 5** is a dedicated interview-prep bank (beginner → advanced).

---

## Table of Contents

### Front Matter
- [Preface: Why C++14 Matters](#preface-why-c14-matters)
- [The Standard Timeline](#the-standard-timeline)
- [C++11 Recap (The Baseline)](#c11-recap-the-baseline)
- [C++14 Feature Map at a Glance](#c14-feature-map-at-a-glance)

### Chapter 1 — `constexpr` Evolution & Variable Templates
*File:* [`01_constexpr_and_variable_templates.md`](01_constexpr_and_variable_templates.md)
- 1.1 The C++11 `constexpr` Straitjacket
- 1.2 Relaxed `constexpr` in C++14
- 1.3 Variable Templates
- 1.4 Before vs After, Pitfalls, Cheat Sheet

### Chapter 2 — Lambda Improvements
*File:* [`02_lambda_improvements.md`](02_lambda_improvements.md)
- 2.1 Generic (Polymorphic) Lambdas
- 2.2 Generalized Lambda Capture (Init Capture / Capture by Move)
- 2.3 Real-World Patterns, Pitfalls, Cheat Sheet

### Chapter 3 — Type Deduction Upgrades
*File:* [`03_type_deduction.md`](03_type_deduction.md)
- 3.1 Function Return Type Deduction (`auto` return)
- 3.2 `decltype(auto)`
- 3.3 Deduction Rules, Gotchas, Cheat Sheet

### Chapter 4 — Library & STL Additions + Smaller Features
*File:* [`04_library_and_misc.md`](04_library_and_misc.md)
- 4.1 `std::make_unique`
- 4.2 Transparent Comparators (`std::less<>`)
- 4.3 `std::integer_sequence` / `index_sequence`
- 4.4 Heterogeneous Lookup, `std::exchange`, `std::quoted`
- 4.5 Binary Literals, Digit Separators, `[[deprecated]]`
- 4.6 Shared-Mutex (TS), `tuple` addressing by type
- 4.7 Cheat Sheet

### Chapter 5 — Interview Preparation Bank
*File:* [`05_interview_questions.md`](05_interview_questions.md)
- 5.1 Beginner Questions
- 5.2 Intermediate Questions
- 5.3 Advanced & Tricky Edge-Case Questions
- 5.4 Code-Based Challenges
- 5.5 Rapid-Fire Cheat Sheet

### Back Matter
- [Common Mistakes Developers Make](#common-mistakes-developers-make-c14)
- [When NOT to Use a Feature](#when-not-to-use-a-feature)
- [Compilation & Performance Notes](#compilation--performance-notes)
- [Master Cheat Sheet](#master-cheat-sheet-c14)

---

## Preface: Why C++14 Matters

C++11 was a **revolution** — it introduced move semantics, lambdas, `auto`,
smart pointers, `constexpr`, variadic templates, and more. But revolutions are
messy. Many C++11 features shipped with **sharp edges and artificial
restrictions** because the committee had to freeze the standard.

**C++14 is the "bug-fix and polish" release.** It is a *minor* standard by
design — no giant new subsystems — but it removes friction from C++11 features
so they become genuinely usable in everyday code. If C++11 gave you the tools,
C++14 made the tools comfortable to hold.

**Key theme:** *Make C++11 features actually pleasant to use.*

---

## The Standard Timeline

| Standard | Year | Nickname | Nature |
|----------|------|----------|--------|
| C++98/03 | 1998/2003 | Classic C++ | Foundation |
| **C++11** | 2011 | "Modern C++" | Major revolution |
| **C++14** | 2014 | The "completion" of C++11 | Minor / polish |
| C++17 | 2017 | Pragmatic upgrades | Medium |
| C++20 | 2020 | Second revolution | Major |

> **Interview soundbite:** "C++14 is a *minor* release whose job was to round
> off the rough edges of C++11 — relaxed `constexpr`, generic lambdas, return
> type deduction, and `make_unique` are the headline items."

---

## C++11 Recap (The Baseline)

To appreciate C++14 you must remember what C++11 *almost* gave you:

| C++11 Feature | What It Gave | What Was Still Painful |
|---------------|--------------|------------------------|
| `auto` | Type deduction for variables | Could **not** deduce function return types |
| Lambdas | Inline callables | Parameters needed **explicit types**; no capture-by-move |
| `constexpr` | Compile-time functions | Body limited to a **single `return`**; no loops/locals |
| `unique_ptr` | Safe ownership | No `make_unique` (only `make_shared` existed) |
| `decltype` | Query an expression's type | No `decltype(auto)` convenience |
| Templates | Variadic, alias | No **variable** templates |

C++14 attacks **every "still painful" cell** in that table.

---

## C++14 Feature Map at a Glance

```text
C++14
├── Language
│   ├── Relaxed constexpr        (loops, locals, mutation)   → Ch.1
│   ├── Variable templates                                   → Ch.1
│   ├── Generic lambdas          (auto params)               → Ch.2
│   ├── Init-capture lambdas     (capture by move)           → Ch.2
│   ├── Return type deduction    (auto f())                  → Ch.3
│   ├── decltype(auto)                                       → Ch.3
│   ├── Binary literals 0b1010                               → Ch.4
│   ├── Digit separators 1'000'000                           → Ch.4
│   └── [[deprecated]] attribute                             → Ch.4
└── Library
    ├── std::make_unique                                     → Ch.4
    ├── Transparent comparators std::less<>                  → Ch.4
    ├── std::integer_sequence / index_sequence               → Ch.4
    ├── std::exchange                                        → Ch.4
    ├── std::quoted                                          → Ch.4
    ├── Heterogeneous lookup in associative containers       → Ch.4
    ├── get<T>() for tuple by type                           → Ch.4
    └── Standard user-defined literals (s, ms, i, ...)       → Ch.4
```

---

## Common Mistakes Developers Make (C++14)

A preview — each chapter expands on these:

1. **Assuming `constexpr` *guarantees* compile-time evaluation.** It only makes
   it *possible*; use in a constant context (or C++20 `consteval`) to force it.
2. **Returning references to locals from `auto`/`decltype(auto)` functions.**
   `decltype(auto)` faithfully preserves references — including dangling ones.
3. **Forgetting that generic lambdas instantiate per argument type**, causing
   code bloat if misused in hot template code.
4. **Mixing `make_unique` with arrays incorrectly** (`make_unique<T[]>(n)` vs
   `make_unique<T>(args...)`).
5. **Expecting return type deduction to work across translation units** — it
   requires the definition to be visible.

---

## When NOT to Use a Feature

| Feature | Avoid When |
|---------|-----------|
| Relaxed `constexpr` | Logic is genuinely runtime-only; forcing it hurts readability |
| Generic lambdas | A simple typed lambda is clearer and limits instantiations |
| `decltype(auto)` | You don't *need* exact ref/cv preservation — plain `auto` is safer |
| Return type deduction | Public API headers where an explicit type documents intent |
| Variable templates | A plain `constexpr` constant or function suffices |

---

## Compilation & Performance Notes

- **C++14 features are zero-runtime-cost abstractions.** Relaxed `constexpr`,
  generic lambdas, and return type deduction are resolved *at compile time*.
- **Compile-time cost can rise:** generic lambdas and variable templates are
  templates — each unique instantiation adds to build time and binary size.
- **`make_unique` does not change runtime cost** vs `new` + `unique_ptr`; it
  improves **exception safety** in function-argument evaluation order.
- Enable with `-std=c++14` (GCC/Clang) or `/std:c++14` (MSVC).

---

## Master Cheat Sheet (C++14)

```cpp
// 1. Relaxed constexpr — loops + locals allowed
constexpr int factorial(int n) {
    int r = 1;
    for (int i = 2; i <= n; ++i) r *= i;   // legal in C++14, not C++11
    return r;
}

// 2. Variable template
template <typename T>
constexpr T pi = T(3.1415926535897932385);

// 3. Generic lambda
auto add = [](auto a, auto b) { return a + b; };

// 4. Init-capture / capture by move
auto p = std::make_unique<int>(42);
auto f = [q = std::move(p)] { return *q; };

// 5. Return type deduction
auto square(int x) { return x * x; }

// 6. decltype(auto) — perfect forwarding of return type
template <class C, class I>
decltype(auto) at(C&& c, I i) { return std::forward<C>(c)[i]; }

// 7. make_unique
auto up = std::make_unique<std::string>("hello");

// 8. Binary literal + digit separator
int mask = 0b1010'1010;
long big = 1'000'000;
```

---

*Next: [Chapter 1 — `constexpr` Evolution & Variable Templates](01_constexpr_and_variable_templates.md)*
