# Chapter 4 — Smart Pointers & Memory Management (C++11)

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

C++11 made **manual `new`/`delete` obsolete** for ownership by providing
smart pointers that automate lifetime via RAII.

---

## 4.1 The Problem: Manual Memory Management

### C++03 Pain

```cpp
// ─────────── C++03 ───────────
Widget* w = new Widget;
doWork(w);              // if this throws → LEAK (delete never runs)
delete w;               // easy to forget, double-delete, or use-after-free
```

Manual memory bugs — **leaks, double frees, dangling pointers, exception
leaks** — were among the most common and dangerous C++ defects.

### RAII — The Solution Principle

**Resource Acquisition Is Initialization:** tie a resource's lifetime to an
object's scope. The destructor releases the resource **automatically**, even when
exceptions are thrown. Smart pointers are RAII applied to heap memory.

---

## 4.2 `std::unique_ptr` — Exclusive Ownership

### What It Is

A smart pointer that **exclusively owns** an object and deletes it automatically
when the `unique_ptr` goes out of scope. It is **move-only** (cannot be copied).

```cpp
#include <memory>

std::unique_ptr<Widget> w(new Widget);   // owns the Widget
w->doWork();                             // use like a raw pointer
// no delete needed — freed automatically at end of scope

// Move ownership (cannot copy)
std::unique_ptr<Widget> w2 = std::move(w);  // w is now null, w2 owns it
```

> `std::make_unique` is **C++14**, not C++11. In C++11 use the constructor:
> `std::unique_ptr<Widget>(new Widget)`.

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Safety** | Automatic deletion — no leaks, even on exceptions |
| **Cost** | **Zero overhead** vs a raw pointer (same size, no atomics) |
| **Intent** | The type documents *exclusive* ownership |

### Custom Deleters & Arrays

```cpp
// Array form — calls delete[]
std::unique_ptr<int[]> arr(new int[10]);
arr[0] = 1;

// Custom deleter (e.g., C resource)
auto closer = [](FILE* f){ if (f) std::fclose(f); };
std::unique_ptr<FILE, decltype(closer)> file(std::fopen("f.txt", "r"), closer);
```

### Use Cases

- The **default** smart pointer — use it whenever ownership is unique.
- Factory return types (`std::unique_ptr<Base> create();`).
- PIMPL idiom; owning members in classes (enables Rule of Zero).

---

## 4.3 `std::shared_ptr` — Shared Ownership

### What It Is

A reference-counted smart pointer. **Multiple `shared_ptr`s can own the same
object**; it is deleted when the **last** owner is destroyed.

```cpp
auto sp1 = std::make_shared<Widget>();   // ref count = 1
{
    auto sp2 = sp1;                      // ref count = 2 (copy shares ownership)
    sp2->doWork();
}                                        // sp2 gone → ref count = 1
// Widget destroyed when sp1 also goes away (ref count → 0)
```

### Why `make_shared` Is Preferred

```cpp
auto sp = std::make_shared<Widget>(args);  // ONE allocation (object + control block)
std::shared_ptr<Widget> sp2(new Widget);   // TWO allocations + exception risk
```

`make_shared` allocates the object and its **control block** (the ref counts)
together — fewer allocations, better cache locality, and exception-safe.

### How It Works — The Control Block

A `shared_ptr` holds:
1. a pointer to the **managed object**, and
2. a pointer to a **control block** containing the **strong count** (owners) and
   the **weak count**.

The reference count is updated **atomically**, making `shared_ptr` thread-safe to
*copy/destroy* (the count), though the pointed-to object still needs its own
synchronization.

### Cost & Use Cases

| Aspect | Note |
|--------|------|
| **Cost** | Larger than a raw pointer; atomic ref-count updates |
| **Use when** | Ownership is genuinely **shared** (graph nodes, caches, observers) |
| **Avoid when** | Ownership is unique → use `unique_ptr` (no atomic overhead) |

---

## 4.4 `std::weak_ptr` — Non-Owning Observer

### What It Is

A **non-owning** reference to an object managed by `shared_ptr`. It does **not**
keep the object alive and is used to **break reference cycles** and to safely
observe a possibly-expired object.

```cpp
auto sp = std::make_shared<Widget>();
std::weak_ptr<Widget> wp = sp;       // observes, does NOT increase strong count

if (auto locked = wp.lock()) {       // try to get a shared_ptr
    locked->doWork();                // object still alive — safe to use
} else {
    // object has been destroyed
}
```

### The Cycle Problem It Solves

