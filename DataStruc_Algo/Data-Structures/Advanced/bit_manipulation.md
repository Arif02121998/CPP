# File: Advanced/bit_manipulation.md

[← Repository Index](../README.md) · **Advanced:** [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [suffix_array](suffix_array.md) · [suffix_tree](suffix_tree.md) · [sparse_table](sparse_table.md) · bit_manipulation · [advanced_data_structures](advanced_data_structures.md)

---

## Conceptual Understanding

**Definition.** **Bit manipulation** works directly on the binary representation of integers using bitwise operators (`&`, `|`, `^`, `~`, `<<`, `>>`). A **bitmask** uses an integer's bits as a compact set of boolean flags.

**Why it exists.** Bit operations are single-cycle CPU instructions — the fastest, most memory-dense way to represent sets, toggle states, and perform arithmetic tricks. Bitmasking also compresses exponential state (subsets) into integers for DP.

**Problem it solves.** Fast set operations, flags/permissions, subset enumeration, parity/counting, and bitmask DP over up to ~20 elements.

**Real-world analogy.** A row of light switches: one integer's bits are the switches; flipping, checking, and combining them are instant.

---

## Core Operators & Identities

| Operation | Expression | Effect |
|---|---|---|
| get bit i | `(x >> i) & 1` | is bit i set? |
| set bit i | `x \| (1 << i)` | turn bit i on |
| clear bit i | `x & ~(1 << i)` | turn bit i off |
| toggle bit i | `x ^ (1 << i)` | flip bit i |
| lowest set bit | `x & (-x)` | isolate rightmost 1 (Fenwick!) |
| clear lowest set bit | `x & (x - 1)` | remove rightmost 1 |
| is power of two | `x && !(x & (x-1))` | exactly one bit set |
| check even | `!(x & 1)` | last bit is 0 |

💡 **Key identities:**
- `x ^ x = 0`, `x ^ 0 = x`, XOR is associative/commutative → used to cancel pairs.
- `x & (x-1)` clears the lowest 1 → **Brian Kernighan's** popcount loops only over set bits.
- `x | (x+1)` sets the lowest 0.

⚠️ Watch **signed** shifts and overflow: use `1u`/`1ULL` when shifting into high bits; `1 << 31` on a 32-bit `int` is UB — use `1u << 31` or `1LL`.

---

## Built-in Bit Functions (GCC/Clang)

```cpp
__builtin_popcount(x);      // number of set bits (unsigned int)
__builtin_popcountll(x);    // ... for long long
__builtin_clz(x);           // count leading zeros  (x != 0)
__builtin_ctz(x);           // count trailing zeros (x != 0)  → index of lowest set bit
__builtin_parity(x);        // parity of set bits

// C++20:
#include <bit>
std::popcount(x); std::countl_zero(x); std::countr_zero(x); std::has_single_bit(x);
```
`31 - __builtin_clz(x)` = `floor(log2(x))`; `__builtin_ctz(x)` = index of the lowest set bit.

---

## Bitmask as a Set

Represent a subset of {0..n−1} as an integer:
```cpp
int s = 0;
s |= (1 << i);                 // add element i
s &= ~(1 << i);                // remove element i
bool in = s & (1 << i);        // membership
int u = a | b;                 // union
int inter = a & b;             // intersection
int diff = a & ~b;             // set difference a \ b
int cnt = __builtin_popcount(s);   // size
```

### Enumerate all subsets — `1 << n` masks
```cpp
for (int mask = 0; mask < (1 << n); ++mask) {
    for (int i = 0; i < n; ++i)
        if (mask & (1 << i)) { /* element i is in this subset */ }
}
```

### Enumerate submasks of a mask (O(3^n) total over all masks)
```cpp
for (int sub = mask; sub; sub = (sub - 1) & mask) {
    // sub ranges over all non-empty submasks of mask
}
```

---

## Bitmask Dynamic Programming

Encode "which elements are used/visited" as a bitmask state. Classic for n ≤ ~20.

### Traveling Salesman (Held-Karp) — O(n²·2ⁿ)
```cpp
#include <vector>
#include <algorithm>
int tsp(const std::vector<std::vector<int>>& dist) {
    int n = dist.size();
    std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, 1e9));
    dp[1][0] = 0;                                    // start at city 0, only it visited
    for (int mask = 1; mask < (1 << n); ++mask)
        for (int u = 0; u < n; ++u) {
            if (dp[mask][u] == 1e9 || !(mask & (1 << u))) continue;
            for (int v = 0; v < n; ++v)
                if (!(mask & (1 << v)))
                    dp[mask | (1 << v)][v] = std::min(dp[mask | (1 << v)][v],
                                                      dp[mask][u] + dist[u][v]);
        }
    int best = 1e9;
    for (int u = 1; u < n; ++u) best = std::min(best, dp[(1<<n)-1][u] + dist[u][0]);
    return best;
}
```
**Explanation.** `dp[mask][u]` = shortest path visiting exactly `mask` ending at `u`. Transitions add one new city. Exponential but feasible for small n.
**Other bitmask DP:** assignment problems, "minimum number of teams," subset-sum partitioning, graph coloring.

---

## Complexity

| Task | Time |
|---|---|
| single bit op | O(1) |
| popcount / clz / ctz | O(1) (hardware) |
| enumerate subsets | O(n·2ⁿ) |
| enumerate all submasks | O(3ⁿ) |
| bitmask DP (e.g., TSP) | O(n²·2ⁿ) |

---

## Tradeoffs
- ✅ Fastest and most memory-dense set/flag representation; enables otherwise-infeasible subset DP.
- ❌ Limited to ~64 elements per machine word (use `std::bitset` for more).
- ❌ Easy to write cryptic, bug-prone code; signed-shift/overflow traps.

**`std::bitset<N>`** gives fixed-size bitsets > 64 bits with fast bulk ops (`count()`, `&`, `|`, `<<`), useful for large boolean arrays and bitset-accelerated DP.

---

## Use Cases
- Flags/permissions, feature toggles, hardware registers.
- Compact sets, subset enumeration, bitmask DP (TSP, assignment, coloring).
- Fast math: multiply/divide by powers of two, parity, power-of-two checks.
- Hashing/state encoding; bitset-accelerated DP and sieves.
- XOR tricks: find the unique/missing number, swap without temp.

---

## Pitfalls
- ⚠️ Shifting into the sign bit / overflow: `1 << 31` (int) is UB — use `1u << 31` or `1LL << k`.
- ⚠️ `__builtin_clz(0)` / `ctz(0)` are **undefined** — guard against zero.
- ⚠️ Operator precedence: `x & 1 == 0` parses as `x & (1 == 0)`; parenthesize: `(x & 1) == 0`.
- ⚠️ Confusing logical (`&&`, `||`) with bitwise (`&`, `|`).
- ⚠️ Mixing signed/unsigned in shifts; right-shift of negatives is implementation-defined.
- ⚠️ Bitmask DP blows up beyond ~20–22 elements (2ⁿ memory/time).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Every element appears twice except one" → **XOR all** (pairs cancel).
- "Count/iterate subsets, n ≤ ~20" → **bitmask enumeration / DP**.
- "Is it a power of two / count set bits" → **`x & (x-1)`, popcount**.
- "Toggle/track a small set of states" → **bitmask flags**.
- "Assignment / visit-all with small n (TSP)" → **Held-Karp bitmask DP**.
- "Max XOR pair" → **binary trie** ([../Tries/applications.md](../Tries/applications.md)).

---

## Example Problems

### Easy — Single Number
Every element appears twice except one; XOR everything — pairs cancel to 0.
```cpp
int singleNumber(std::vector<int>& nums) {
    int x = 0; for (int n : nums) x ^= n; return x;   // O(n), O(1)
}
```
**Explanation.** `a ^ a = 0` and XOR is commutative, so all duplicates vanish, leaving the unique value.

### Medium — Counting Bits (0..n)
`bits[i] = bits[i >> 1] + (i & 1)` — DP on the removed lowest bit. O(n).
```cpp
std::vector<int> countBits(int n) {
    std::vector<int> bits(n + 1, 0);
    for (int i = 1; i <= n; ++i) bits[i] = bits[i >> 1] + (i & 1);
    return bits;
}
```
**Explanation.** `i` has the same set bits as `i/2` plus its own last bit.

### Hard — Partition to K Equal Sum Subsets / Min Cost Assignment
Bitmask DP over used-elements states. E.g., `dp[mask]` = feasibility/cost given the chosen subset. O(n·2ⁿ) or O(n²·2ⁿ).
**Explanation.** The bitmask compresses "which items are already placed," turning exponential search into memoized DP feasible for small n.

---

*Next →* [advanced_data_structures.md](advanced_data_structures.md)
