# Mastering Advanced C++ — Beyond Basic Templates

> A book-style deep dive into the C++ concepts that come *after* you've learned
> the basics and basic templates.
> Cross-linked to the [C++11](../CPP11/README.md), [C++14](../CPP14/README.md),
> and [C++17](../CPP17/README.md) books.
> Format: Markdown (PDF/book ready).

---

## Who This Book Is For

You already know:
- C++ basics (types, control flow, functions, classes, OOP)
- Memory management fundamentals (stack/heap, `new`/`delete`, RAII intro)
- **Basic templates** (function templates, simple class templates)

This book picks up **from there** and covers the advanced topics that separate an
intermediate programmer from an expert.

---

## How to Use This Book

- **Read progressively.** Each chapter builds toward expert-level understanding.
- **Same teaching template throughout:** What → Why → Problem it solves →
  Before/After → Real-world use → Pitfalls → Cheat sheet.
- **Cross-links:** wherever a topic was introduced in a specific standard, you'll
  see a link to the relevant chapter in the C++11/14/17 books.

---

## Table of Contents

### Chapter 1 — Advanced Templates & Metaprogramming
*File:* [`01_advanced_templates.md`](01_advanced_templates.md)
- Full & partial specialization
- Non-type template parameters
- SFINAE & `enable_if`
- Type traits & the detection idiom
- CRTP (Curiously Recurring Template Pattern)
- Tag dispatch
- Template metaprogramming & `constexpr` computation
- Policy-based design
> *Builds on:* [C++11 templates](../CPP11/05_templates_and_constexpr.md),
> [C++17 `if constexpr`/`void_t`](../CPP17/02_templates_and_constexpr.md)

### Chapter 2 — The STL, In Depth
*File:* [`02_stl_deep_dive.md`](02_stl_deep_dive.md)
- Container internals & when to choose each
- Iterator categories & invalidation rules
- The algorithm library & complexity
- Allocators (overview)
- Big-O reference per operation
> *Builds on:* [C++11 STL additions](../CPP11/07_stl_and_library.md),
> [C++17 parallel algorithms](../CPP17/04_filesystem_and_parallelism.md)

### Chapter 3 — Exception Handling & Error Models
*File:* [`03_exceptions_and_error_models.md`](03_exceptions_and_error_models.md)
- `throw`/`try`/`catch` mechanics, stack unwinding
- The three exception-safety guarantees
- RAII as the foundation of safety
- `noexcept` and move operations
- Exceptions vs error codes vs `optional`/`expected`
> *Builds on:* [C++11 smart pointers/RAII](../CPP11/04_smart_pointers.md),
> [C++17 `std::optional`](../CPP17/03_vocabulary_types.md)

### Chapter 4 — Design Patterns in Modern C++
*File:* [`04_design_patterns.md`](04_design_patterns.md)
- Creational: Singleton, Factory, Builder
- Structural: PIMPL, Adapter, Decorator
- Behavioral: Strategy, Observer, Visitor
- Modern idioms: RAII, Rule of Zero, type erasure
> *Builds on:* [C++11 lambdas](../CPP11/03_lambdas.md),
> [C++17 `variant`/`visit`](../CPP17/03_vocabulary_types.md)

### Chapter 5 — Advanced Concurrency Patterns
*File:* [`05_concurrency_patterns.md`](05_concurrency_patterns.md)
- Thread pools
- Producer/consumer & thread-safe queues
- Lock-free programming & atomics
- Futures, continuations, async pipelines
- Synchronization patterns & hazards
> *Builds on:* [C++11 concurrency](../CPP11/06_concurrency.md)

### Chapter 6 — Move & Forwarding, In Depth
*File:* [`06_move_and_forwarding_deep.md`](06_move_and_forwarding_deep.md)
- Value categories (lvalue/xvalue/prvalue) in full
- Reference collapsing & forwarding references
- `std::move` vs `std::forward` internals
- Perfect forwarding & its failure cases
- Copy elision / guaranteed elision
> *Builds on:* [C++11 move semantics](../CPP11/02_move_semantics.md),
> [C++17 copy elision](../CPP17/01_core_language_features.md)

### Chapter 7 — Interview Preparation Bank
*File:* [`07_interview_questions.md`](07_interview_questions.md)
- All topics: beginner → advanced, conceptual / code / tricky

---

## The Modern C++ Concept Map

```text
You are here ──► Basic templates
                      │
                      ▼
   ┌──────────────────────────────────────────────────────────┐
   │  ADVANCED C++ (this book)                                 │
   ├──────────────────────────────────────────────────────────┤
   │  Ch1 Advanced Templates   → specialization, SFINAE, CRTP  │
   │  Ch2 STL Deep Dive        → containers, iterators, algos   │
   │  Ch3 Error Models         → exception safety, noexcept     │
   │  Ch4 Design Patterns      → idiomatic modern solutions     │
   │  Ch5 Concurrency Patterns → pools, lock-free, async        │
   │  Ch6 Move/Forwarding      → value categories, forwarding    │
   └──────────────────────────────────────────────────────────┘
                      │
                      ▼
        Expert: library & framework design
```

---

## How Standards Map to These Topics

| Topic | Introduced / Improved In |
|-------|--------------------------|
| SFINAE / `enable_if` | C++11 (`<type_traits>`) |
| `void_t` detection idiom | C++17 |
| `if constexpr` (replaces much SFINAE) | C++17 |
| Concepts (next step beyond SFINAE) | C++20 |
| Move semantics / forwarding | C++11 |
| Guaranteed copy elision | C++17 |
| `std::optional` error model | C++17 |
| `std::expected` error model | C++23 |
| Parallel algorithms | C++17 |
| `std::variant` (type-safe visitor) | C++17 |
| Atomics / threads | C++11 |
| `std::jthread` / cooperative cancel | C++20 |

> This book explains the **concepts**; the version books explain **what each
> standard added**. Together they give you both the *idea* and its *history*.

---

## Master Cheat Sheet (Advanced C++)

```cpp
// SFINAE / enable_if
template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void f(T);

// CRTP — static polymorphism
template <class Derived> struct Base { void run(){ static_cast<Derived*>(this)->impl(); } };

// Exception safety: copy-and-swap for strong guarantee
T& operator=(T other) { swap(*this, other); return *this; }  // 'other' by value

// noexcept move (enables container move optimization)
T(T&&) noexcept;

// Type erasure interface (how std::function works)
struct Concept { virtual ~Concept() = default; virtual void call() = 0; };

// Thread-safe queue (producer/consumer)
std::mutex m; std::condition_variable cv; std::queue<T> q;

// Perfect forwarding
template <class... A> auto make(A&&... a) { return T(std::forward<A>(a)...); }
```

---

*Next: [Chapter 1 — Advanced Templates & Metaprogramming](01_advanced_templates.md)*
