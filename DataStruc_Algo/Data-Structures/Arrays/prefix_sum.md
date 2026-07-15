# File: Arrays/prefix_sum.md

[← Repository Index](../README.md) · **Arrays:** [basics](basics.md) · [operations](operations.md) · prefix_sum · [sliding_window_patterns](sliding_window_patterns.md) · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** A **prefix sum** array stores cumulative totals: `pre[i] = a[0] + a[1] + … + a[i-1]`. Any range sum is then `sum(l..r) = pre[r+1] − pre[l]` in **O(1)**. The **difference array** is the inverse idea: apply many *range updates* in O(1) each, then reconstruct.

**Why it exists.** Recomputing a range sum naively is O(n) per query. With many queries on a static array, prefix sums turn each query into O(1) after an O(n) preprocessing pass — a classic **preprocess-once, query-many** tradeoff.

**Problem it solves.** Fast range aggregates; counting subarrays with a target sum; batch range increments.

**Real-world analogy.** A bank statement's "running balance" column: to know how much you spent between two dates, subtract the two running balances instead of re-adding every transaction.

---

## Internal Working

```
a   :       3   1   4   1   5      (indices 0..4)
pre : 0     3   4   8   9  14      (pre[0]=0, pre[i+1]=pre[i]+a[i])
        ^empty prefix

sum(a[1..3]) = pre[4] - pre[1] = 9 - 3 = 6   (= 1+4+1) ✓
```

The leading `pre[0] = 0` (empty prefix) removes special-casing for `l = 0`.

**Difference array** (inverse): to add `v` to all of `[l, r]`, do `diff[l] += v; diff[r+1] -= v;`. After all updates, the prefix sum of `diff` reconstructs the final array — N updates in O(N + n) instead of O(N·n).

```
update [1,3] += 5, then [2,4] += 2:
diff: [0, +5, +2, 0, 0, -5, -2]   (index 0..6)
prefix → [0,5,7,7,7,2,0] → final a[i] = 0,5,7,7,7 over indices...
```

---

## Operations

| Operation | Prefix sum | Difference array |
|---|---|---|
| Build | O(n) | — |
| Range query | O(1) | — |
| Range update | — | O(1) |
| Final reconstruct | — | O(n) |
| Point update (then re-query) | O(n) rebuild | — |

💡 Prefix sums are for **static** arrays (no updates between queries). If you need **updates interleaved with queries**, use a [Fenwick tree](../Advanced/fenwick_tree.md) or [segment tree](../Advanced/segment_tree.md) (O(log n) both).

---

## Complexity

| | Time | Space |
|---|---|---|
| Build prefix | O(n) | O(n) |
| Range query | O(1) | — |
| 2D build | O(n·m) | O(n·m) |
| 2D submatrix query | O(1) | — |

---

## C++ Implementation

```cpp
#include <vector>

// 1D prefix sums
struct PrefixSum {
    std::vector<long long> pre;
    explicit PrefixSum(const std::vector<int>& a) : pre(a.size() + 1, 0) {
        for (size_t i = 0; i < a.size(); ++i) pre[i+1] = pre[i] + a[i];
    }
    long long range(int l, int r) const { return pre[r+1] - pre[l]; } // inclusive
};

// Difference array: many range updates, one final read
struct DiffArray {
    std::vector<long long> diff;
    explicit DiffArray(int n) : diff(n + 1, 0) {}
    void update(int l, int r, long long v) { diff[l] += v; diff[r+1] -= v; }
    std::vector<long long> build() {
        std::vector<long long> a(diff.size() - 1);
        long long run = 0;
        for (size_t i = 0; i < a.size(); ++i) { run += diff[i]; a[i] = run; }
        return a;
    }
};

// 2D prefix sum (submatrix sums in O(1))
struct Prefix2D {
    std::vector<std::vector<long long>> p;
    Prefix2D(const std::vector<std::vector<int>>& m)
        : p(m.size()+1, std::vector<long long>(m[0].size()+1, 0)) {
        for (size_t i = 0; i < m.size(); ++i)
            for (size_t j = 0; j < m[0].size(); ++j)
                p[i+1][j+1] = m[i][j] + p[i][j+1] + p[i+1][j] - p[i][j];
    }
    long long region(int r1,int c1,int r2,int c2) const {     // inclusive
        return p[r2+1][c2+1] - p[r1][c2+1] - p[r2+1][c1] + p[r1][c1];
    }
};
```

