# File: Trees/segment_tree.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · [traversals](traversals.md) · [binary_search_tree](binary_search_tree.md) · [balanced_trees](balanced_trees.md) · [heap](heap.md) · segment_tree · [fenwick_tree](fenwick_tree.md) · [advanced_tree_problems](advanced_tree_problems.md)

> This file covers the **fundamentals** (point update, range query). Lazy propagation, range updates, and 2D/persistent variants live in [../Advanced/segment_tree.md](../Advanced/segment_tree.md).

---

## Conceptual Understanding

**Definition.** A **segment tree** is a binary tree over an array where each node stores an **aggregate** (sum, min, max, gcd, …) of a contiguous **segment** of the array. Leaves are single elements; each internal node combines its two children's ranges.

**Why it exists.** Consider "answer many range queries *and* update elements." A plain array gives O(1) update but O(n) range query; a prefix-sum array gives O(1) query but O(n) update. A segment tree does **both in O(log n)**.

**Problem it solves.** Dynamic range queries with updates: range sum/min/max/gcd while individual elements change.

**Real-world analogy.** A management hierarchy where each manager keeps a summary (total sales) of their whole team; when one employee's number changes, only their chain of managers updates — not everyone.

---

## Internal Working

For array size n, build a tree of ~2·(next power of two) nodes (commonly `4n` array slots to be safe).

```
array = [1, 3, 5, 7]   (range-sum segment tree)

                 [0..3]=16
                /         \
          [0..1]=4      [2..3]=12
          /    \         /     \
       [0]=1 [1]=3    [2]=5  [3]=7
```

- **Build:** recurse; each internal node = combine(left child, right child). O(n).
- **Query(l, r):** descend; if a node's range is fully inside [l,r], return its value; if disjoint, return the identity; else combine partial results from both children. O(log n).
- **Point update(i, v):** update the leaf, then recombine ancestors on the way up. O(log n).

💡 Each query/update touches at most O(log n) nodes because the query range decomposes into O(log n) canonical segments.

---

## Operations

| Operation | Time |
|---|---|
| build | O(n) |
| range query | O(log n) |
| point update | O(log n) |
| range update (needs lazy) | O(log n) — see Advanced |
| Space | O(n) |

The aggregate must be **associative** with an identity (a monoid): sum (0), min (+∞), max (−∞), gcd (0), product (1), etc.

---

## C++ Implementation — Range Sum, Point Update

```cpp
#include <vector>

class SegmentTree {
    int n;
    std::vector<long long> tree;      // 1-indexed, size 4n

    void build(const std::vector<int>& a, int node, int lo, int hi) {
        if (lo == hi) { tree[node] = a[lo]; return; }
        int mid = (lo + hi) / 2;
        build(a, 2*node,   lo,    mid);
        build(a, 2*node+1, mid+1, hi);
        tree[node] = tree[2*node] + tree[2*node+1];      // combine
    }
    void update(int node, int lo, int hi, int idx, int val) {
        if (lo == hi) { tree[node] = val; return; }
        int mid = (lo + hi) / 2;
        if (idx <= mid) update(2*node,   lo,    mid, idx, val);
        else            update(2*node+1, mid+1, hi,  idx, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }
    long long query(int node, int lo, int hi, int l, int r) {
        if (r < lo || hi < l) return 0;                  // disjoint → identity
        if (l <= lo && hi <= r) return tree[node];       // fully inside
        int mid = (lo + hi) / 2;
        return query(2*node,   lo,    mid, l, r)
             + query(2*node+1, mid+1, hi,  l, r);        // partial
    }
public:
    SegmentTree(const std::vector<int>& a) : n(a.size()), tree(4 * a.size()) {
        build(a, 1, 0, n - 1);
    }
    void update(int idx, int val) { update(1, 0, n - 1, idx, val); }
    long long query(int l, int r) { return query(1, 0, n - 1, l, r); }  // inclusive
};
```

**Generalize** by replacing `+` and the identity `0`: for range-min use `std::min` with identity `LLONG_MAX`.

**STL note.** No standard segment tree. For a **static** array (no updates) prefer a [sparse table](../Advanced/sparse_table.md) (O(1) idempotent queries) or prefix sums. For **point-update + prefix-sum only**, a [Fenwick tree](fenwick_tree.md) is smaller and faster.

---

## Tradeoffs
- ✅ O(log n) for *both* updates and range queries; supports any associative aggregate.
- ✅ More flexible than Fenwick (min/max/gcd/assign, range updates with lazy).
- ❌ ~4n memory and a larger constant factor than a Fenwick tree.
- ❌ More code; recursion overhead (iterative versions exist).
- **vs prefix sums:** prefix sums are O(1) query but O(n) update and sum-only.
- **vs Fenwick:** Fenwick is simpler/faster for sum/prefix; segment tree handles more operations.

---

## Use Cases
- Range sum/min/max/gcd with updates.
- Range assignment/addition (lazy propagation — Advanced).
- Counting/order-statistics (merge-sort tree, wavelet-like uses).
- Computational geometry sweeps; interval stabbing.
- Game/graphics: range brightness, terrain aggregates.

---

## Pitfalls
- ⚠️ Under-sizing the `tree` array — use `4n` to be safe.
- ⚠️ Wrong **identity** for the disjoint case (0 for sum, +∞ for min, −∞ for max).
- ⚠️ Inclusive vs exclusive range bugs; mixing 0- and 1-based indexing.
- ⚠️ Forgetting to recombine ancestors after a point update.
- ⚠️ Using segment tree where prefix sums / Fenwick suffice (over-engineering).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Range sum/min/max **and** updates" → **segment tree**.
- "Range assign/add over intervals + range query" → **segment tree with lazy** (Advanced).
- "Count elements < x in a range" → **merge-sort tree / persistent segment tree** (Advanced).
- "Only prefix sums + point add" → **Fenwick tree** (simpler).
- "Static array, idempotent op (min/gcd)" → **sparse table**.

---

## Example Problems

### Easy — Range Sum Query, Mutable
`update(i, val)` and `sumRange(l, r)` — exactly the class above. O(log n) each.

### Medium — Range Minimum Query with updates
Swap the combiner to `std::min` and identity to `LLONG_MAX`; supports point updates unlike a sparse table.

### Hard — Count of Range Sum / inversions via segment tree
Coordinate-compress prefix sums, then use a segment tree (or Fenwick) over compressed values to count qualifying pairs in O(n log n). See [fenwick_tree.md](fenwick_tree.md) and [../Advanced/segment_tree.md](../Advanced/segment_tree.md).

---

*Next →* [fenwick_tree.md](fenwick_tree.md)
