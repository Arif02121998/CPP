# Chapter 8 — Cheatsheets

[← Back to Table of Contents](README.md)

> Quick-revision tables. Skim these the night before an interview or contest.

---

## 8.1 Data Structure Complexity (typical / average)

| Structure | Access | Search | Insert | Delete | Space | Notes |
|---|---|---|---|---|---|---|
| Array / `vector` | O(1) | O(n) | O(n)* | O(n) | O(n) | *O(1) amortized at end |
| Sorted array | O(1) | O(log n) | O(n) | O(n) | O(n) | binary search |
| Singly Linked List | O(n) | O(n) | O(1)† | O(1)† | O(n) | †at head / given node |
| Doubly Linked List | O(n) | O(n) | O(1)† | O(1)† | O(n) | both ends O(1) |
| Stack | O(n) | O(n) | O(1) | O(1) | O(n) | top only |
| Queue / Deque | O(n) | O(n) | O(1) | O(1) | O(n) | ends only |
| Hash Table | — | O(1) | O(1) | O(1) | O(n) | O(n) worst case |
| Balanced BST (`set`/`map`) | — | O(log n) | O(log n) | O(log n) | O(n) | sorted, range queries |
| Heap | O(1) top | O(n) | O(log n) | O(log n) | O(n) | min/max only; build O(n) |
| Trie | — | O(L) | O(L) | O(L) | O(Σ·N) | L = key length |
| Segment Tree | — | O(log n) | O(log n) | — | O(n) | range query + update |
| Fenwick (BIT) | — | O(log n) | O(log n) | — | O(n) | prefix sums |
| Union-Find | — | α(n) | α(n) | — | O(n) | α ≈ O(1) |

---

## 8.2 Sorting Algorithm Summary

| Algorithm | Best | Average | Worst | Space | Stable | In-place | Use when |
|---|---|---|---|---|---|---|---|
| Bubble | O(n) | O(n²) | O(n²) | O(1) | ✅ | ✅ | teaching only |
| Insertion | O(n) | O(n²) | O(n²) | O(1) | ✅ | ✅ | small / nearly sorted |
| Selection | O(n²) | O(n²) | O(n²) | O(1) | ❌ | ✅ | minimize swaps |
| Merge | O(n log n) | O(n log n) | O(n log n) | O(n) | ✅ | ❌ | stability, linked lists, external |
| Quick | O(n log n) | O(n log n) | O(n²) | O(log n) | ❌ | ✅ | general fastest (default) |
| Heap | O(n log n) | O(n log n) | O(n log n) | O(1) | ❌ | ✅ | guaranteed, in-place |
| Counting | O(n+k) | O(n+k) | O(n+k) | O(k) | ✅ | ❌ | small integer range k |
| Radix | O(d·n) | O(d·n) | O(d·n) | O(n+k) | ✅ | ❌ | fixed-width integers |

`std::sort` = introsort (quick + heap fallback) → O(n log n) guaranteed. `std::stable_sort` → stable O(n log n).

---

## 8.3 Algorithm Paradigm Quick Reference

| Paradigm | Signature complexity | Trigger words |
|---|---|---|
| Binary Search | O(log n) | sorted, monotonic, "min/max feasible" |
| Two Pointers | O(n) | sorted array, pair/triplet, in-place |
| Sliding Window | O(n) | contiguous subarray/substring, window |
| Backtracking | O(2ⁿ)/O(n!) | all combinations/permutations/subsets |
| Greedy | O(n log n) | "min/max", local choice provably global |
| DP | states × work | overlapping subproblems, "# ways/min cost" |
| BFS | O(V+E) | shortest unweighted, levels, min steps |
| DFS | O(V+E) | connectivity, cycles, topo sort, paths |
| Dijkstra | O((V+E)log V) | weighted shortest path, non-negative |
| Bellman-Ford | O(VE) | negative edges, cycle detection |
| Floyd-Warshall | O(V³) | all-pairs shortest path |
| Kruskal/Prim (MST) | O(E log V) | min cost to connect all |
| Topological Sort | O(V+E) | dependencies, ordering, prerequisites |
| Divide & Conquer | Master Theorem | independent subproblems |

---

## 8.4 Pattern → Technique Map (Interview)

| Signal | Pattern |
|---|---|
| sorted array, find pair | two pointers / binary search |
| contiguous window | sliding window |
| seen before? / frequency / dedup | hash set / map |
| all arrangements | backtracking |
| # ways / min-max with choices | DP |
| shortest unweighted path | BFS |
| weighted shortest path | Dijkstra |
| dependencies / ordering | topological sort |
| connected components / merge | union-find |
| top K / Kth largest / stream median | heap |
| prefix / autocomplete | trie |
| range query + update | segment tree / BIT |
| next greater/smaller | monotonic stack |
| max/min in moving window | monotonic deque |
| cycle / middle of linked list | fast & slow pointers |
| overlapping intervals | sort + greedy / sweep line |
| search the answer | binary search on answer |

