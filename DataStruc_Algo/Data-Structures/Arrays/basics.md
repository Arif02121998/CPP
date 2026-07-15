# File: Arrays/basics.md

[← Repository Index](../README.md) · **Arrays:** basics · [operations](operations.md) · [prefix_sum](prefix_sum.md) · [sliding_window_patterns](sliding_window_patterns.md) · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** An array is a **contiguous block of memory** holding a fixed-type sequence of elements, accessed by integer indices `0 … n-1`.

**Why it exists.** It is the most fundamental data structure because it mirrors how hardware memory works: a base address plus an offset. This gives the single most valuable primitive in computing — **O(1) random access** by index.

**Problem it solves.** Whenever you need to store an ordered collection and reach any element instantly (matrices, buffers, lookup tables, the backing store for almost every other structure).

**Real-world analogy.** A row of numbered lockers. To open locker #42 you walk straight to it — you don't check lockers 1–41 first — because its position is computable from the start.

---

## Internal Working

For `int a[5]` at base address `1000` (4-byte ints):

```
index:    0     1     2     3     4
addr:   1000  1004  1008  1012  1016
        +-----+-----+-----+-----+-----+
value:  | 10  | 20  | 30  | 40  | 50  |
        +-----+-----+-----+-----+-----+
```

**Address formula:** `address(a[i]) = base + i × sizeof(element)`.

