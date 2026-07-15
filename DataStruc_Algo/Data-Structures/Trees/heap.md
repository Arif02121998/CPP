# File: Trees/heap.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · [traversals](traversals.md) · [binary_search_tree](binary_search_tree.md) · [balanced_trees](balanced_trees.md) · heap · [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [advanced_tree_problems](advanced_tree_problems.md)

---

## Conceptual Understanding

**Definition.** A **binary heap** is a **complete** binary tree that satisfies the **heap property**: in a **max-heap** every parent ≥ its children; in a **min-heap** every parent ≤ its children. It is stored implicitly in an array (no pointers).

**Why it exists.** It gives O(1) access to the extreme element and O(log n) insertion/removal — perfect for a **priority queue** where you repeatedly need the current best. It also underpins **heapsort** (O(n log n), in-place).

**Problem it solves.** Efficient repeated extraction of the min/max from a dynamic set (see the abstraction in [../Queues/priority_queue.md](../Queues/priority_queue.md)).

**Real-world analogy.** A tournament bracket where the champion (max) sits at the top; when they leave, the next matches promote a new champion upward.

---

## Internal Working

**Complete tree ⇒ compact array.** Fill level by level, left to right. For index `i` (0-based):
- parent = `(i-1)/2`, left = `2i+1`, right = `2i+2`.

```
max-heap [9,7,6,3,5,1]:

           9(0)
          /    \
        7(1)    6(2)
       /  \     /
     3(3) 5(4) 1(5)
```

### Sift-up (bubble-up) — used by push
Insert at the end; while it's larger than its parent (max-heap), swap upward.
### Sift-down (bubble-down) — used by pop
Move last element to the root; while it's smaller than its larger child, swap downward.

### Build-heap in O(n)
Sift-down every non-leaf node from the last parent up to the root. The tighter bound is O(n), not O(n log n), because most nodes are near the bottom and sift down only a little.

$$\sum_{h=0}^{\log n} \frac{n}{2^{h+1}} \cdot h = O(n)$$

---

## Operations

| Operation | Time |
|---|---|
| `top` / peek | O(1) |
| `push` (sift-up) | O(log n) |
| `pop` (sift-down) | O(log n) |
| build-heap (heapify) | O(n) |
| heapsort | O(n log n) |
| search arbitrary | O(n) |
| decrease/increase-key (index known) | O(log n) |

⚠️ A heap is **weakly ordered** — it does NOT support O(log n) search or in-order traversal like a BST. It only knows the extreme.

---

## C++ Implementation

### Custom max-heap on a vector
```cpp
#include <vector>
#include <algorithm>

class MaxHeap {
    std::vector<int> h;
    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (h[p] >= h[i]) break;
            std::swap(h[p], h[i]); i = p;
        }
    }
    void siftDown(int i) {
        int n = h.size();
        while (true) {
            int l = 2*i+1, r = 2*i+2, big = i;
            if (l < n && h[l] > h[big]) big = l;
            if (r < n && h[r] > h[big]) big = r;
            if (big == i) break;
            std::swap(h[i], h[big]); i = big;
        }
    }
public:
    void push(int x) { h.push_back(x); siftUp(h.size() - 1); }
    int  top() const { return h.front(); }
    void pop() { h[0] = h.back(); h.pop_back(); if (!h.empty()) siftDown(0); }
    bool empty() const { return h.empty(); }
    size_t size() const { return h.size(); }
};
```

### STL heap algorithms on a raw vector
```cpp
#include <vector>
#include <algorithm>
std::vector<int> v{3,1,4,1,5,9};
std::make_heap(v.begin(), v.end());        // O(n) → max-heap
std::pop_heap(v.begin(), v.end());         // moves max to back
int mx = v.back(); v.pop_back();           // extract it
v.push_back(2); std::push_heap(v.begin(), v.end());
std::sort_heap(v.begin(), v.end());        // heapsort → ascending
```

### STL priority_queue (the usual choice)
```cpp
#include <queue>
std::priority_queue<int> maxpq;                                   // max-heap
std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;  // min-heap
```

**When to use STL vs custom.** Use `std::priority_queue` (or the `*_heap` algorithms) for nearly everything. Hand-roll only to learn, or when you need **decrease-key with index tracking** (an indexed binary heap) for cleaner Dijkstra/Prim.

---

## Heapsort

```cpp
void heapsort(std::vector<int>& a) {
    std::make_heap(a.begin(), a.end());        // O(n)
    for (auto end = a.end(); end != a.begin(); --end)
        std::pop_heap(a.begin(), end);          // repeatedly move max to the end
}
```
- **O(n log n)** worst case, **in-place**, **not stable**.
- Slower in practice than introsort/quicksort due to cache-unfriendly access, but has no bad worst case and no extra memory.

---

## Tradeoffs
- ✅ O(1) peek, O(log n) push/pop, O(n) build, O(1) extra memory (array).
- ✅ Great for priority scheduling and top-K.
- ❌ No efficient search, ordered traversal, or range queries (use a BST for those).
- ❌ `std::priority_queue` has no decrease-key or iteration.
- **Heap vs balanced BST:** heap = faster "just the extreme"; BST = ordered iteration, predecessor/successor, arbitrary erase.

---

## Use Cases
- Priority queues; Dijkstra, Prim, A* frontiers.
- Top-K / K-th largest, K closest points, merge K sorted lists.
- Median of a stream (two heaps).
- Heapsort; Huffman coding; task/event scheduling.

---

## Pitfalls
- ⚠️ Confusing min vs max heap; STL `greater<>` gives a **min-heap** (counter-intuitive).
- ⚠️ Expecting search/sorted order from a heap — it's only partially ordered.
- ⚠️ `top()`/`pop()` on empty → UB.
- ⚠️ Building by n pushes is O(n log n); use `make_heap`/heapify for O(n).
- ⚠️ `std::priority_queue` can't decrease-key — use lazy deletion or an indexed heap.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Top K / K-th largest/smallest / K closest" → **heap of size K**.
- "Repeatedly take min/max from a changing set" → **heap**.
- "Merge K sorted sequences" → **min-heap of heads**.
- "Median of a stream" → **two heaps**.
- "Schedule by priority / earliest deadline" → **heap**.

---

## Example Problems

### Easy — K-th Largest Element in an Array
Min-heap of size k; its top is the answer. O(n log k).
```cpp
#include <queue>
int findKthLargest(std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> pq;
    for (int x : nums) { pq.push(x); if ((int)pq.size() > k) pq.pop(); }
    return pq.top();
}
```
**Explanation.** Keep only the k largest seen; the smallest of those is the k-th largest.

### Medium — K Closest Points to Origin
Max-heap of size k keyed by squared distance; pop the farthest when it exceeds k. O(n log k).

### Hard — Merge K Sorted Lists
Min-heap of current heads; pop the smallest, push its successor. O(N log k). Full code in [../Linked-Lists/advanced_problems.md](../Linked-Lists/advanced_problems.md).

---

*Next →* [segment_tree.md](segment_tree.md)
