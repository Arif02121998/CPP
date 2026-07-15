# Mastering C++11 — A Book-Style Study & Interview Guide

> A progressive, industry-level reference for the C++11 standard — the release
> that created "Modern C++."
> Designed for self-study, revision, and interview preparation.
> Format: Markdown (PDF/book ready).

---

## How to Use This Book

- **Read progressively.** Chapters move from *basic → intermediate → advanced*.
- **Every feature follows the same template:** What → Why → Previous limitation
  (C++03) → How it improves things → Real-world use → Before/After → Edge cases
  & pitfalls.
- **Cheat sheets** end every chapter.
- **Chapter 8** is the interview-prep bank.

> Companion books: [C++14 »](../CPP14/README.md) and [C++17 »](../CPP17/README.md).
> C++11 is the foundation those two refine.

---

## Table of Contents

### Front Matter
- [Preface: The Birth of Modern C++](#preface-the-birth-of-modern-c)
- [The Standard Timeline](#the-standard-timeline)
- [C++03 Recap (The Baseline)](#c03-recap-the-baseline)
- [C++11 Feature Map at a Glance](#c11-feature-map-at-a-glance)

### Chapter 1 — Core Language Essentials
*File:* [`01_core_language_essentials.md`](01_core_language_essentials.md)
- `auto`, `decltype`, trailing return types
- `nullptr`, range-based `for`, uniform/brace initialization
- `enum class`, `using` aliases, `override`/`final`, `= default`/`= delete`
- Raw/Unicode string literals, `constexpr` intro

### Chapter 2 — Move Semantics & Rvalue References
*File:* [`02_move_semantics.md`](02_move_semantics.md)
- Lvalues vs rvalues, rvalue references `&&`
- Move constructors / move assignment
- `std::move`, `std::forward`, perfect forwarding, reference collapsing

### Chapter 3 — Lambda Expressions
*File:* [`03_lambdas.md`](03_lambdas.md)
- Syntax, captures (by value/reference), `mutable`
- Closures, use with STL algorithms, `std::function`

### Chapter 4 — Smart Pointers & Memory Management
*File:* [`04_smart_pointers.md`](04_smart_pointers.md)
- `unique_ptr`, `shared_ptr`, `weak_ptr`, RAII
- Ownership models, cycles, custom deleters

### Chapter 5 — Templates & Compile-Time Programming
*File:* [`05_templates_and_constexpr.md`](05_templates_and_constexpr.md)
- Variadic templates, alias templates
- `constexpr`, `static_assert`, `<type_traits>`, `default`/`delete`

### Chapter 6 — Concurrency & the Memory Model
*File:* [`06_concurrency.md`](06_concurrency.md)
- `std::thread`, `mutex`, `lock_guard`, `unique_lock`
- `std::atomic`, `future`/`promise`/`async`, `condition_variable`
- `thread_local`, the C++11 memory model

### Chapter 7 — STL & Library Additions
*File:* [`07_stl_and_library.md`](07_stl_and_library.md)
- `unordered_map`/`unordered_set`, `std::array`, `std::tuple`
- `std::initializer_list`, `std::chrono`, `<random>`, `<regex>`
- `std::function`, `std::bind`, new algorithms

### Chapter 8 — Interview Preparation Bank
*File:* [`08_interview_questions.md`](08_interview_questions.md)
- Beginner / Intermediate / Advanced / Code-based / Tricky

### Back Matter
- [Common Mistakes Developers Make](#common-mistakes-developers-make-c11)
- [When NOT to Use a Feature](#when-not-to-use-a-feature)
- [Compilation & Performance Notes](#compilation--performance-notes)
- [Master Cheat Sheet](#master-cheat-sheet-c11)

---

## Preface: The Birth of Modern C++

C++11 is the **most important release in the language's history**. Standardized
13 years after C++03, it was so transformative that Bjarne Stroustrup said it
*"feels like a new language."* The term **"Modern C++"** refers to the style that
C++11 made possible.

It introduced the pillars every later standard builds on:

- **Move semantics** — eliminate needless copies; make value types fast.
- **`auto` & `decltype`** — type inference that tames verbose declarations.
- **Lambdas** — inline callables that finally made STL algorithms ergonomic.
- **Smart pointers** — `unique_ptr`/`shared_ptr` for safe, leak-free ownership.
- **Concurrency** — a *standard* threading and memory model for the first time.
- **Variadic templates & `constexpr`** — type-safe, compile-time programming.

**Key theme:** *Safety, performance, and expressiveness — without giving up the
zero-overhead principle.*

---

## The Standard Timeline

| Standard | Year | Nature | Headline |
|----------|------|--------|----------|
| C++98 | 1998 | Foundation | The first ISO C++; STL |
| C++03 | 2003 | Bug-fix | Minor corrections to C++98 |
| **C++11** | 2011 | **Major revolution** | Move, lambdas, `auto`, smart pointers, threads |
| C++14 | 2014 | Minor | Polishes C++11 |
| C++17 | 2017 | Medium | Vocabulary types, `if constexpr` |
| C++20 | 2020 | Major | Concepts, ranges, coroutines, modules |

> **Interview soundbite:** "C++11 created Modern C++ — move semantics, `auto`,
> lambdas, smart pointers, variadic templates, and a standard threading/memory
> model. Everything since has refined these foundations."

---

## C++03 Recap (The Baseline)

To appreciate C++11, recall how painful C++03 was:

| Task in C++03 | The Pain |
|---------------|----------|
| Declaring an iterator | `std::map<K,V>::const_iterator it = m.begin();` (verbose) |
| Returning a big container | Expensive **deep copy** (no move) |
| Inline callable for `sort` | Write a separate **functor struct** |
| Dynamic memory | Manual `new`/`delete` → leaks, double-frees |
| Null pointer | `NULL`/`0` — ambiguous with integers |
| Threads | **No standard support** — platform APIs only |
| Variadic functions | C-style `va_args` (unsafe) or many overloads |
| Compile-time constants | `enum` hacks / template metaprogramming |
| Initializing a container | Push elements one by one |

C++11 fixes **every single row** above.

---

## C++11 Feature Map at a Glance

```text
C++11
├── Core Language                                          → Ch.1
│   ├── auto / decltype / trailing return type
│   ├── nullptr
│   ├── range-based for
│   ├── uniform initialization {}
│   ├── enum class
│   ├── using alias (alias templates)
│   ├── override / final
│   ├── = default / = delete
│   └── raw & Unicode string literals
├── Move Semantics                                         → Ch.2
│   ├── rvalue references &&
│   ├── move ctor / move assignment
│   ├── std::move / std::forward
│   └── perfect forwarding, reference collapsing
├── Lambdas                                                → Ch.3
├── Smart Pointers / Memory                                → Ch.4
│   ├── unique_ptr / shared_ptr / weak_ptr
│   └── RAII
├── Templates & constexpr                                  → Ch.5
│   ├── variadic templates
│   ├── alias templates
│   ├── constexpr
│   ├── static_assert
│   └── <type_traits>
├── Concurrency                                            → Ch.6
│   ├── std::thread / mutex / lock_guard
│   ├── std::atomic / memory model
│   ├── future / promise / async
│   └── condition_variable / thread_local
└── Library / STL                                          → Ch.7
    ├── unordered_map / unordered_set
    ├── std::array / std::tuple
    ├── std::initializer_list
    ├── std::chrono / <random> / <regex>
    └── std::function / std::bind
```

---

## Common Mistakes Developers Make (C++11)

A preview — each chapter expands on these:

1. **Thinking `std::move` moves something.** It only *casts* to an rvalue; the
   actual move happens in a move constructor/assignment.
2. **Using a moved-from object** expecting its old value (it's valid but
   unspecified).
3. **Creating `shared_ptr` cycles** that leak — break them with `weak_ptr`.
4. **Brace-init surprise:** `std::vector<int> v{3}` is one element `3`, not three.
5. **Capturing locals by reference in a lambda** that outlives them → dangling.
6. **Forgetting to `join()`/`detach()` a `std::thread`** → `std::terminate`.
7. **Assuming `auto` keeps references/const** — it strips them.

---

## When NOT to Use a Feature

| Feature | Avoid When |
|---------|-----------|
| `auto` | The explicit type documents intent or prevents a wrong deduction |
| `std::move` | On a `const` object (silently copies) or a value you still need |
| `shared_ptr` | Ownership is unique — `unique_ptr` is cheaper (no atomics) |
| Lambdas with `[&]` | The closure escapes the current scope |
| Raw `std::thread` | A higher-level `std::async`/task model fits better |
| `std::bind` | A lambda is usually clearer and faster |

---

## Compilation & Performance Notes

- **Move semantics is the headline performance win** — returning/storing large
  objects no longer deep-copies. This made value semantics practical again.
- **`unique_ptr` is zero-overhead** vs a raw pointer; `shared_ptr` adds an
  **atomic** reference count (small but real cost).
- **`auto`/lambdas/variadic templates are compile-time constructs** — no runtime
  cost, but can increase build time and binary size via instantiations.
- **`constexpr`** pushes work to compile time (zero runtime cost).
- Enable with `-std=c++11` (GCC/Clang) or `/std:c++11` where supported (MSVC's
  flag varies; modern MSVC defaults to newer standards).

---

## Master Cheat Sheet (C++11)

```cpp
// Type inference
auto it = container.begin();
decltype(x + y) sum;

// nullptr + range-for + uniform init
int* p = nullptr;
for (const auto& e : v) use(e);
std::vector<int> v{1, 2, 3};

// Move semantics
std::string s2 = std::move(s1);          // move, not copy
template <class T> void wrap(T&& x) {     // perfect forwarding
    inner(std::forward<T>(x));
}

// Lambda
auto cmp = [](int a, int b) { return a > b; };
std::sort(v.begin(), v.end(), cmp);

// Smart pointers
auto u = std::unique_ptr<Widget>(new Widget); // (make_unique is C++14)
auto sp = std::make_shared<Widget>();

// Variadic template + constexpr + static_assert
template <class... Ts> void log(Ts... xs);
constexpr int sq(int x) { return x * x; }
static_assert(sizeof(int) == 4, "");

// Concurrency
std::thread t([]{ work(); });
t.join();
std::atomic<int> counter{0};
auto fut = std::async([]{ return 42; });
```

---

*Next: [Chapter 1 — Core Language Essentials](01_core_language_essentials.md)*