```cpp
struct Node {
    std::shared_ptr<Node> next;   // if 'prev' is also shared_ptr...
    std::shared_ptr<Node> prev;   // ...two nodes reference each other →
};                                // ref counts never reach 0 → LEAK

// Fix: make one direction weak
struct NodeFixed {
    std::shared_ptr<NodeFixed> next;  // owns forward
    std::weak_ptr<NodeFixed>   prev;  // observes backward — no cycle
};
```

This is the classic **parent/child** or **doubly-linked** ownership pattern:
own one way with `shared_ptr`, observe the other way with `weak_ptr`.

---

## 4.5 Choosing the Right Pointer

```text
Need to own the object?
├── No  → use a raw pointer / reference (non-owning), or weak_ptr to observe a shared
└── Yes
    ├── Exclusive owner?      → std::unique_ptr   (default choice, zero overhead)
    └── Shared ownership?     → std::shared_ptr
        └── Avoid cycles with → std::weak_ptr for back-references
```

| Pointer | Owns? | Copyable? | Overhead | Use For |
|---------|:-----:|:---------:|----------|---------|
| `unique_ptr` | ✅ exclusive | ❌ (move only) | none | Default ownership |
| `shared_ptr` | ✅ shared | ✅ | atomic ref count | Shared ownership |
| `weak_ptr` | ❌ | ✅ | small | Break cycles / observe |
| raw `T*` | ❌ | ✅ | none | Non-owning access only |

---

## 4.6 Pitfalls

1. **Don't create two `shared_ptr`s from the same raw pointer:**
   ```cpp
   Widget* raw = new Widget;
   std::shared_ptr<Widget> a(raw);
   std::shared_ptr<Widget> b(raw);   // BUG: two control blocks → double delete
   ```
   Always use `make_shared` or copy an existing `shared_ptr`.

2. **`shared_ptr` cycles leak** — use `weak_ptr` for back-references.

3. **`unique_ptr` is move-only** — pass it by value (`std::move`) to transfer, or
   pass the raw pointer/reference for non-owning access.

4. **`.get()` returns a non-owning raw pointer** — never `delete` it, and don't
   build another smart pointer from it.

5. **`shared_ptr` atomic counting isn't free** — don't default to it; reach for
   `unique_ptr` first.

6. **`enable_shared_from_this`** is required if an object must hand out a
   `shared_ptr` to itself; constructing one from `this` directly causes a double
   control block.

---

## C++03 vs C++11 — Memory Management Comparison

| Capability | C++03 | C++11 |
|-----------|:-----:|:-----:|
| `unique_ptr` | ❌ (`auto_ptr`, broken) | ✅ |
| `shared_ptr` | ❌ (Boost/TR1) | ✅ |
| `weak_ptr` | ❌ | ✅ |
| RAII for heap memory | manual | ✅ standardized |
| Reference-counted sharing | manual/Boost | ✅ |

> Note: C++03's `std::auto_ptr` attempted unique ownership but had broken copy
> semantics (it *moved* on copy) and is deprecated/removed. `unique_ptr` is its
> correct replacement.

---

## Chapter 4 Cheat Sheet

```cpp
// Exclusive ownership (default choice)
std::unique_ptr<Widget> u(new Widget);     // make_unique is C++14
u = std::move(otherUnique);                // transfer ownership

// Shared ownership
auto s = std::make_shared<Widget>();       // one allocation, preferred
auto s2 = s;                               // share (ref count++)

// Observe without owning / break cycles
std::weak_ptr<Widget> w = s;
if (auto sp = w.lock()) sp->use();         // safe access if still alive

// Custom deleter
std::unique_ptr<FILE, decltype(&fclose)> f(fopen("x","r"), &fclose);
```

**Remember:**
- **Default to `unique_ptr`**; it's zero-overhead and expresses unique ownership.
- Use `shared_ptr` only for **genuinely shared** ownership (atomic cost).
- Break `shared_ptr` cycles with `weak_ptr`.
- Prefer `make_shared`; never build two smart pointers from one raw pointer.

---

## Common Mistakes (Memory)

1. Two `shared_ptr`s from one raw pointer → double free.
2. `shared_ptr` cycles → leak (need `weak_ptr`).
3. Defaulting to `shared_ptr` when `unique_ptr` suffices (needless atomics).
4. `delete`-ing the result of `.get()`.
5. Still writing manual `new`/`delete` for owned resources.

## When NOT to Use

- **`shared_ptr`** when ownership is unique — it's slower and hides intent.
- **Smart pointers at all** for non-owning access — a raw pointer/reference is
  the correct, cheapest tool to say "I observe but don't own."

---

*Next: [Chapter 5 — Templates & Compile-Time Programming](05_templates_and_constexpr.md)*
