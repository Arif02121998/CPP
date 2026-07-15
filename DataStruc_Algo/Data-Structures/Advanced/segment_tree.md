# File: Advanced/segment_tree.md

[← Repository Index](../README.md) · **Advanced:** segment_tree · [fenwick_tree](fenwick_tree.md) · [suffix_array](suffix_array.md) · [suffix_tree](suffix_tree.md) · [sparse_table](sparse_table.md) · [bit_manipulation](bit_manipulation.md) · [advanced_data_structures](advanced_data_structures.md)

> Fundamentals (point update, range query) are in [../Trees/segment_tree.md](../Trees/segment_tree.md). This file covers the **advanced machinery**: lazy propagation, range updates, and 2D / persistent / merge-sort variants.

---

## Conceptual Understanding

**Definition.** Beyond point updates, real problems need **range updates** ("add 5 to all elements in [l, r]") combined with **range queries**. **Lazy propagation** makes both O(log n) by deferring updates: a node records a pending update and pushes it to children only when needed.

**Why it exists.** Applying a range update naively touches O(n) leaves. Lazy propagation stores the update at the highest fully-covered nodes and propagates lazily, keeping updates O(log n).

**Problem it solves.** Range-update + range-query, plus multi-dimensional and historical (persistent) range queries.

**Real-world analogy.** A manager told "give everyone in your department a raise" writes one note (lazy tag) instead of visiting each employee; the raise is applied to an individual only when someone asks about that person's salary.

---

## Lazy Propagation — How It Works

Each node stores its aggregate **and** a `lazy` value = a pending update not yet applied to its children.

- **Apply an update** to a node: update its aggregate to reflect the whole range, and add to its `lazy`.
- **push_down** before recursing: apply the parent's `lazy` to both children, then clear it.
- Both range update and range query recurse with the same three cases as the basic tree (disjoint / fully covered / partial), calling `push_down` on partial nodes.

```
Range add +3 to [0..3] on the whole segment:
  → set root's sum += 3 * rangeLen; root.lazy += 3
  → children untouched until a query/update descends into them (then push_down)
```

💡 The invariant: a node's aggregate is **always correct**; its `lazy` is what its **children** still owe.

---

## C++ Implementation — Range Add, Range Sum

```cpp
#include <vector>

class LazySegTree {
    int n;
    std::vector<long long> tree, lazy;

    void push_down(int node, int lo, int hi) {
        if (lazy[node] == 0) return;
        int mid = (lo + hi) / 2;
        applyTag(2*node,   lo,    mid, lazy[node]);
        applyTag(2*node+1, mid+1, hi,  lazy[node]);
        lazy[node] = 0;
    }
    void applyTag(int node, int lo, int hi, long long add) {
        tree[node] += add * (hi - lo + 1);       // whole range gets +add
        lazy[node] += add;                        // children still owe it
    }
    void build(const std::vector<int>& a, int node, int lo, int hi) {
        if (lo == hi) { tree[node] = a[lo]; return; }
        int mid = (lo + hi) / 2;
        build(a, 2*node, lo, mid);
        build(a, 2*node+1, mid+1, hi);
        tree[node] = tree[2*node] + tree[2*node+1];
    }
    void update(int node, int lo, int hi, int l, int r, long long add) {
        if (r < lo || hi < l) return;                         // disjoint
        if (l <= lo && hi <= r) { applyTag(node, lo, hi, add); return; }  // covered
        push_down(node, lo, hi);                              // partial → propagate first
        int mid = (lo + hi) / 2;
        update(2*node, lo, mid, l, r, add);
        update(2*node+1, mid+1, hi, l, r, add);
        tree[node] = tree[2*node] + tree[2*node+1];
    }
    long long query(int node, int lo, int hi, int l, int r) {
        if (r < lo || hi < l) return 0;
        if (l <= lo && hi <= r) return tree[node];
        push_down(node, lo, hi);
        int mid = (lo + hi) / 2;
        return query(2*node, lo, mid, l, r) + query(2*node+1, mid+1, hi, l, r);
    }
public:
    LazySegTree(const std::vector<int>& a) : n(a.size()), tree(4*a.size()), lazy(4*a.size(), 0) {
        build(a, 1, 0, n-1);
    }
    void update(int l, int r, long long add) { update(1, 0, n-1, l, r, add); }
    long long query(int l, int r) { return query(1, 0, n-1, l, r); }
};
```

