# Chapter 2 — The STL, In Depth

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)
> Builds on [C++11 STL additions](../CPP11/07_stl_and_library.md) and
> [C++17 parallel algorithms](../CPP17/04_filesystem_and_parallelism.md).

The STL has three pillars: **containers** (store data), **iterators** (traverse
data), and **algorithms** (operate on data). Understanding their internals and
complexity is what separates "uses the STL" from "uses it well."

---

## 2.1 Container Internals & Choosing the Right One

### Sequence Containers

| Container | Internals | Strengths | Weaknesses |
|-----------|-----------|-----------|------------|
| `std::vector` | Contiguous dynamic array | O(1) random access & back insert; cache-friendly | O(n) middle insert/erase; reallocation invalidates |
| `std::array` | Fixed contiguous array | Zero overhead, stack-allocated | Fixed size at compile time |
| `std::deque` | Segmented (chunks) | O(1) push front *and* back | Slower iteration than vector; no contiguity guarantee |
| `std::list` | Doubly-linked list | O(1) insert/erase anywhere with iterator; stable iterators | No random access; poor cache locality; high per-node cost |
| `std::forward_list` | Singly-linked list | Lowest list overhead | Forward-only; no `size()` (pre-C++11 style) |

> **Default to `std::vector`.** Its cache locality usually beats `list`/`deque`
> even for middle insertions on modern hardware, until profiling says otherwise.

### Associative Containers (Ordered — Balanced Trees)

| Container | Keeps | Lookup | Notes |
|-----------|-------|--------|-------|
| `std::set` | Unique sorted keys | O(log n) | In-order traversal |
| `std::multiset` | Sorted keys (dups allowed) | O(log n) | |
| `std::map` | Sorted key→value | O(log n) | Sorted iteration |
| `std::multimap` | Sorted key→value (dups) | O(log n) | |

### Unordered Associative (Hash Tables)

| Container | Lookup (avg) | Lookup (worst) | Notes |
|-----------|--------------|----------------|-------|
| `std::unordered_set` | O(1) | O(n) | No ordering |
| `std::unordered_map` | O(1) | O(n) | Needs `std::hash` for custom keys |

### Decision Flow

```text
Need key→value?
├── No (just values)
│   ├── Need sorted order?        → std::set
│   ├── Fastest membership test?  → std::unordered_set
│   └── Sequence of items?
│       ├── Random access + grow at back? → std::vector  (default)
│       ├── Grow at BOTH ends?            → std::deque
│       └── Frequent middle insert/erase
│           with stable iterators?        → std::list
└── Yes (key→value)
    ├── Need sorted keys / range queries? → std::map
    └── Fastest lookup, order irrelevant? → std::unordered_map
```

### Container Adapters

`std::stack`, `std::queue`, `std::priority_queue` wrap an underlying container to
present a restricted interface (LIFO/FIFO/heap).

```cpp
std::priority_queue<int> maxHeap;                 // max-heap by default
std::priority_queue<int, std::vector<int>,
                    std::greater<int>> minHeap;   // min-heap
```

---

## 2.2 Iterators & Invalidation

### Iterator Categories (Hierarchy)

```text
Input / Output      → single-pass, read or write (e.g., istream_iterator)
   └─ Forward       → multi-pass, ++ only            (forward_list)
        └─ Bidirectional → ++ and --                 (list, map, set)
             └─ Random Access → +n, -n, [], ordering (vector, deque, array)
                  └─ Contiguous (C++17) → elements truly contiguous (vector, array)
```

Algorithms require a **minimum** category. `std::sort` needs **random access**
(so it works on `vector`/`deque`/`array` but **not** `list` — which has its own
`list::sort` member).

### Iterator Invalidation — Critical Rules

| Container | Operation | Invalidates |
|-----------|-----------|-------------|
| `vector` | insert/push_back causing realloc | **all** iterators & references |
| `vector` | erase | iterators at/after the erase point |
| `deque` | insert/erase in middle | **all** iterators |
| `list` / `forward_list` | insert | none |
| `list` | erase | only the erased element's iterator |
| `map`/`set` | insert | none (references stable) |
| `map`/`set` | erase | only the erased element |
| `unordered_*` | rehash (on insert) | **all iterators** (references stay valid) |

### The Erase Pitfall

```cpp
// WRONG — erase invalidates 'it', then ++it is UB
for (auto it = v.begin(); it != v.end(); ++it)
    if (*it == target) v.erase(it);          // BUG

// RIGHT — erase returns the next valid iterator
for (auto it = v.begin(); it != v.end(); )
    if (*it == target) it = v.erase(it);
    else               ++it;

// BEST (C++20) — std::erase / std::erase_if
std::erase(v, target);
std::erase_if(v, [](int x){ return x % 2 == 0; });
```

---

## 2.3 The Algorithm Library

### Why Use `<algorithm>` Instead of Hand-Written Loops?

