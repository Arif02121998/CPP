# File: Queues/priority_queue.md

[← Repository Index](../README.md) · **Queues:** [queue](queue.md) · [deque](deque.md) · priority_queue · [monotonic_queue](monotonic_queue.md)

---

## Conceptual Understanding

**Definition.** A **priority queue** serves elements by **priority** rather than arrival order: the highest- (or lowest-) priority element is always removed next. It is almost universally implemented with a **binary heap**.

**Why it exists.** Many algorithms repeatedly need "the current best/smallest/largest" — Dijkstra, Prim, scheduling, top-K. Scanning for it each time is O(n); a heap gives O(log n) insertion and O(1) peek.

**Problem it solves.** Efficient *repeated* extraction of the extreme element from a changing set.

**Real-world analogy.** A hospital emergency room: patients are treated by severity, not arrival order. A critical patient jumps ahead of everyone waiting.

> Deep dive on the heap data structure itself (sift-up/down, heapify, heapsort) lives in [../Trees/heap.md](../Trees/heap.md). This file focuses on the **priority-queue abstraction** and its C++ usage.

---

## Internal Working

A binary heap is a complete binary tree stored in an array. For a **max-heap**, every parent ≥ its children (min-heap: parent ≤ children). Array indexing (0-based):
- parent(i) = (i−1)/2, left(i) = 2i+1, right(i) = 2i+2.

```
max-heap array [9,7,6,3,5,1]:

            9
          /   \
         7     6
        / \   /
       3   5 1
```

- **push:** append at the end, **sift up** while it beats its parent → O(log n).
- **pop (extract top):** swap root with last, remove last, **sift down** the new root → O(log n).
- **top:** read index 0 → O(1).
- **build-heap (heapify):** from an unordered array in O(n) (not O(n log n)).

---

## Operations

| Operation | Time |
|---|---|
| `top()` (peek extreme) | O(1) |
| `push(x)` | O(log n) |
| `pop()` (remove extreme) | O(log n) |
| build from n items | O(n) |
| `size()` / `empty()` | O(1) |

---

## C++ Implementation

```cpp
#include <queue>
#include <vector>

// Default: MAX-heap (largest on top)
std::priority_queue<int> maxpq;
maxpq.push(3); maxpq.push(9); maxpq.push(5);
int big = maxpq.top();   // 9
maxpq.pop();             // removes 9

// MIN-heap: greater<> comparator
std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;
minpq.push(3); minpq.push(9); minpq.push(5);
int small = minpq.top(); // 3
```

**Custom comparator (e.g., by second field, min by distance):**
```cpp
#include <queue>
#include <vector>
using P = std::pair<int,int>;                       // {distance, node}
auto cmp = [](const P& a, const P& b){ return a.first > b.first; }; // min-heap by distance
std::priority_queue<P, std::vector<P>, decltype(cmp)> pq(cmp);
pq.push({0, src});
```
⚠️ The comparator returns **true when `a` should come *after* `b`**. So `a.first > b.first` yields a **min-heap** (counter-intuitive).

**STL note.** `std::priority_queue` does **not** support decrease-key or iteration. For Dijkstra, the common workaround is to push duplicates and skip stale ones (lazy deletion). For decrease-key, use an indexed heap or `std::set`.

**When to use STL vs custom.** Use `std::priority_queue` for nearly everything. Build a custom indexed binary heap when you need **decrease-key** (cleaner Prim/Dijkstra) or O(1) membership checks.

---

## Tradeoffs
- ✅ O(log n) push/pop, O(1) peek, O(n) build — far better than sorting repeatedly.
- ✅ Compact array storage, good locality.
- ❌ No efficient search/decrease-key/iteration in `std::priority_queue`.
- ❌ Not stable (equal-priority order is unspecified).
- **Heap vs balanced BST (`std::set`):** heap is faster for "just the extreme"; a BST also supports ordered iteration, predecessor/successor, and arbitrary erase.

---

## Use Cases
- **Dijkstra** and **Prim's MST** (min-heap of frontier).
- **Top-K** / K-th largest/smallest, K closest points.
- **Merge K sorted lists/arrays** (min-heap of heads).
- Median maintenance (two heaps).
- Task schedulers, Huffman coding, A* search.

---

## Pitfalls
- ⚠️ Comparator direction confusion (`greater<>` → min-heap; lambda returns "a after b").
- ⚠️ Expecting FIFO among equal priorities — it's **not stable**.
- ⚠️ Calling `top()`/`pop()` on an empty PQ → UB.
- ⚠️ Trying to update a key in place — not supported; use lazy deletion or an indexed heap.
- ⚠️ Forgetting the min-heap for shortest-path/MST (a max-heap gives wrong frontier).

---

## Problem Patterns

🎯 **Recognition hints:**
- "K-th largest/smallest," "top K," "K closest" → **heap of size K**.
- "Repeatedly take the min/max" → **priority queue**.
- "Merge K sorted ..." → **min-heap of K heads**.
- "Find/maintain the median of a stream" → **two heaps** (max-heap low half, min-heap high half).
- "Shortest path / MST" → **min-heap** (Dijkstra/Prim).

---

## Example Problems

### Easy — K-th Largest Element in a Stream
Keep a **min-heap of size k**; its top is the k-th largest.
```cpp
#include <queue>
#include <vector>
class KthLargest {
    int k;
    std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;
public:
    KthLargest(int k_, std::vector<int>& nums) : k(k_) {
        for (int x : nums) add(x);
    }
    int add(int val) {
        minpq.push(val);
        if ((int)minpq.size() > k) minpq.pop();   // keep only k largest
        return minpq.top();
    }
};
```
**Explanation.** Discard everything below the top k; the smallest of those k is the answer. O(log k) per add.

### Medium — Top K Frequent Elements
Count with a hash map, then a min-heap of size k keyed by frequency → O(n log k).

### Hard — Find Median from Data Stream
Two heaps: a max-heap for the lower half, a min-heap for the upper half, balanced in size.
```cpp
#include <queue>
class MedianFinder {
    std::priority_queue<int> lo;                                  // max-heap (lower half)
    std::priority_queue<int, std::vector<int>, std::greater<int>> hi; // min-heap (upper half)
public:
    void addNum(int num) {
        lo.push(num);
        hi.push(lo.top()); lo.pop();                 // balance values
        if (hi.size() > lo.size()) { lo.push(hi.top()); hi.pop(); } // balance sizes
    }
    double findMedian() {
        if (lo.size() > hi.size()) return lo.top();
        return (lo.top() + hi.top()) / 2.0;
    }
};
```
**Explanation.** The two heaps partition the data at the median; tops give the middle element(s). O(log n) insert, O(1) query.

---

*Next →* [monotonic_queue.md](monotonic_queue.md)
