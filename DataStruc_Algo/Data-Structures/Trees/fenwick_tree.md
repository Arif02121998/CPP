# File: Trees/fenwick_tree.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · [traversals](traversals.md) · [binary_search_tree](binary_search_tree.md) · [balanced_trees](balanced_trees.md) · [heap](heap.md) · [segment_tree](segment_tree.md) · fenwick_tree · [advanced_tree_problems](advanced_tree_problems.md)

> Fundamentals here (point update, prefix/range sum). Range-update BIT, 2D BIT, and order-statistics uses are in [../Advanced/fenwick_tree.md](../Advanced/fenwick_tree.md).

---

## Conceptual Understanding

**Definition.** A **Fenwick tree** (Binary Indexed Tree / BIT) is a compact array structure that maintains **prefix aggregates** (usually sums) with **point update** and **prefix query** both in O(log n), using only O(n) space and a very small constant.

**Why it exists.** It solves the same "update + prefix sum" problem as a segment tree but with **less memory, less code, and a smaller constant factor** — at the cost of flexibility (it shines for invertible operations like sum/xor).

**Problem it solves.** Dynamic prefix sums: `add(i, delta)` and `prefixSum(i)` (hence any `rangeSum(l, r)`), while elements change.

**Real-world analogy.** A running-total ledger organized so that any cumulative total is reconstructable by adding a few "checkpoint" partial sums instead of scanning every entry.

---

## Internal Working

The magic is the **lowest set bit**, `i & (-i)`. Each index `i` (1-based) is responsible for a range of `i & (-i)` elements ending at `i`.

```
Index responsibilities (range each BIT slot covers):
 i:      1   2      3   4          5   6      7   8
 covers [1] [1..2] [3] [1..4]     [5] [5..6] [7] [1..8]
 i&(-i): 1   2      1   4          1   2      1   8
```

- **update(i, δ):** add δ, then jump `i += i & (-i)` to update all responsible ancestors.
- **query(i):** sum, then jump `i -= i & (-i)` to accumulate the disjoint blocks covering the prefix `[1..i]`.

Each jump clears/adds one bit → at most O(log n) steps.

💡 **1-based indexing is essential** — `i & (-i)` breaks at index 0.

---

## Operations

| Operation | Time |
|---|---|
| point update `add(i, δ)` | O(log n) |
| prefix query `sum(1..i)` | O(log n) |
| range query `sum(l..r)` | O(log n) (= sum(r) − sum(l−1)) |
| build | O(n log n) naive, O(n) optimized |
| Space | O(n) |

---

## C++ Implementation — Prefix Sums

```cpp
#include <vector>

class Fenwick {
    int n;
    std::vector<long long> bit;      // 1-indexed
public:
    Fenwick(int n_) : n(n_), bit(n_ + 1, 0) {}

    void add(int i, long long delta) {          // 1-based index
        for (; i <= n; i += i & (-i))
            bit[i] += delta;
    }
    long long sum(int i) const {                // prefix sum [1..i]
        long long s = 0;
        for (; i > 0; i -= i & (-i))
            s += bit[i];
        return s;
    }
    long long rangeSum(int l, int r) const {    // inclusive [l..r], 1-based
        return sum(r) - sum(l - 1);
    }
};
```

**Build in O(n):**
```cpp
Fenwick fromArray(const std::vector<long long>& a) {  // a is 1-indexed logically
    int n = a.size() - 1;
    Fenwick f(n);
    for (int i = 1; i <= n; ++i) {
        f.addRaw(i, a[i]);                 // add without propagation, then push once
    }
    // Optimized: bit[i] += a[i]; then bit[i + (i&-i)] += bit[i];
    return f;
}
```
(The standard O(n) trick: fill `bit[i] += a[i]`, then for each `i`, push `bit[i]` into `bit[i + (i & -i)]`.)

**STL note.** No standard BIT. For sum/prefix with updates it's the go-to; for min/max or range-assign use a [segment tree](segment_tree.md) (min/max isn't invertible, so a plain Fenwick can't do range-min with updates).

---

## Fenwick vs Segment Tree

| Aspect | Fenwick (BIT) | Segment Tree |
|---|---|---|
| Memory | O(n), ~1× | O(n), ~4× |
| Constant factor | very small | larger |
| Code size | tiny | moderate |
| Operations | invertible (sum, xor, count) | any associative (min/max/gcd/assign) |
| Range update | with tricks (Advanced) | clean via lazy |
| Ease | easiest | more general |

💡 **Choose Fenwick** for prefix-sum-style problems; **choose a segment tree** when you need min/max/gcd, range assignment, or complex merges.

---

## Tradeoffs
- ✅ Smallest/fastest structure for dynamic prefix sums; trivial to code.
- ✅ Extends to 2D, range-update (difference-BIT), and order statistics.
- ❌ Limited to invertible/groupable operations (no range min/max with point updates).
- ❌ 1-based indexing and bit tricks are error-prone at first.

---

## Use Cases
- Dynamic prefix/range sums with updates.
- **Counting inversions** and "smaller/greater after self."
- Order statistics: find k-th element, rank of an element (BIT on frequencies).
- 2D BIT for dynamic submatrix sums.
- Frequency tables, sweep-line counting.

---

## Pitfalls
- ⚠️ Using **0-based** indexing — `i & (-i)` requires 1-based (shift indices by +1).
- ⚠️ Integer overflow on sums — use `long long`.
- ⚠️ Trying range-min/max with a plain BIT (not invertible → impossible without extra structure).
- ⚠️ Off-by-one in `rangeSum(l, r) = sum(r) − sum(l−1)`.
- ⚠️ Not coordinate-compressing large/ sparse values before using them as indices.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Prefix/range **sum** with point updates" → **Fenwick tree**.
- "Count inversions / smaller elements after self" → **Fenwick on compressed values**.
- "k-th smallest / rank with dynamic multiset of small values" → **Fenwick as an order-statistics tree**.
- "Dynamic 2D submatrix sum" → **2D Fenwick** (Advanced).

---

## Example Problems

### Easy — Range Sum Query, Mutable
`update(i, val)` = `add(i, val − old[i])`; `sumRange(l, r) = rangeSum(l, r)`. O(log n).

### Medium — Count of Smaller Numbers After Self
Coordinate-compress values; iterate right→left, query the BIT for how many smaller values already seen, then add the current value.
```cpp
#include <vector>
#include <algorithm>
std::vector<int> countSmaller(std::vector<int>& nums) {
    std::vector<int> sorted(nums), res(nums.size());
    std::sort(sorted.begin(), sorted.end());
    Fenwick bit(sorted.size());
    for (int i = nums.size() - 1; i >= 0; --i) {
        int rank = std::lower_bound(sorted.begin(), sorted.end(), nums[i]) - sorted.begin() + 1;
        res[i] = (int)bit.sum(rank - 1);     // count of values strictly smaller seen so far
        bit.add(rank, 1);
    }
    return res;
}
```
**Explanation.** Processing right to left, the BIT holds frequencies of already-seen (to the right) values; the prefix below the current rank counts smaller ones. O(n log n).

### Hard — Count of Range Sum
Compress prefix sums, then for each prefix use a BIT to count earlier prefixes with sum in `[pre − upper, pre − lower]`. O(n log n).

---

*Next →* [advanced_tree_problems.md](advanced_tree_problems.md)
