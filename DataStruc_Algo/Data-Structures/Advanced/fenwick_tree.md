# File: Advanced/fenwick_tree.md

[← Repository Index](../README.md) · **Advanced:** [segment_tree](segment_tree.md) · fenwick_tree · [suffix_array](suffix_array.md) · [suffix_tree](suffix_tree.md) · [sparse_table](sparse_table.md) · [bit_manipulation](bit_manipulation.md) · [advanced_data_structures](advanced_data_structures.md)

> Fundamentals (point update, prefix sum) are in [../Trees/fenwick_tree.md](../Trees/fenwick_tree.md). Here: **range updates, 2D BIT, order-statistics, and 2-BIT tricks**.

---

## Conceptual Understanding

**Definition.** This file extends the basic Fenwick tree (BIT) to **range updates**, **2D** queries, and **order-statistics** (finding the k-th element / rank), using clever combinations of one or two BITs.

**Why it matters.** A plain BIT does point-update + prefix-query. With difference-array tricks and paired BITs, the *same* tiny structure handles range-update + range-query and multi-dimensional sums — still O(log n) (or O(log²n) in 2D) and far lighter than a lazy segment tree.

**Real-world analogy.** A ledger of *changes* rather than absolute values: recording "+5 starting here, −5 after there" lets a range raise be logged with two entries and reconstructed on demand.

---

## Technique 1 — Range Update, Point Query (difference BIT)

Maintain a BIT over the **difference array**. To add `v` to `[l, r]`:
```
add(l, +v);  add(r + 1, -v);
```
A point query `value(i) = prefixSum(i)` (the BIT accumulates the differences).
```cpp
// range add [l,r] += v, then point-query index i
void rangeAdd(Fenwick& b, int l, int r, long long v) { b.add(l, v); b.add(r+1, -v); }
long long pointQuery(Fenwick& b, int i) { return b.sum(i); }
```
💡 This is the BIT version of a **difference array** made dynamic.

---

## Technique 2 — Range Update, Range Query (two BITs)

To support **both** range update and range **sum** query, keep two BITs `B1`, `B2`. For `add(l, r, v)`:
```
B1.add(l, v);        B1.add(r+1, -v);
B2.add(l, v*(l-1));  B2.add(r+1, -v*r);
```
Prefix sum `[1..i] = B1.sum(i) * i - B2.sum(i)`; range sum `[l..r] = pre(r) - pre(l-1)`.
```cpp
struct RangeBIT {
    Fenwick b1, b2;
    RangeBIT(int n): b1(n), b2(n) {}
    void rangeAdd(int l, int r, long long v) {
        b1.add(l, v);          b1.add(r+1, -v);
        b2.add(l, v*(l-1));    b2.add(r+1, -v*r);
    }
    long long prefix(int i) { return b1.sum(i) * i - b2.sum(i); }
    long long rangeSum(int l, int r) { return prefix(r) - prefix(l-1); }
};
```
**Explanation.** The two BITs together encode the closed-form of a prefix sum after range additions — O(log n) per op, much lighter than a lazy segment tree for sum.

---

## Technique 3 — 2D Fenwick Tree (submatrix sums)

A BIT of BITs for dynamic 2D prefix sums. `update(x, y, v)` and `query(x, y)` (sum of the rectangle `[1..x][1..y]`) are both O(log R · log C).
```cpp
#include <vector>
struct Fenwick2D {
    int R, C;
    std::vector<std::vector<long long>> bit;
    Fenwick2D(int R_, int C_): R(R_), C(C_), bit(R_+1, std::vector<long long>(C_+1, 0)) {}
    void update(int x, int y, long long v) {
        for (int i = x; i <= R; i += i & (-i))
            for (int j = y; j <= C; j += j & (-j))
                bit[i][j] += v;
    }
    long long query(int x, int y) {                 // sum over [1..x][1..y]
        long long s = 0;
        for (int i = x; i > 0; i -= i & (-i))
            for (int j = y; j > 0; j -= j & (-j))
                s += bit[i][j];
        return s;
    }
    long long rect(int x1, int y1, int x2, int y2) {   // inclusive
        return query(x2,y2) - query(x1-1,y2) - query(x2,y1-1) + query(x1-1,y1-1);
    }
};
```
**Use:** dynamic 2D range sums, counting points in rectangles (with offline/compression).

