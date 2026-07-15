# File: Arrays/operations.md

[← Repository Index](../README.md) · **Arrays:** [basics](basics.md) · operations · [prefix_sum](prefix_sum.md) · [sliding_window_patterns](sliding_window_patterns.md) · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** Array *operations* are the primitive manipulations — access, update, insert, delete, search, traverse, resize, rotate — that every higher-level algorithm composes.

**Why this matters.** Knowing the **exact cost** of each operation lets you choose the right structure and avoid hidden O(n) traps (e.g., inserting at the front of a vector in a loop → O(n²)).

**Problem it solves.** Gives you a precise mental cost model so you can reason about performance before writing a single benchmark.

**Real-world analogy.** Editing a printed list on paper: appending at the bottom is cheap; inserting a line in the middle forces you to rewrite everything below it.

---

## Internal Working — Dynamic Array Growth

The crucial internal mechanism is **amortized doubling**. When a `std::vector` is full and you `push_back`, it:
1. Allocates a new block of **2× capacity**.
2. **Copies/moves** all existing elements.
3. Frees the old block.

```
cap=1: [a]
push → cap=2: [a,b]
push → cap=4: [a,b,c,_]
push×2 → full → cap=8: [a..f,_,_]
```

Total copy work over n pushes: `1+2+4+…+n ≈ 2n = O(n)` → **O(1) amortized per push_back**.

💡 Growing by a *constant factor* (×2) gives O(1) amortized. Growing by a *constant amount* (+1) would be O(n) amortized — quadratic total. This is why `reserve()` matters when you know the size.

---

## Operations — Step by Step

### Access / Update — O(1)
Direct address arithmetic; no traversal.

### Search
- **Unsorted:** linear scan, O(n).
- **Sorted:** binary search, O(log n).

### Insert
- **At end:** `push_back` — O(1) amortized.
- **At index i:** shift `[i..n)` right by one, then write — O(n).

```
insert(2, 99) in [10,20,30,40]:
shift 30,40 → [10,20,_,30,40]
write 99    → [10,20,99,30,40]
```

### Delete
- **At end:** `pop_back` — O(1).
- **At index i:** shift `[i+1..n)` left by one — O(n).

### Traverse — O(n)
Range-for is idiomatic and cache-friendly.

### Rotate — O(n)
Reversal trick (see Example Problems below).

---

## Complexity

| Operation | Best | Worst | Notes |
|---|---|---|---|
| Access/Update | O(1) | O(1) | |
| Search unsorted | O(1) | O(n) | |
| Search sorted | O(1) | O(log n) | binary search |
| push_back / pop_back | O(1) | O(n)* | *resize copy, amortized O(1) |
| insert/erase middle | O(n) | O(n) | shifting |
| Space | O(n) | O(n) | capacity ≥ size |

---

## C++ Implementation

```cpp
#include <vector>
#include <algorithm>

std::vector<int> v = {10, 20, 30, 40};

// Insert / erase (O(n) — shifting)
v.insert(v.begin() + 2, 99);     // {10,20,99,30,40}
v.erase(v.begin() + 2);          // {10,20,30,40}

// End operations (O(1) amortized)
v.push_back(50);
v.pop_back();

// Search
auto it = std::find(v.begin(), v.end(), 30);          // O(n)
bool ok = std::binary_search(v.begin(), v.end(), 30); // O(log n), sorted only

// Idiomatic erase: remove-erase idiom (delete all 20s in O(n))
v.erase(std::remove(v.begin(), v.end(), 20), v.end());

// Capacity management
v.reserve(1000);   // pre-allocate
v.shrink_to_fit(); // release unused capacity
```

**STL equivalent / when to use.** `std::vector` already provides all operations optimally. Use raw arrays only for fixed compile-time sizes or interop. ⚠️ Never reimplement dynamic-array growth yourself in production.

---

## Tradeoffs

- ✅ End operations are cheap; access is O(1).
- ❌ Middle/front operations are O(n); a front-insert loop is a classic O(n²) bug.
- **Performance vs memory:** capacity ≥ size means a vector may hold up to ~2× memory after growth; `shrink_to_fit` reclaims it.
- **Simplicity vs flexibility:** if you need cheap front/middle inserts, consider `std::deque` (both ends O(1)) or `std::list` (O(1) at a known node).

---

## Use Cases
- Append-heavy logs/buffers (`push_back`).
- Stacks (`push_back`/`pop_back`).
- Any workload dominated by indexing + iteration.

---

## Pitfalls

- ⚠️ **`vector::insert/erase` invalidates iterators** at/after the point of change.
- ⚠️ **Erasing in a loop** with `++it` after `erase` skips elements — use the returned iterator: `it = v.erase(it);`.
- ⚠️ **Front insert/erase is O(n)** — use `std::deque` if you need it often.
- ⚠️ Resizing inside a range-based loop over the same vector → UB.
- ⚠️ `reserve` changes capacity, not size; `resize` changes size (and value-initializes).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Remove/move elements in place, O(1) space" → **two pointers** (slow/fast).
- "Rotate / reverse" → **reversal trick**.
- "Delete matching elements efficiently" → **remove-erase idiom**.

**Common problem types:** in-place removal, partitioning, rotation, merging sorted arrays.

---

## Example Problems

### Easy — Remove Element In Place
Remove all occurrences of `val`, return new length.
**Brute force:** build a new array → O(n)/O(n).
**Optimized (two pointers):** O(n)/O(1).
```cpp
int removeElement(std::vector<int>& a, int val) {
    int k = 0;                              // next write position
    for (int x : a) if (x != val) a[k++] = x;
    return k;
}
```
**Explanation.** `k` writes only kept elements; everything past `k` is discarded.

### Medium — Merge Sorted Array (in place, from the back)
`a` has size m+n with n zeros at the end; merge sorted `b` into it.
**Brute force:** concatenate + sort → O((m+n)log(m+n)).
**Optimized:** fill from the back to avoid overwrites → O(m+n)/O(1).
```cpp
void merge(std::vector<int>& a, int m, std::vector<int>& b, int n) {
    int i = m - 1, j = n - 1, k = m + n - 1;
    while (j >= 0)
        a[k--] = (i >= 0 && a[i] > b[j]) ? a[i--] : b[j--];
}
```
**Explanation.** Writing largest-first into the tail never clobbers unprocessed `a` values.

### Hard — Rotate Array by k in O(1) space
**Brute force:** rotate one step k times → O(n·k).
**Optimized:** triple reversal → O(n)/O(1).
```cpp
void rotate(std::vector<int>& a, int k) {
    int n = a.size(); k %= n;
    std::reverse(a.begin(), a.end());
    std::reverse(a.begin(), a.begin() + k);
    std::reverse(a.begin() + k, a.end());
}
```
**Explanation.** Reverse all, then reverse the two parts to restore internal order at the rotated position. ⚠️ `k %= n` for `k ≥ n`.

---

*Next →* [prefix_sum.md](prefix_sum.md)