---

## 8.5 Big-O Growth Reference

| n | log₂ n | n | n log n | n² | 2ⁿ |
|---|---|---|---|---|---|
| 10 | 3 | 10 | 33 | 100 | 1,024 |
| 100 | 7 | 100 | 664 | 10,000 | 10³⁰ |
| 1,000 | 10 | 1,000 | ~10⁴ | 10⁶ | huge |
| 10⁶ | 20 | 10⁶ | ~2×10⁷ | 10¹² | — |

**Rule of thumb (≈10⁸ simple ops/sec budget):**

| Max n | Feasible complexity |
|---|---|
| ≤ 12 | O(n!) |
| ≤ 25 | O(2ⁿ) |
| ≤ 500 | O(n³) |
| ≤ 10⁴ | O(n²) |
| ≤ 10⁶ | O(n log n) |
| ≤ 10⁸ | O(n) |
| 10¹⁸ | O(log n) / O(1) |

---

## 8.6 STL Container Quick Picks

| Container | Underlying | Key ops | When |
|---|---|---|---|
| `vector` | dynamic array | `push_back`, `[]`, `at` | default sequence |
| `array` | static array | `[]` | fixed compile-time size |
| `deque` | block array | `push_front/back` | both-end queue |
| `list` | doubly linked | `splice`, `insert`/`erase` | O(1) node ops, no indexing |
| `stack` | deque adapter | `push/pop/top` | LIFO |
| `queue` | deque adapter | `push/pop/front` | FIFO |
| `priority_queue` | heap | `push/pop/top` | min/max priority |
| `set`/`map` | red-black tree | `insert/find/lower_bound` | sorted, range queries |
| `unordered_set/map` | hash table | `insert/find/count` | fastest lookup |

**Min-heap idiom:** `priority_queue<int, vector<int>, greater<int>>`.

---

## 8.7 STL Algorithm Quick Picks

```cpp
std::sort(v.begin(), v.end());                 // O(n log n)
std::stable_sort(v.begin(), v.end());          // stable
std::binary_search(v.begin(), v.end(), x);     // bool, sorted
std::lower_bound(v.begin(), v.end(), x);       // first >= x
std::upper_bound(v.begin(), v.end(), x);       // first > x
std::nth_element(v.begin(), v.begin()+k, v.end()); // O(n) kth
std::partial_sort(v.begin(), v.begin()+k, v.end()); // top-k sorted
std::max_element(v.begin(), v.end());
std::accumulate(v.begin(), v.end(), 0LL);      // sum (#include <numeric>)
std::unique(v.begin(), v.end());               // dedup consecutive (sort first)
std::reverse(v.begin(), v.end());
std::next_permutation(v.begin(), v.end());     // generate permutations
std::__gcd(a, b);                              // or std::gcd in C++17
```

---

## 8.8 Recurrence / Master Theorem Quick Card

$T(n) = a\,T(n/b) + f(n)$, let $c = \log_b a$:

| Compare f(n) to n^c | Result |
|---|---|
| f(n) = O(n^{c−ε}) | Θ(n^c) |
| f(n) = Θ(n^c) | Θ(n^c log n) |
| f(n) = Ω(n^{c+ε}) | Θ(f(n)) |

- Merge sort: 2T(n/2)+O(n) → **Θ(n log n)**
- Binary search: T(n/2)+O(1) → **Θ(log n)**
- Karatsuba: 3T(n/2)+O(n) → **Θ(n^{1.585})**

---

## 8.9 Final Revision Checklist

**Before an interview, can you implement from memory in < 5 min each?**
- [ ] Binary search (+ lower_bound)
- [ ] Merge sort & quicksort partition
- [ ] BFS & DFS (graph + grid)
- [ ] Dijkstra with min-heap
- [ ] Reverse a linked list
- [ ] Tree traversals (all 4)
- [ ] Validate BST
- [ ] Heap push/pop (or use priority_queue)
- [ ] Union-Find with path compression
- [ ] DP: knapsack, LCS, coin change
- [ ] Sliding window template
- [ ] Backtracking template (subsets/permutations)

**Always state:** time complexity, space complexity, and one edge case you handled.

---

[← Back to Table of Contents](README.md)

*End of book. Practice deliberately, name your patterns, and explain your reasoning out loud.*