- **Correctness** — battle-tested, fewer off-by-one bugs.
- **Readability** — the name states the intent (`std::find`, `std::sort`).
- **Performance** — optimized, and parallelizable in C++17.
- **Composability** — combine with iterators, lambdas, ranges.

### Essential Algorithms by Category

```cpp
#include <algorithm>
#include <numeric>

// Non-modifying
std::find(b, e, val);            std::count_if(b, e, pred);
std::all_of(b, e, pred);         std::any_of(b, e, pred);
std::for_each(b, e, fn);         std::find_if(b, e, pred);

// Modifying
std::copy(b, e, out);            std::transform(b, e, out, fn);
std::fill(b, e, val);            std::replace(b, e, old, neu);
std::remove(b, e, val);          // "remove-erase" idiom (see below)

// Sorting & ordered ops
std::sort(b, e);                 std::stable_sort(b, e);
std::partial_sort(b, mid, e);    std::nth_element(b, nth, e);
std::binary_search(b, e, val);   // requires sorted range
std::lower_bound(b, e, val);     std::upper_bound(b, e, val);

// Numeric
std::accumulate(b, e, init);     // sequential fold (C++17 reduce = parallel)
std::inner_product(...);         std::iota(b, e, start);
```

### The Remove-Erase Idiom

`std::remove` doesn't actually shrink the container — it shifts kept elements to
the front and returns the new logical end. **Pair it with `erase`:**

```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());        // remove all 'value'
v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());      // remove matching
// C++20 one-liners: std::erase(v, value); std::erase_if(v, pred);
```

---

## 2.4 Complexity Quick Reference (Big-O)

| Operation | vector | deque | list | map/set | unordered_map/set |
|-----------|:------:|:-----:|:----:|:-------:|:-----------------:|
| Random access `[i]` | O(1) | O(1) | — | — | — |
| Insert/erase front | O(n) | **O(1)** | O(1) | O(log n) | O(1) avg |
| Insert/erase back | O(1)* | O(1) | O(1) | O(log n) | O(1) avg |
| Insert/erase middle | O(n) | O(n) | **O(1)**† | O(log n) | O(1) avg |
| Find by value/key | O(n) | O(n) | O(n) | **O(log n)** | **O(1) avg** |
| Memory locality | best | good | poor | poor | medium |

\* amortized (occasional reallocation) † with an iterator already at the position

---

## 2.5 Allocators (Overview)

Every STL container has an **allocator** template parameter controlling how it
obtains memory (default: `std::allocator<T>`, which uses `::operator new`).

```cpp
std::vector<int, MyAllocator<int>> v;   // custom memory strategy
```

Custom allocators matter for:
- **Performance** — pool/arena allocators avoid per-element `new` overhead.
- **Special memory** — shared memory, GPU memory, fixed buffers (embedded).

> Most code never needs a custom allocator. Reach for one only when profiling
> shows allocation is a bottleneck. C++17 added `std::pmr` (polymorphic memory
> resources) to make custom allocation far easier.

```cpp
#include <memory_resource>
char buffer[8192];
std::pmr::monotonic_buffer_resource pool{buffer, sizeof(buffer)};
std::pmr::vector<int> v{&pool};   // allocates from the stack buffer — no heap
```

---

## Chapter 2 Cheat Sheet

```cpp
// Choose: vector (default) | deque (both ends) | list (stable iters)
//         map/set (sorted) | unordered_* (fast lookup)

std::sort(v.begin(), v.end());            // needs random-access iterators
v.erase(std::remove(v.begin(), v.end(), x), v.end());  // remove-erase
it = v.erase(it);                          // erase returns next valid iterator

std::lower_bound(b, e, val);               // first ≥ val (sorted range)
std::accumulate(b, e, 0);                  // fold

std::priority_queue<int, std::vector<int>, std::greater<>> minHeap;
```

**Remember:**
- **Default to `vector`** — cache locality usually wins.
- `std::sort` needs random access → not usable on `list` (use `list::sort`).
- Know your **invalidation rules**; use the `erase`-returns-next pattern.
- `std::remove` doesn't erase — pair it with `erase` (or use C++20 `std::erase`).
- `unordered_map` is O(1) average but **unordered** and O(n) worst-case.

---

## Common Mistakes (STL)

1. `++it` after `erase(it)` on a vector (invalidated iterator → UB).
2. Expecting `std::remove` to actually shrink the container.
3. Calling `std::sort` on a `std::list` (no random access).
4. Relying on `unordered_map` iteration order.
5. Choosing `list` for "fast inserts" without profiling — `vector` often wins.

## When NOT to Use

- **`std::list`** unless you truly need stable iterators / O(1) splicing.
- **Custom allocators** before profiling proves allocation is the bottleneck.
- **`map`** when you never iterate in order — `unordered_map` is faster.

---

*Next: [Chapter 3 — Exception Handling & Error Models](03_exceptions_and_error_models.md)*