### Range Assign (set) vs Range Add
For **assignment** (`set all in [l,r] = v`), the lazy tag is "assigned value + hasTag flag"; applying overwrites rather than adds, and `push_down` copies the assignment. Combining add + assign needs careful tag composition (assignment clears pending adds).

---

## Advanced Variants

### 2D Segment Tree
A segment tree of segment trees for **submatrix** queries/updates. O(log²n) per op, O(n²) space (heavy — prefer a [2D Fenwick](fenwick_tree.md) for sum-only).

### Persistent Segment Tree
Each update creates O(log n) new nodes and returns a **new root**, keeping all previous versions immutable. Enables queries on any historical version and problems like **k-th smallest in a range** (via version differences). O(log n) time & extra space per update.

### Merge-Sort Tree
Each node stores a **sorted list** of its range's elements. Answers "count elements < x in [l,r]" via binary search in each covered node. O(log²n) query, O(n log n) space (static).

### Segment Tree Beats
Handles complex range ops like "range chmin/chmax" with amortized O(log n) / O(log²n).

---

## Complexity

| Operation | Time | Space |
|---|---|---|
| build | O(n) | O(n) |
| range update (lazy) | O(log n) | — |
| range query | O(log n) | — |
| 2D range op | O(log²n) | O(n²) |
| persistent update | O(log n) | O(log n) new nodes |
| merge-sort tree query | O(log²n) | O(n log n) |

---

## Tradeoffs
- ✅ Range update **and** range query in O(log n); extremely flexible aggregates.
- ✅ Persistent/2D/merge-sort variants solve otherwise-hard offline problems.
- ❌ Larger constant, ~4n (or more) memory, intricate code — lazy tag composition is bug-prone.
- **vs Fenwick:** BIT is smaller/faster for sum but can't do range-min/assign; segment tree is the general tool.

---

## Use Cases
- Range add/assign + range sum/min/max (interval scheduling, range coloring).
- k-th smallest in a subarray (persistent segment tree).
- Count/aggregate with a value threshold over a range (merge-sort tree).
- 2D range sums with updates; computational geometry sweeps.

---

## Pitfalls
- ⚠️ Forgetting `push_down` before descending on partial ranges → stale children.
- ⚠️ Wrong lazy composition (add vs assign order; assignment must clear pending adds).
- ⚠️ Multiplying by the wrong range length in `applyTag`.
- ⚠️ Under-sizing `tree`/`lazy` (use 4n).
- ⚠️ Integer overflow — use `long long`.
- ⚠️ Persistent tree: accidentally mutating a shared (old-version) node.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Add/assign over a range **and** query a range" → **lazy segment tree**.
- "k-th smallest in a range / query a past version" → **persistent segment tree**.
- "Count values < x in a range" → **merge-sort tree / persistent segment tree**.
- "Submatrix update + query" → **2D segment tree / 2D BIT**.
- "range chmin/chmax with sum" → **segment tree beats**.

---

## Example Problems

### Medium — Range Sum with Range Updates
`update(l, r, +v)` and `query(l, r)` — exactly `LazySegTree` above. O(log n) each.

### Medium — Range Assignment + Range Max
Change the aggregate to `max`, identity `−∞`, and the lazy tag to an assignment flag+value.

### Hard — Count of Smaller Numbers After Self / K-th smallest in range
Use a **persistent segment tree** over compressed values: build a version per prefix; the range k-th query subtracts two versions. O(n log n) build, O(log n) per query.
**Explanation.** Version differences give the frequency of each value within a range, enabling a descent for the k-th smallest.

---

*Next →* [fenwick_tree.md](fenwick_tree.md)