That single multiplication-and-add is *why* indexing is O(1) and *why* indices start at 0 (the first element's offset is 0).

**Cache locality (the secret weapon).** CPUs fetch memory in **cache lines** (~64 bytes). Reading `a[i]` pulls `a[i+1], a[i+2], …` into cache for free. Sequential array traversal is therefore dramatically faster than pointer-chasing structures — the #1 reason arrays often beat "asymptotically equal" linked lists in practice.

**Static vs Dynamic arrays.**
- **Static** (`int a[5]`, `std::array`): size fixed at compile time, usually on the stack, zero overhead.
- **Dynamic** (`std::vector`): heap-allocated, **grows by doubling** capacity → amortized O(1) append (see [operations.md](operations.md)).

---

## Operations

| Operation | How | Complexity |
|---|---|---|
| Access `a[i]` | address arithmetic | O(1) |
| Update `a[i]=x` | direct write | O(1) |
| Search (unsorted) | linear scan | O(n) |
| Search (sorted) | binary search | O(log n) |
| Insert/delete at end | dynamic array append | O(1) amortized |
| Insert/delete in middle | shift elements | O(n) |

**Insertion in the middle** requires shifting everything after the gap:
```
Insert 25 at index 2 of [10,20,30,40,50]:
shift 30,40,50 right → [10,20,_,30,40,50]
place 25            → [10,20,25,30,40,50]   (O(n) shifts)
```

---

## Complexity

| | Best | Worst |
|---|---|---|
| Access / Update | O(1) | O(1) |
| Search | O(1) (first) | O(n) |
| Insert/Delete (end) | O(1) | O(n) (resize copy) |
| Insert/Delete (middle/front) | O(n) | O(n) |
| **Space** | O(n) | O(n) |

---

## C++ Implementation

```cpp
#include <array>
#include <vector>

// Static array — size known at compile time, stack-allocated
int raw[5] = {10, 20, 30, 40, 50};
std::array<int, 5> sa = {10, 20, 30, 40, 50};  // safe wrapper, zero overhead

// Dynamic array — the workhorse
std::vector<int> v = {10, 20, 30};
v.push_back(40);          // amortized O(1)
v[1] = 99;                // O(1) update
int x = v.at(2);          // bounds-checked (throws std::out_of_range)
size_t n = v.size();      // logical size
v.reserve(1000);          // pre-allocate to avoid reallocations
```

**STL equivalent.**
- `std::array<T,N>` — fixed size, no overhead. Use when size is a compile-time constant.
- `std::vector<T>` — dynamic array; **use this 95% of the time**.

**When to use STL vs custom.** Always prefer `std::vector`/`std::array`. Write a raw array only for interop, embedded/no-STL environments, or specialized memory layouts (SIMD alignment).

---

## Tradeoffs

- ✅ O(1) access, excellent cache locality, minimal per-element overhead, dead simple.
- ❌ Expensive middle insert/delete (O(n) shifting); fixed size (static) or amortized resize cost (dynamic); contiguous memory must be available.
- **Performance vs memory:** `reserve` trades a bit of memory for far fewer reallocations.
- **Simplicity vs flexibility:** arrays are simple but rigid; linked lists are flexible but slower and heavier.

---

## Use Cases

- Matrices / grids / images / audio buffers.
- Lookup tables, DP tables, frequency counts.
- Backing store for stacks, heaps, hash tables, ring buffers.
- Any "list of things" dominated by indexing and iteration.

---

## Pitfalls

- ⚠️ **Out-of-bounds access is undefined behavior** in C++ (no automatic error). Use `.at()` for bounds checking.
- ⚠️ **Iterator/pointer invalidation:** `push_back` may reallocate and invalidate *all* existing iterators/pointers/references. Never cache a pointer across a possible resize.
- ⚠️ Confusing `size()` (elements) with `capacity()` (allocated slots).
- ⚠️ Forgetting `reserve()` when the final size is known → repeated reallocations.
- ⚠️ Off-by-one: `a[n]` is invalid for size `n`.

---

## Problem Patterns

🎯 **Recognition hints — reach for array techniques when you see:**
- "Subarray" / "contiguous" → [sliding window](sliding_window_patterns.md) or [prefix sum](prefix_sum.md).
- "Sorted array" + pair/triplet → two pointers / binary search.
- "In-place, O(1) extra space" → two pointers, cyclic sort.
- "Range sum/update queries" → [prefix sums](prefix_sum.md) / Fenwick / segment tree.

**Common problem types:** rotations, in-place dedup, partitioning, max-subarray, range queries, matrix traversal.

---

## Example Problems

### Easy — Running Sum of 1D Array
Return `out[i] = a[0]+…+a[i]`.
```cpp
std::vector<int> runningSum(std::vector<int> a) {
    for (size_t i = 1; i < a.size(); ++i) a[i] += a[i-1];
    return a;                              // O(n) time, O(1) extra
}
```
**Explanation.** Each element accumulates the previous prefix — a one-pass prefix sum.

### Medium — Rotate Array by k (in place)
**Brute force:** rotate one step k times → O(n·k).
**Optimized (reversal trick):** reverse whole, reverse first k, reverse rest → O(n)/O(1).
```cpp
#include <algorithm>
void rotate(std::vector<int>& a, int k) {
    int n = a.size(); k %= n;
    std::reverse(a.begin(), a.end());
    std::reverse(a.begin(), a.begin() + k);
    std::reverse(a.begin() + k, a.end());
}
```
**Explanation.** Reversing three times repositions every element in O(n) with no extra array. ⚠️ `k %= n` handles `k > n`.

### Hard — Product of Array Except Self (no division)
**Brute force:** for each i multiply all others → O(n²).
**Optimized:** prefix product × suffix product in two passes → O(n)/O(1) extra.
```cpp
std::vector<int> productExceptSelf(const std::vector<int>& a) {
    int n = a.size();
    std::vector<int> res(n, 1);
    int pre = 1;
    for (int i = 0; i < n; ++i) { res[i] = pre; pre *= a[i]; }
    int suf = 1;
    for (int i = n - 1; i >= 0; --i) { res[i] *= suf; suf *= a[i]; }
    return res;
}
```
**Explanation.** `res[i]` = (product of everything left of i) × (product of everything right). Two passes avoid division and the O(n²) loop. Handles zeros naturally.

---

*Next →* [operations.md](operations.md)
