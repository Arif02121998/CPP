# Chapter 8 — Interview Preparation Bank (C++11)

> Back to [Table of Contents](README.md)
> Categories: **Beginner → Intermediate → Advanced**, plus **code-based** and
> **tricky edge-case** questions. Concise, complete answers.

C++11 is the most heavily interviewed standard. Master this chapter.

---

## 8.1 Beginner Questions (Conceptual)

**Q1. Why is C++11 considered so important?**
It created "Modern C++": move semantics, `auto`, lambdas, smart pointers, a
standard threading/memory model, variadic templates, and `constexpr`. Stroustrup
said it "feels like a new language."

**Q2. What does `auto` do, and what does it strip?**
It deduces a variable's type from its initializer. It **strips top-level `const`
and references** (add `&`/`const` explicitly to keep them).

**Q3. What is `nullptr` and why is it better than `NULL`?**
A type-safe null pointer (`std::nullptr_t`). `NULL` is just `0` (an int), causing
overload ambiguity between integer and pointer parameters; `nullptr` resolves
correctly to the pointer overload.

**Q4. What is the range-based `for` loop?**
Iterates directly over a range's elements: `for (const auto& x : v)`. Use
`const auto&` to avoid copying.

**Q5. What is `enum class` and why use it?**
A scoped, strongly-typed enum. No implicit int conversion and no name leakage —
`Color::Red` can't clash with `Light::Red` and won't silently compare to `1`.

**Q6. What is a smart pointer?**
An RAII object that owns a heap allocation and frees it automatically:
`unique_ptr` (exclusive), `shared_ptr` (shared), `weak_ptr` (non-owning observer).

**Q7. What is a lambda?**
An inline anonymous function object that can capture surrounding variables:
`[](int x){ return x*x; }`.

---

## 8.2 Intermediate Questions

**Q8. What problem do move semantics solve?**
They avoid expensive deep copies of temporaries by **stealing** their resources.
Returning/storing large objects becomes O(1) pointer transfers instead of O(n)
copies.

**Q9. What is an rvalue reference?**
`T&&` — a reference that binds to temporaries (rvalues), enabling a function to
know its argument is expendable and can be moved from.

**Q10. What does `std::move` actually do?**
**Nothing at runtime by itself** — it's a `static_cast` to an rvalue reference,
signaling that the object may be moved from. The actual move happens in a move
constructor/assignment.

**Q11. `unique_ptr` vs `shared_ptr` — when to use each?**
`unique_ptr` for exclusive ownership (zero overhead, move-only) — the default.
`shared_ptr` only when ownership is genuinely shared (adds atomic ref counting).

**Q12. How does `shared_ptr` know when to delete?**
A **control block** holds an atomic strong reference count. Each copy increments
it, each destruction decrements it; the object is deleted when it reaches 0.

**Q13. What is `weak_ptr` for?**
A non-owning observer of a `shared_ptr`-managed object. It breaks reference
cycles and lets you check/access the object safely via `.lock()`.

**Q14. Difference between capture by value `[=]` and by reference `[&]`?**
`[=]` copies (snapshots) used variables at definition time; `[&]` refers to the
live variables. `[&]` risks dangling if the lambda outlives them.

**Q15. What are variadic templates used for?**
Type-safe functions/classes accepting any number of typed arguments. They power
`make_shared`, `tuple`, `emplace`, and `std::thread`.

**Q16. `map` vs `unordered_map`?**
`map` is a sorted balanced tree (O(log n)); `unordered_map` is a hash table
(O(1) average, unordered). Use `map` for ordered traversal, `unordered_map` for
fastest lookup.

**Q17. What is the Rule of Five?**
If you define any of destructor, copy ctor, copy assignment, move ctor, or move
assignment, you usually need to consider all five. **Rule of Zero**: prefer RAII
members so you write none of them.

---

## 8.3 Advanced & Tricky Edge-Case Questions

**Q18. What does this output, and why?**
```cpp
std::vector<int> v{3};
std::cout << v.size();
```
`1`. Braces prefer the `initializer_list` constructor, so `{3}` is one element
`3`, not three default ints. Use `std::vector<int> v(3)` for three elements.

**Q19. Why must move operations be `noexcept`?**
STL containers (e.g., `std::vector` on reallocation) only **move** elements if
the move constructor is `noexcept`; otherwise they **copy** to preserve the
strong exception guarantee. Missing `noexcept` silently kills the performance gain.

**Q20. What's wrong with `return std::move(local);`?**
It can **disable copy elision (RVO)**, making the code *slower*, not faster. Just
write `return local;` — the compiler moves/elides automatically.

**Q21. What happens here?**
```cpp
const std::string c = "hi";
std::string d = std::move(c);
```
It **copies**, not moves. `c` is `const`, so the move constructor (which modifies
its source) can't bind; it falls back to the copy constructor.

**Q22. Explain forwarding references and reference collapsing.**
A `T&&` where `T` is a deduced template parameter is a **forwarding reference**
that binds to both lvalues and rvalues. During deduction, references collapse:
`& &`, `& &&`, `&& &` → `&`; only `&& &&` → `&&`. `std::forward<T>` uses this to
preserve value category.

