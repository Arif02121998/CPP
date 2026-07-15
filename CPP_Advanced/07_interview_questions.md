# Chapter 7 — Interview Preparation Bank (Advanced C++)

> Back to [Table of Contents](README.md)
> Covers all six chapters: advanced templates, STL internals, error models,
> design patterns, concurrency patterns, move/forwarding.
> Categories: **Beginner → Intermediate → Advanced**, plus **code** and **tricky**.

---

## 7.1 Beginner / Warm-Up Questions

**Q1. Difference between full and partial template specialization?**
Full specialization provides an implementation for one specific type
(`template <> struct S<int>`). Partial specialization covers a family of types
(`template <class T> struct S<T*>`). Only **class** templates can be partially
specialized; functions use overloading.

**Q2. What is RAII?**
Resource Acquisition Is Initialization — bind a resource's lifetime to an
object's scope so its destructor releases it automatically, even on exceptions.

**Q3. `std::vector` vs `std::list` — when each?**
`vector` for contiguous storage, random access, cache locality (the default).
`list` only when you need O(1) insert/erase anywhere with stable iterators.

**Q4. `map` vs `unordered_map`?**
`map` = sorted balanced tree, O(log n). `unordered_map` = hash table, O(1)
average but unordered and O(n) worst case.

**Q5. Why catch exceptions by `const` reference?**
To avoid object slicing and unnecessary copies, and to access the polymorphic
`what()` of the actual exception type.

**Q6. What are the three primary value categories?**
lvalue (named, persists), prvalue (temporary), xvalue (expiring, e.g.,
`std::move(x)`). rvalue = prvalue ∪ xvalue.

---

## 7.2 Intermediate Questions

**Q7. What is SFINAE and what replaced it?**
"Substitution Failure Is Not An Error" — invalid template substitutions in the
immediate context remove a candidate instead of erroring. `if constexpr` (C++17)
and **concepts** (C++20) are the modern, readable replacements.

**Q8. What is CRTP and why use it?**
The Curiously Recurring Template Pattern: a class derives from a base templated on
itself (`class D : Base<D>`). It gives **static (compile-time) polymorphism** with
no vtable cost — calls can be inlined.

**Q9. Explain the three exception-safety guarantees.**
- **No-throw:** never throws (destructors, swap, moves → `noexcept`).
- **Strong:** commit-or-rollback; on failure, state is unchanged.
- **Basic:** remains valid (no leaks/corruption) but state may change.

**Q10. How do you implement the strong exception guarantee?**
Copy-and-swap: do throwing work on a copy, then swap (which is `noexcept`) as the
atomic commit point. If the copy throws, the original is untouched.

**Q11. Why must move operations be `noexcept`?**
`std::vector` only **moves** elements on reallocation if the move ctor is
`noexcept`; otherwise it **copies** to keep the strong guarantee. Missing
`noexcept` silently kills the performance benefit.

**Q12. Difference between `std::move` and `std::forward`?**
`std::move` unconditionally casts to an rvalue (use on a value you own).
`std::forward<T>` conditionally casts — only if the original was an rvalue (use on
a forwarding reference to preserve value category).

**Q13. What is a forwarding (universal) reference?**
`T&&` where `T` is a **deduced** template parameter (or `auto&&`). It binds to both
lvalues and rvalues; a `std::string&&` (concrete type) is just a plain rvalue
reference.

**Q14. What is the remove-erase idiom?**
`std::remove` shifts kept elements forward and returns the new logical end without
shrinking the container; pair it with `erase`:
`v.erase(std::remove(v.begin(), v.end(), x), v.end());` (C++20: `std::erase`).

**Q15. How does a thread pool work?**
A fixed set of worker threads pull tasks from a shared thread-safe queue.
`std::packaged_task` bridges each task to a `std::future` for its result, avoiding
the cost of spawning a thread per task.

**Q16. What is PIMPL and what does it buy you?**
Pointer-to-implementation: hide a class's data behind `std::unique_ptr<Impl>` with
only a forward declaration in the header. Benefits: faster builds (clients don't
recompile on impl changes), stable ABI, true encapsulation.

---

## 7.3 Advanced & Tricky Questions

**Q17. Explain reference collapsing.**
When references combine during deduction: `T& &`, `T& &&`, `T&& &` all collapse to
`T&`; only `T&& &&` stays `T&&`. Mnemonic: any lvalue reference wins. It's what
makes forwarding references and `std::forward` work.

**Q18. Why is `return std::move(local);` an anti-pattern?**
It prevents (named) return value optimization / guaranteed copy elision, forcing a
move where the compiler could have constructed the object directly in place — so
it's slower. Write `return local;`.

**Q19. Name three cases where perfect forwarding fails.**
Braced-init-lists (`{1,2,3}` — no deducible type), `0`/`NULL` as null pointers
(deduce as int — use `nullptr`), overloaded/template function names (ambiguous),
bitfields, and certain `static const` members. (Any three.)

**Q20. What's wrong with this?**
```cpp
template <class T> void wrapper(T&& x) { inner(std::move(x)); }
```
`std::move` **unconditionally** moves, so if the caller passes an **lvalue**, its
object is silently moved-from (likely a bug). Use `std::forward<T>(x)` to preserve
the caller's value category.

