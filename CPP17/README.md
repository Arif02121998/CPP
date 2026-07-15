# Mastering C++17 — A Book-Style Study & Interview Guide

> A progressive, industry-level reference for the C++17 standard.
> Designed for self-study, revision, and interview preparation.
> Format: Markdown (PDF/book ready).

---

## How to Use This Book

- **Read progressively.** Chapters move from *basic → intermediate → advanced*.
- **Every feature follows the same template:** What → Why → Previous limitation →
  How it improves things → Real-world use → Before/After → Edge cases & pitfalls.
- **Cheat sheets** end every chapter.
- **Chapter 5** is the interview-prep bank.

> If you haven't read it yet, the companion [C++14 book](../CPP14/README.md)
> covers the features C++17 builds on.

---

## Table of Contents

### Front Matter
- [Preface: The Pragmatic Standard](#preface-the-pragmatic-standard)
- [The Standard Timeline](#the-standard-timeline)
- [C++14 → C++17 Evolution Snapshot](#c14--c17-evolution-snapshot)
- [C++17 Feature Map at a Glance](#c17-feature-map-at-a-glance)

### Chapter 1 — Core Language Features
*File:* [`01_core_language_features.md`](01_core_language_features.md)
- 1.1 Structured Bindings
- 1.2 `if`/`switch` with Initializer
- 1.3 `if constexpr` (Compile-Time Branching)
- 1.4 Fold Expressions
- 1.5 Inline Variables
- 1.6 Guaranteed Copy Elision
- 1.7 Nested Namespaces, `[[nodiscard]]`, `[[maybe_unused]]`, `[[fallthrough]]`

### Chapter 2 — Templates & `constexpr` Improvements
*File:* [`02_templates_and_constexpr.md`](02_templates_and_constexpr.md)
- 2.1 Class Template Argument Deduction (CTAD)
- 2.2 `auto` Non-Type Template Parameters
- 2.3 `constexpr` Lambdas & `constexpr if` synergy
- 2.4 `std::void_t`, deduction guides, `template<auto>`

### Chapter 3 — Vocabulary Types (STL Additions)
*File:* [`03_vocabulary_types.md`](03_vocabulary_types.md)
- 3.1 `std::optional`
- 3.2 `std::variant`
- 3.3 `std::any`
- 3.4 `std::string_view`

### Chapter 4 — Filesystem & Parallel Algorithms
*File:* [`04_filesystem_and_parallelism.md`](04_filesystem_and_parallelism.md)
- 4.1 `std::filesystem`
- 4.2 Parallel STL Algorithms (Execution Policies)
- 4.3 Other library additions (`std::apply`, `std::invoke`, `std::byte`, maps)

### Chapter 5 — Interview Preparation Bank
*File:* [`05_interview_questions.md`](05_interview_questions.md)
- Beginner / Intermediate / Advanced / Code-based / Tricky

### Back Matter
- [Common Mistakes Developers Make](#common-mistakes-developers-make-c17)
- [When NOT to Use a Feature](#when-not-to-use-a-feature)
- [Compilation & Performance Notes](#compilation--performance-notes)
- [Master Cheat Sheet](#master-cheat-sheet-c17)

---

## Preface: The Pragmatic Standard

If C++14 *completed* C++11, **C++17 made everyday C++ dramatically more
convenient and expressive.** It is a **medium-sized** release that delivered:

- **Vocabulary types** that change how you design APIs: `optional`, `variant`,
  `any`, `string_view`.
- **Syntax that removes boilerplate**: structured bindings, `if` with initializer,
  fold expressions, CTAD.
- **`if constexpr`** — finally, readable compile-time branching that replaces
  tag dispatch and SFINAE gymnastics.
- A real **`std::filesystem`** library and **parallel algorithms**.

**Key theme:** *Write less boilerplate; express intent directly; make safer APIs.*

---

## The Standard Timeline

| Standard | Year | Nature | Headline |
|----------|------|--------|----------|
| C++11 | 2011 | Major | Move, lambdas, `auto`, smart pointers |
| C++14 | 2014 | Minor | Relaxed `constexpr`, generic lambdas |
| **C++17** | 2017 | **Medium** | Vocabulary types, `if constexpr`, structured bindings, filesystem |
| C++20 | 2020 | Major | Concepts, ranges, coroutines, modules |

> **Interview soundbite:** "C++17 is the *pragmatic* standard — it added
> vocabulary types (`optional`/`variant`/`string_view`), compile-time branching
> (`if constexpr`), structured bindings, CTAD, filesystem, and parallel
> algorithms."

---

## C++14 → C++17 Evolution Snapshot

| Area | C++14 | C++17 |
|------|-------|-------|
| Returning "maybe a value" | Sentinels / `pair<bool,T>` | `std::optional<T>` |
| Type-safe unions | `union` + manual tag, or `boost::variant` | `std::variant` |
| Non-owning strings | `const std::string&` (forces allocations) | `std::string_view` |
| Compile-time branching | Tag dispatch / SFINAE | `if constexpr` |
| Unpacking pairs/tuples | `std::tie`, `.first/.second` | Structured bindings |
| Class template type args | Always explicit (`pair<int,int>`) | Deduced (CTAD) |
| Variadic reduction | Recursive templates | Fold expressions |
| File handling | Platform APIs / Boost | `std::filesystem` |
| Parallelism | Manual threads / TBB | Parallel STL policies |

---

## C++17 Feature Map at a Glance

```text
C++17
├── Core Language                                          → Ch.1
│   ├── Structured bindings   auto [a,b] = pair
│   ├── if/switch with init   if (auto x = f(); cond)
│   ├── if constexpr          compile-time branch
│   ├── Fold expressions      (args + ...)
│   ├── Inline variables      inline constexpr in headers
│   ├── Guaranteed copy elision
│   └── Attributes [[nodiscard]] [[maybe_unused]] [[fallthrough]]
├── Templates / constexpr                                  → Ch.2
│   ├── CTAD                  std::pair p{1, 2.0};
│   ├── auto NTTP             template<auto N>
│   ├── constexpr lambdas
│   └── std::void_t, deduction guides
├── Vocabulary Types                                       → Ch.3
│   ├── std::optional<T>
│   ├── std::variant<...>
│   ├── std::any
│   └── std::string_view
└── Library / System                                       → Ch.4
    ├── std::filesystem
    ├── Parallel algorithms (execution policies)
    ├── std::apply / std::invoke / std::make_from_tuple
    ├── std::byte
    └── map/set: try_emplace, insert_or_assign, merge, extract
```

---

## Common Mistakes Developers Make (C++17)

1. **Dangling `string_view`** — pointing into a temporary `std::string` that
   dies immediately. The single most dangerous C++17 footgun.
2. **Ignoring `std::optional` emptiness** — calling `*opt` or `opt->x` when empty
   is **undefined behavior** (use `.value()` for a checked access that throws).
3. **`std::variant` `bad_variant_access`** — `std::get<T>` on the wrong
   alternative throws.
4. **Assuming structured bindings create references** — by default
   `auto [a,b]` copies; use `auto&` to bind references.
5. **Overusing `std::any`** — it's type-erased and heap-prone; usually a
   `variant` or template is better.
6. **Forgetting to link the filesystem library** on older toolchains
   (`-lstdc++fs` on GCC < 9).

---

## When NOT to Use a Feature

| Feature | Avoid When |
|---------|-----------|
| `std::any` | You know the finite set of types — use `variant` |
| `std::optional<T&>` | Not allowed; use a pointer for optional references |
| `string_view` | The string must outlive the view but might not — own it instead |
| `if constexpr` | A runtime `if` suffices — don't force compile-time branching |
| Parallel policies | Small data sets — thread overhead outweighs benefit |
| CTAD | Deduction is ambiguous or hurts readability — spell the type |

---

## Compilation & Performance Notes

- **`string_view` removes allocations** for read-only string parameters — often a
  measurable speedup in parsing/logging-heavy code.
- **`if constexpr` discards the false branch at compile time** → smaller code and
  fewer instantiations than tag dispatch/SFINAE.
- **Structured bindings, CTAD, fold expressions are zero-runtime-cost** syntax.
- **Guaranteed copy elision** makes returning by value strictly as cheap as the
  best move — sometimes cheaper (no move at all).
- **Parallel algorithms** can speed up large workloads but add scheduling
  overhead; measure before adopting.
- Enable with `-std=c++17` / `/std:c++17`. Filesystem may need `-lstdc++fs`
  (GCC 8) or `-lc++fs` (older libc++).

---

## Master Cheat Sheet (C++17)

```cpp
// Structured bindings
auto [iter, inserted] = mymap.insert({k, v});

// if with initializer
if (auto it = m.find(k); it != m.end()) use(it->second);

// if constexpr
template <class T> auto stringify(T v) {
    if constexpr (std::is_arithmetic_v<T>) return std::to_string(v);
    else                                   return std::string(v);
}

// Fold expression
template <class... Ts> auto sum(Ts... xs) { return (xs + ...); }

// CTAD
std::pair p{1, 2.5};         // pair<int,double>
std::vector v{1, 2, 3};      // vector<int>

// optional / variant / string_view
std::optional<int> o = find();
std::variant<int, std::string> var = "hi";
std::string_view sv = "no allocation";

// filesystem
for (auto& e : std::filesystem::directory_iterator("."))
    std::cout << e.path() << '\n';
```

---

*Next: [Chapter 1 — Core Language Features](01_core_language_features.md)*
