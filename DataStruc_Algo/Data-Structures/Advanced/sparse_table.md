# File: Advanced/sparse_table.md

[← Repository Index](../README.md) · **Advanced:** [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [suffix_array](suffix_array.md) · [suffix_tree](suffix_tree.md) · sparse_table · [bit_manipulation](bit_manipulation.md) · [advanced_data_structures](advanced_data_structures.md)

---

## Conceptual Understanding

**Definition.** A **sparse table** precomputes answers for all ranges whose length is a **power of two**, enabling **O(1)** range queries for **idempotent** operations (min, max, gcd, AND, OR) on a **static** array — after O(n log n) preprocessing.

**Why it exists.** For static data with many range-min/max queries, a segment tree's O(log n) per query is unnecessary. A sparse table answers each query in **O(1)** by combining just two overlapping power-of-two blocks.

**Problem it solves.** Fast, repeated **Range Minimum/Maximum Query (RMQ)** and gcd/AND/OR queries on an unchanging array — including O(1) LCA via Euler tour.

**Real-world analogy.** Precomputed distance charts between mileposts at powers-of-two spacing; any span is covered by two overlapping precomputed chunks.

---

## Internal Working

`sparse[i][j]` = answer for the range `[i, i + 2^j − 1]` (length `2^j`). Build with DP:
$$\text{sparse}[i][j] = f(\text{sparse}[i][j-1],\ \text{sparse}[i + 2^{j-1}][j-1])$$

```
Two half-blocks combine into the next size:
[i .. i+2^(j-1)-1]  +  [i+2^(j-1) .. i+2^j-1]  →  [i .. i+2^j-1]
```

**Query(l, r):** let `k = floor(log2(r − l + 1))`. The two blocks `[l, l+2^k−1]` and `[r−2^k+1, r]` **overlap** and together cover `[l, r]`. Because min/max/gcd are **idempotent** (overlap doesn't distort the answer), combine them directly:
$$\text{answer} = f(\text{sparse}[l][k],\ \text{sparse}[r - 2^k + 1][k])$$

⚠️ **Idempotency is required** for O(1): `f(x, x) = x` (min, max, gcd, AND, OR). For **sum** (not idempotent), overlap double-counts — sum needs a Fenwick/segment tree or non-overlapping blocks (O(log n)).

---

## C++ Implementation — Range Minimum (O(1) query)

```cpp
#include <vector>

class SparseTable {
    std::vector<std::vector<int>> sp;      // sp[j][i] = min of [i, i+2^j-1]
    std::vector<int> logv;
public:
    SparseTable(const std::vector<int>& a) {
        int n = a.size();
        int K = 1; while ((1 << K) <= n) ++K;      // max power
        sp.assign(K, std::vector<int>(n));
        logv.assign(n + 1, 0);
        for (int i = 2; i <= n; ++i) logv[i] = logv[i/2] + 1;
        sp[0] = a;                                  // length-1 ranges
        for (int j = 1; j < K; ++j)
            for (int i = 0; i + (1 << j) <= n; ++i)
                sp[j][i] = std::min(sp[j-1][i], sp[j-1][i + (1 << (j-1))]);
    }
    int query(int l, int r) const {                 // inclusive, O(1)
        int k = logv[r - l + 1];
        return std::min(sp[k][l], sp[k][r - (1 << k) + 1]);
    }
};
```

**STL note.** No standard sparse table. Precompute the `log2` table once; use `__builtin_clz` for fast logs if preferred.

**When to use a sparse table vs a segment tree.** Sparse table for **static** arrays with **idempotent** queries (fastest: O(1)); segment tree/Fenwick when the array **changes** or the operation isn't idempotent (sum).

---

## Complexity

| | Time | Space |
|---|---|---|
| build | O(n log n) | O(n log n) |
| query (idempotent) | **O(1)** | — |
| update | ❌ (not supported) | — |

---

## Sparse Table vs Segment Tree vs Fenwick

| Aspect | Sparse table | Segment tree | Fenwick |
|---|---|---|---|
| Query | O(1) (idempotent) | O(log n) | O(log n) |
| Update | none (static) | O(log n) | O(log n) |
| Operations | min/max/gcd/and/or | any associative | invertible (sum/xor) |
| Build/space | O(n log n) | O(n) | O(n) |

💡 Static + min/max/gcd → **sparse table**. Dynamic or sum → **segment tree / Fenwick**.

---

## Tradeoffs
- ✅ Unbeatable O(1) queries for static idempotent RMQ; simple build.
- ❌ No updates (rebuild on change); O(n log n) memory.
- ❌ Only idempotent operations get O(1); sum needs a different structure (or a disjoint-sparse-table for O(1) associative at higher constant).

---

## Use Cases
- Static RMQ (range min/max) — the canonical use.
- Range gcd / AND / OR queries.
- **O(1) LCA** via Euler tour + RMQ on depths.
- Offline query problems where the array is fixed.
- Building block for suffix-array LCP range-min queries ([suffix_array.md](suffix_array.md)).

---

## Pitfalls
- ⚠️ Using it for **sum** (overlap double-counts — not idempotent).
- ⚠️ Trying to update — it's static; any change requires a full rebuild.
- ⚠️ Wrong `k = log2(len)` (precompute the log table or use `__builtin_clz`).
- ⚠️ Off-by-one in the second block start `r − 2^k + 1`.
- ⚠️ O(n log n) memory blowup for very large n — consider a segment tree instead.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Many range **min/max/gcd** queries, array never changes" → **sparse table**.
- "O(1) range query required" and op is idempotent → **sparse table**.
- "LCA with O(1) queries" → **Euler tour + sparse-table RMQ**.
- "Array updates between queries" → **segment tree** instead.

---

## Example Problems

### Easy — Static Range Minimum Query
Preprocess once, answer each `query(l, r)` in O(1). Ideal when there are many queries and no updates.

### Medium — Range GCD Queries
Replace `min` with `std::__gcd`; gcd is idempotent, so O(1) queries hold. O(n log n) build.
**Explanation.** gcd(x, x) = x, so overlapping blocks don't change the result.

### Hard — LCA via Euler Tour + RMQ
Flatten the tree with an Euler tour recording depths; the LCA of u and v is the shallowest node between their first occurrences → a range-min query answered in O(1) by a sparse table over depths. O(n log n) build, O(1) per query.
**Explanation.** Tree LCA reduces to array RMQ on the Euler-tour depth sequence, which the sparse table answers in constant time.

---

*Next →* [bit_manipulation.md](bit_manipulation.md)