**Q21. When does iterator invalidation bite you?**
E.g., `vector::push_back` causing reallocation invalidates **all** iterators and
references; erasing in a loop with `++it` after `erase(it)` is UB. Use
`it = v.erase(it);`. `unordered_map` rehash invalidates iterators (not references).

**Q22. How is `std::function`/`std::any` implemented?**
**Type erasure:** an internal abstract `Concept` base with a templated `Model<T>`
derived class stores the concrete object and forwards calls virtually — exposing a
single non-template type that can hold anything matching the interface.

**Q23. What is the ABA problem in lock-free programming?**
A CAS sees a value change from A→B→A and wrongly assumes nothing changed.
Mitigations: tagged pointers (version counters), hazard pointers, or RCU-style
reclamation. It's a key reason lock-free code is hard.

**Q24. What is false sharing and how do you fix it?**
Two independent variables on the **same cache line** cause cores to invalidate
each other's caches on every write, destroying scalability. Fix by padding/aligning
hot data to a cache line: `struct alignas(64) { std::atomic<int> c; };`.

**Q25. Why does the `std::async` future block in its destructor?**
The future returned by `std::async(launch::async, ...)` joins the task in its
destructor to avoid dangling. So an **unstored** `std::async(...)` temporary runs
**synchronously** because its future is destroyed at the end of the statement.

**Q26. What memory ordering would you use for a simple stats counter, and why?**
`memory_order_relaxed` — you only need atomicity (no torn reads/writes), not
ordering relative to other operations, so relaxed is correct and fastest.

**Q27. Why is the Meyers' Singleton thread-safe?**
C++11 guarantees that initialization of a **function-local `static`** is
thread-safe — the runtime serializes the first call, so no manual locking is
needed.

---

## 7.4 Code-Based Challenges

**C1. Write a trait that detects a `.size()` member.**
```cpp
template <typename, typename = std::void_t<>>
struct has_size : std::false_type {};
template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};
static_assert(has_size<std::vector<int>>::value);
static_assert(!has_size<int>::value);
```

**C2. CRTP base adding a `clone()` to derived types.**
```cpp
template <typename Derived>
struct Cloneable {
    std::unique_ptr<Derived> clone() const {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};
struct Doc : Cloneable<Doc> { std::string text; };
```

**C3. Strong-guarantee assignment via copy-and-swap.**
```cpp
class Buf {
    std::vector<int> d_;
public:
    Buf& operator=(Buf other) noexcept { swap(*this, other); return *this; }
    friend void swap(Buf& a, Buf& b) noexcept { std::swap(a.d_, b.d_); }
};
```

**C4. Perfect-forwarding factory.**
```cpp
template <typename T, typename... Args>
std::unique_ptr<T> create(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
```

**C5. Safe erase-in-loop.**
```cpp
for (auto it = v.begin(); it != v.end(); ) {
    if (shouldRemove(*it)) it = v.erase(it);   // erase returns next
    else                   ++it;
}
```

**C6. Visitor over a variant.**
```cpp
using Shape = std::variant<Circle, Square>;
struct Area {
    double operator()(const Circle& c) const { return 3.14159 * c.r * c.r; }
    double operator()(const Square& s) const { return s.side * s.side; }
};
double a = std::visit(Area{}, shape);
```

---

## 7.5 Rapid-Fire Cheat Sheet (Interview)

| Question | One-line answer |
|----------|-----------------|
| Partial specialization on functions? | Not allowed — overload instead. |
| SFINAE replaced by? | `if constexpr` (C++17), concepts (C++20). |
| CRTP gives? | Compile-time polymorphism, zero vtable cost. |
| Three safety guarantees? | no-throw > strong (commit/rollback) > basic. |
| Strong guarantee via? | Copy-and-swap. |
| `noexcept` on moves matters because? | Containers move (not copy) only if noexcept. |
| `move` vs `forward`? | Unconditional vs conditional rvalue cast. |
| Forwarding reference? | `T&&` with deduced `T`. |
| Reference collapsing? | Only `&& &&` → `&&`; else `&`. |
| `return std::move(local)`? | Anti-pattern — kills elision. |
| Remove-erase idiom? | `erase(remove(...), end())`. |
| Thread pool bridge to result? | `std::packaged_task` → `std::future`. |
| `std::async` temporary? | Runs synchronously (future blocks in dtor). |
| False sharing fix? | `alignas(64)` padding. |
| Type erasure powers? | `std::function`, `std::any`. |
| Meyers' Singleton safe since? | C++11 (local static init). |

---

## Final Recap — Advanced C++ Topics

| Chapter | Master These |
|---------|--------------|
| Advanced Templates | specialization, SFINAE, CRTP, traits, tag dispatch, policies |
| STL Deep Dive | container choice, invalidation, complexity, remove-erase |
| Error Models | 3 guarantees, copy-and-swap, `noexcept`, exceptions vs values |
| Design Patterns | RAII, Rule of Zero, factory→unique_ptr, variant-visitor, PIMPL |
| Concurrency Patterns | thread-safe queue, pool, CAS, deadlock avoidance, false sharing |
| Move/Forwarding | value categories, collapsing, `move`/`forward`, elision |

---

*End of the Advanced C++ book. See also the version books:
[C++11 »](../CPP11/README.md) · [C++14 »](../CPP14/README.md) ·
[C++17 »](../CPP17/README.md)*
