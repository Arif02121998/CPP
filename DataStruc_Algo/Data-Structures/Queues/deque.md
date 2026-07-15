# File: Queues/deque.md

[← Repository Index](../README.md) · **Queues:** [queue](queue.md) · deque · [priority_queue](priority_queue.md) · [monotonic_queue](monotonic_queue.md)

---

## Conceptual Understanding

**Definition.** A **deque** (double-ended queue, pronounced "deck") allows insertion and removal at **both** the front and the back in O(1). It generalizes both the stack (one end) and the queue (two ends, opposite operations).

**Why it exists.** Some algorithms need to add/remove from either end — sliding-window extremes, undo/redo with bounded history, work-stealing schedulers. A deque provides all four end-operations at O(1).

**Problem it solves.** Any "both ends matter" workload, and it serves as the building block for `std::queue` and `std::stack` and the monotonic-deque technique.

**Real-world analogy.** A double-ended ticket line where VIPs can join the front and walk-ins join the back, and people can leave from either end.

---

## Internal Working

`std::deque` is **not** one contiguous array. It's a **sequence of fixed-size blocks** ("chunks") tracked by a central map (array of block pointers). This gives:
- O(1) `push_front`/`push_back` (allocate/advance into a block; grow the map rarely),
- O(1) random access via index arithmetic across blocks,
- but **non-contiguous** storage (no `&d[0]` as a C array; iterators invalidate differently than vector).

```
map → [blk0][blk1][blk2] ...
blk1:  [ _ , x0, x1, x2 ]   front grows leftward, back grows rightward
```

A simple custom deque can also be a **doubly linked list** (O(1) ends, O(n) random access) or a **circular buffer** that grows from both ends.

---

## Operations (all O(1))

| Operation | Meaning |
|---|---|
| `push_front(x)` / `push_back(x)` | insert at front / back |
| `pop_front()` / `pop_back()` | remove front / back |
| `front()` / `back()` | read ends |
| `operator[]` / `at()` | random access (deque only) |

---

## Complexity

| Operation | std::deque | Doubly linked list |
|---|---|---|
| push/pop front & back | O(1) | O(1) |
| random access `[i]` | O(1) | O(n) |
| insert/erase middle | O(n) | O(1) at known node |
| cache locality | good (block-local) | poor |

---

## C++ Implementation

```cpp
#include <deque>

std::deque<int> d;
d.push_back(2);    // [2]
d.push_front(1);   // [1,2]
d.push_back(3);    // [1,2,3]
int f = d.front(); // 1
int b = d.back();  // 3
d.pop_front();     // [2,3]
d.pop_back();      // [2]
int mid = d[0];    // random access
```

**STL equivalent.** `std::deque<T>` is the standard double-ended queue and the **default backing container** for `std::stack` and `std::queue`.

**When to use STL vs custom.** Use `std::deque` for general work. Use a custom circular buffer when you need a strict fixed capacity (e.g., a bounded ring buffer) or a doubly linked list when you need O(1) middle insert/erase with stable node references.

💡 **vector vs deque:** prefer `vector` for pure back-insertion and contiguous memory; choose `deque` when you also need cheap front operations.

---

## Tradeoffs
- ✅ O(1) at both ends *and* O(1) random access.
- ✅ No large reallocation/copy on growth (blocks added incrementally) → more stable than `vector` for front growth.
- ❌ Non-contiguous → slightly worse locality than `vector`; can't pass as a C array.
- ❌ Higher per-element bookkeeping than `vector`.

---

## Use Cases
- **Sliding-window maximum/minimum** (monotonic deque — see [monotonic_queue.md](monotonic_queue.md)).
- Backing store for stacks and queues.
- Undo/redo with a bounded buffer (drop from the far end).
- Work-stealing schedulers (push/pop one end locally, steal from the other).
- Palindrome checking (compare both ends inward).

---

## Pitfalls
- ⚠️ Don't assume contiguous memory — `&d[0]` is not a valid C array pointer for the whole deque.
- ⚠️ Iterator invalidation rules differ from `vector` (insertion at ends keeps element refs valid but invalidates iterators).
- ⚠️ Guard `front()/back()/pop_*` against empty (UB).
- ⚠️ Random middle insert/erase is O(n) — use `std::list` if that's frequent.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Window min/max in O(1) amortized" → **monotonic deque**.
- "Add/remove from both ends" → **deque**.
- "Palindrome / two-ends-inward" → **deque** (or two pointers).
- "0-1 BFS (edge weights 0 or 1)" → **deque** (push_front for weight 0, push_back for weight 1).

---

## Example Problems

### Easy — Palindrome check with a deque
```cpp
#include <deque>
#include <string>
bool isPalindrome(const std::string& s) {
    std::deque<char> d(s.begin(), s.end());
    while (d.size() > 1) {
        if (d.front() != d.back()) return false;
        d.pop_front(); d.pop_back();
    }
    return true;
}
```
**Explanation.** Compare and shrink from both ends — natural deque use. O(n).

### Medium — Design Circular Deque
Implement `insertFront/insertLast/deleteFront/deleteLast/getFront/getRear` on a fixed-capacity ring with O(1) ops — same wrap-around idea as the circular queue ([queue.md](queue.md)).

### Hard — Sliding Window Maximum
Maintain a deque of indices whose values are **decreasing**; the front is always the window max. O(n) total — full solution in [monotonic_queue.md](monotonic_queue.md).

---

## 0-1 BFS Note
When edge weights are only 0 or 1, a deque replaces a priority queue: push 0-weight relaxations to the **front**, 1-weight to the **back**, giving Dijkstra-like results in O(V+E). See [../Graphs/shortest_path.md](../Graphs/shortest_path.md).

---

*Next →* [priority_queue.md](priority_queue.md)