**STL equivalent.** `std::partial_sum` builds prefix sums; `std::adjacent_difference` is the inverse. Use them for quick one-offs:
```cpp
#include <numeric>
std::vector<long long> pre(a.size());
std::partial_sum(a.begin(), a.end(), pre.begin());
```
**When to use STL vs custom:** `std::partial_sum` for simple cumulative sums; a custom struct when you want the `pre[0]=0` convention or 2D/difference variants.

---

## Tradeoffs

- ✅ O(1) queries after O(n) build; trivial to implement; tiny constant factors.
- ❌ **Static only** — a single point update forces an O(n) rebuild. Use Fenwick/segment tree for dynamic data.
- **Performance vs memory:** costs O(n) extra memory to buy O(1) queries — almost always worth it for many queries.

---

## Use Cases
- Range-sum queries on immutable data; analytics dashboards.
- Counting subarrays with a given sum / divisible by k.
- Image integral images (2D prefix) for fast box filters.
- Batch range increments (difference array): booking systems, range coloring, car-pooling.

---

## Pitfalls
- ⚠️ **Off-by-one:** use size `n+1` and the `pre[0]=0` convention; double-check inclusive vs exclusive `r`.
- ⚠️ **Overflow:** sums grow fast — use `long long`.
- ⚠️ For "count subarrays with sum K," seed the hashmap with `{0:1}` (the empty prefix).
- ⚠️ Don't use prefix sums when updates interleave queries.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Sum of range [l, r]" repeated many times → **prefix sum**.
- "Count subarrays summing to K" / "divisible by K" → **prefix sum + hashmap (of prefix or prefix mod)**.
- "Equal number of 0s and 1s" → map `0 → -1`, then "subarray sum = 0".
- "Apply many range increments, read once" → **difference array**.
- "Submatrix sum" → **2D prefix sum**.

---

## Example Problems

### Easy — Range Sum Query (Immutable)
Answer many `sumRange(l, r)`.
```cpp
class NumArray {
    std::vector<long long> pre;
public:
    NumArray(std::vector<int>& a) : pre(a.size()+1, 0) {
        for (size_t i = 0; i < a.size(); ++i) pre[i+1] = pre[i] + a[i];
    }
    int sumRange(int l, int r) { return (int)(pre[r+1] - pre[l]); }
};
```
**Explanation.** Build once O(n); each query O(1).

### Medium — Subarray Sum Equals K
Count contiguous subarrays summing to `k`.
**Brute force:** all subarrays → O(n²).
**Optimized:** prefix sum + hashmap → O(n).
```cpp
#include <unordered_map>
int subarraySum(const std::vector<int>& a, int k) {
    std::unordered_map<long long,int> cnt{{0,1}}; // empty prefix
    long long sum = 0; int res = 0;
    for (int x : a) {
        sum += x;
        res += cnt[sum - k];     // earlier prefixes that make a window of sum k
        ++cnt[sum];
    }
    return res;
}
```
**Explanation.** A subarray `(j..i]` sums to k iff `pre[i] − pre[j] = k`, i.e. `pre[j] = pre[i] − k`. Count how many such `pre[j]` we've seen.

### Hard — Maximum Sum of Submatrix (fixed via 2D prefix) / Range-Update Range-Sum hint
For repeated submatrix sums, build a 2D prefix and query in O(1) (code above). For interleaved updates+queries, escalate to a [Fenwick tree](../Advanced/fenwick_tree.md) (1D/2D) or [segment tree](../Advanced/segment_tree.md).
**Explanation.** Prefix sums are the static baseline; when updates appear, the same range-query *idea* is preserved but the structure upgrades to O(log n).

---

*Next →* [sliding_window_patterns.md](sliding_window_patterns.md)