---

## Technique 4 — Order Statistics on a BIT

Build a BIT over **value frequencies**. Then:
- **rank(x)** = `sum(x)` (how many ≤ x).
- **k-th smallest** = binary-search / **binary-lifting** on the BIT in O(log n): walk bits from high to low, descending where the cumulative count stays < k.
```cpp
int kth(const std::vector<long long>& bit, int n, long long k) {  // n = size (power-of-two cap)
    int pos = 0; long long rem = k;
    for (int pw = 1 << (31 - __builtin_clz(n)); pw; pw >>= 1)
        if (pos + pw <= n && bit[pos + pw] < rem) { pos += pw; rem -= bit[pos]; }
    return pos + 1;                                   // 1-based value/index
}
```
**Use:** dynamic k-th smallest, median maintenance over a bounded value domain, "count smaller after self."

---

## Complexity Summary

| Technique | Update | Query | Space |
|---|---|---|---|
| Range-update point-query (diff BIT) | O(log n) | O(log n) | O(n) |
| Range-update range-query (2 BITs) | O(log n) | O(log n) | O(n) |
| 2D BIT | O(log R·log C) | O(log R·log C) | O(R·C) |
| Order-statistics (BIT binary lifting) | O(log n) | O(log n) | O(maxVal) |

---

## Tradeoffs
- ✅ Tiny, fast, low-memory alternative to lazy/2D segment trees for **sum-type** problems.
- ✅ Order-statistics and 2D with minimal code.
- ❌ Limited to invertible/groupable ops (no range min/max with point updates).
- ❌ Value-indexed BITs need **coordinate compression** for large/sparse domains.
- **vs lazy segment tree:** BIT wins on constant factor and code size for sums; segment tree needed for min/max/assign.

---

## Use Cases
- Dynamic range-add + range-sum (lighter than lazy segment tree).
- Dynamic 2D submatrix sums; rectangle point-counting.
- Counting inversions / smaller-after-self; k-th order statistics.
- Median of a stream over a bounded domain.

---

## Pitfalls
- ⚠️ Two-BIT range formula is easy to misderive — memorize/test `pre(i) = B1.sum(i)*i − B2.sum(i)`.
- ⚠️ 1-based indexing everywhere; shift 0-based inputs.
- ⚠️ Overflow — `long long` (especially `v*(l-1)` and `v*r`).
- ⚠️ Forgetting coordinate compression for value-indexed BITs.
- ⚠️ 2D BIT memory O(R·C) can be large; consider offline/compression.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Range add + range **sum** query, sum-only" → **two-BIT range trick** (lighter than lazy seg tree).
- "Dynamic submatrix sum" → **2D BIT**.
- "k-th smallest / rank with insertions over a bounded domain" → **BIT order statistics**.
- "Count inversions / smaller-after-self / range-sum counting" → **BIT on compressed values**.

---

## Example Problems

### Medium — Range Sum with Range Updates (sum-only)
Use `RangeBIT` above — O(log n) per operation, smaller/faster than a lazy segment tree.

### Medium — Count of Smaller Numbers After Self
BIT on compressed values, iterate right→left querying the prefix below each rank. O(n log n). (Code in [../Trees/fenwick_tree.md](../Trees/fenwick_tree.md).)

### Hard — Count of Range Sum / Reverse Pairs
Compress prefix sums, then use a BIT to count qualifying earlier prefixes for each index. O(n log n).
**Explanation.** Order-statistics on prefix sums via a BIT counts pairs satisfying a sum condition in log-linear time.

---

*Next →* [suffix_array.md](suffix_array.md)