**Q23. What's the danger with this lambda?**
```cpp
std::function<int()> f() { int x = 42; return [&]{ return x; }; }
```
It captures `x` by reference and returns the lambda; `x` is destroyed when `f`
returns, so calling the lambda reads freed memory (dangling). Capture by value.

**Q24. Why can creating two `shared_ptr`s from one raw pointer be fatal?**
Each `shared_ptr(raw)` creates its **own control block**, so both think they're
the sole owner and each deletes the object → **double free**. Use `make_shared`
or copy an existing `shared_ptr`.

**Q25. What happens if a `std::thread` is destroyed without join/detach?**
Its destructor calls `std::terminate()`, aborting the program.

**Q26. Why might `std::async(...)` run synchronously?**
The returned `std::future`'s destructor **blocks** until the task finishes. If
you don't store the future (a temporary), it's destroyed at the end of the full
expression, forcing the task to complete there — effectively synchronous.

**Q27. What restriction does C++11 `constexpr` have that C++14 removed?**
C++11 `constexpr` functions are essentially limited to a **single `return`** —
no loops, locals, or branches. C++14 allows all of those.

**Q28. Why use the predicate form of `cv.wait`?**
To handle **spurious wakeups** — a condition variable can wake without a
notification. `cv.wait(lk, pred)` re-checks the predicate and goes back to sleep
if it's still false.

---

## 8.4 Code-Based Challenges

**C1. Implement a move constructor and move assignment.**
```cpp
class Buffer {
    int* data_; std::size_t size_;
public:
    Buffer(Buffer&& o) noexcept : data_(o.data_), size_(o.size_) {
        o.data_ = nullptr; o.size_ = 0;
    }
    Buffer& operator=(Buffer&& o) noexcept {
        if (this != &o) {
            delete[] data_;
            data_ = o.data_; size_ = o.size_;
            o.data_ = nullptr; o.size_ = 0;
        }
        return *this;
    }
    ~Buffer() { delete[] data_; }
};
```

**C2. Perfect-forwarding factory.**
```cpp
template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}   // (this is essentially C++14's std::make_unique)
```

**C3. Variadic sum (recursive, C++11 style).**
```cpp
int sum() { return 0; }                              // base case
template <typename T, typename... R>
int sum(T first, R... rest) { return first + sum(rest...); }
// sum(1,2,3,4) == 10
```

**C4. Break a shared_ptr cycle with weak_ptr.**
```cpp
struct Node {
    std::shared_ptr<Node> next;   // owns forward
    std::weak_ptr<Node>   prev;   // observes backward (no cycle)
};
```

**C5. Thread-safe counter with a mutex.**
```cpp
std::mutex m; int counter = 0;
void inc() { std::lock_guard<std::mutex> lk(m); ++counter; }
// or, simpler and faster:
std::atomic<int> acounter{0};
void inc2() { ++acounter; }
```

**C6. Parallel result with std::async.**
```cpp
auto fut = std::async(std::launch::async, []{ return heavy(); });
// ... other work ...
auto result = fut.get();
```

---

## 8.5 Rapid-Fire Cheat Sheet (Interview)

| Question | One-line answer |
|----------|-----------------|
| C++11 in one phrase? | "The birth of Modern C++." |
| Most important feature? | Move semantics. |
| What does `std::move` do? | Casts to rvalue — enables a move, doesn't move. |
| `noexcept` on moves — why? | Containers only move if it's `noexcept`. |
| `unique_ptr` vs `shared_ptr`? | Exclusive (zero cost) vs shared (atomic count). |
| Break shared_ptr cycles? | `weak_ptr`. |
| `auto` strips what? | Top-level const and references. |
| `vector<int>{3}` size? | 1 (initializer_list), not 3. |
| Forwarding reference? | `T&&` with deduced `T`; binds l/r values. |
| `std::move` on const? | Silently copies. |
| `return std::move(local)`? | Bad — kills RVO. |
| Thread not joined? | `std::terminate`. |
| C++11 constexpr limit? | Single return only (C++14 relaxes). |
| `map` vs `unordered_map`? | Sorted O(log n) vs hashed O(1) avg. |
| Rule of Zero? | Let RAII members manage resources; write no special members. |

---

## Final C++11 Recap Table

| Theme | C++03 Pain | C++11 Fix |
|-------|-----------|-----------|
| Verbose types | spell out iterators | `auto` / `decltype` |
| Copying temporaries | deep copies | move semantics |
| Inline behavior | functor structs | lambdas |
| Manual memory | `new`/`delete` leaks | smart pointers + RAII |
| Null pointer | `NULL` == 0 ambiguity | `nullptr` |
| Variadic, type-safe | C varargs | variadic templates |
| Compile-time values | TMP/enum hacks | `constexpr` + `static_assert` |
| Threads | platform APIs | `std::thread` + memory model |
| Hash lookup | Boost/TR1 | `unordered_map`/`set` |

---

*End of the C++11 book. Continue to the [C++14 book »](../CPP14/README.md) and
[C++17 book »](../CPP17/README.md) to see how the language evolved.*
