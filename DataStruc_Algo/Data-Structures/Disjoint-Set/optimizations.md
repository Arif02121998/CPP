# File: Disjoint-Set/optimizations.md

[← Repository Index](../README.md) · **Disjoint-Set:** [union_find](union_find.md) · optimizations

---

## Conceptual Understanding

**Definition.** This file explains **why** union-find is nearly O(1) — the two optimizations (**path compression** and **union by rank/size**) and their combined amortized complexity — plus advanced variants: **weighted DSU** (relations between elements), **DSU with rollback**, and **offline dynamic connectivity**.

**Why it matters.** A naive union-find can be O(n) per operation (degenerate into a linked list). The optimizations turn it into the fastest known solution for dynamic connectivity — but only if applied correctly and understood.

**Real-world analogy.** Instead of every villager remembering a long chain of "who told whom," each learns to point **directly at the chief** (path compression), and small villages always join large ones to keep hierarchies flat (union by size).

---

## Optimization 1 — Union by Rank / Size

Always attach the **shorter/smaller** tree under the **taller/larger** one so trees stay shallow.
- **Union by rank:** rank ≈ tree height upper bound; attach lower rank under higher; equal ranks → increment the new root's rank.
- **Union by size:** attach the smaller set under the larger; track `size[root]`.

Without this, repeatedly union-ing can create a chain of height n.
```
Bad (no union by size):   Good (union by size):
1→2→3→4→5 (height 5)       balanced, height ~log n
```
Alone, union by rank/size guarantees **O(log n)** per operation.

---

## Optimization 2 — Path Compression

During `find`, make every node on the path point **directly to the root**, flattening the tree for future queries.
```
find(4):  4→3→2→1(root)   ⇒   after: 4→1, 3→1, 2→1
```
Variants:
- **Full compression (two-pass):** find the root, then repoint every node to it.
- **Path halving:** each node points to its grandparent (`parent[x]=parent[parent[x]]`) — one pass, nearly as good, simplest to code.
- **Path splitting:** each node points to its grandparent as you walk.

```cpp
int find(int x) {                       // path halving (one pass)
    while (parent[x] != x) {
        parent[x] = parent[parent[x]];  // skip a level
        x = parent[x];
    }
    return x;
}
// Full compression (recursive):
int findFull(int x) { return parent[x]==x ? x : parent[x]=findFull(parent[x]); }
```

---

## Combined Complexity — Inverse Ackermann

With **both** optimizations, any sequence of m operations on n elements runs in **O(m · α(n))**, where α is the inverse Ackermann function.
$$\alpha(n) \le 4 \quad \text{for all } n \le 2^{2^{2^{2^{16}}}}$$
So α(n) is effectively a small constant — union-find is amortized **O(1)** in practice.

| Optimization | Per-operation |
|---|---|
| Neither | O(n) worst |
| Path compression only | O(log n) amortized |
| Union by rank/size only | O(log n) |
| **Both** | **O(α(n)) ≈ O(1)** |

💡 Use **both** — they're a few lines each and jointly give the optimal bound.

---

## Advanced Variant 1 — Weighted / Relational DSU

Store, alongside each node, its **relation to its parent** (e.g., a value or parity). During `find`, accumulate the relation while compressing. Enables "is A a certain distance/parity from B?" queries.
```cpp
// rank_/parity example: parity[x] = relation of x to parent[x] (0 or 1)
int find(int x) {
    if (parent[x] == x) return x;
    int root = find(parent[x]);
    parity[x] ^= parity[parent[x]];       // accumulate relation to root
    parent[x] = root;
    return root;
}
```
**Use:** "Satisfiability of Equality Equations," parity/bipartite constraints, currency-ratio queries (weighted union), "Evaluate Division" with multiplicative weights.

---

## Advanced Variant 2 — DSU with Rollback

Path compression breaks reversibility, so **rollback DSU uses only union by rank** (no compression) and records changes on a stack to **undo** unions. Each op becomes O(log n) but supports undo — essential for:
- **Offline dynamic connectivity** (segment tree on time + DSU rollback).
- Divide-and-conquer over queries where edges are added and later removed.

---

## Advanced Variant 3 — Persistent / Small-to-Large

- **Small-to-large merging:** when merging auxiliary data (sets/maps) attached to components, always merge the smaller into the larger → total O(n log n).
- **Persistent DSU:** keep historical versions for time-travel queries (heavier; rarely needed).

---

## Tradeoffs
- ✅ Both optimizations → optimal α(n); trivial to add.
- ❌ Path compression prevents rollback → choose one (compression **or** rollback) per problem.
- ❌ Weighted DSU adds bookkeeping and careful relation math.
- **Rollback vs compression:** undo capability costs the near-constant bound (O(log n) instead).

---

## Use Cases
- Everything in [union_find.md](union_find.md), plus:
- Relational constraints (parity, ratios, equalities).
- Offline dynamic connectivity (edges added/removed over time).
- Aggregating per-component statistics with small-to-large merging.

---

## Pitfalls
- ⚠️ Implementing only one optimization when the constraints demand the α(n) bound.
- ⚠️ Combining path compression with rollback (incompatible — compression's side effects can't be cleanly undone).
- ⚠️ Weighted DSU: not updating the stored relation during compression → wrong distances/parities.
- ⚠️ Union by rank: forgetting to increment rank on equal-rank merges.
- ⚠️ Recursion depth in recursive `find` on huge inputs (prefer iterative path halving).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Connectivity queries, must be fast" → **path compression + union by rank**.
- "Relations/parity/ratios between elements" → **weighted DSU**.
- "Edges appear **and disappear** / offline connectivity over time" → **DSU with rollback**.
- "Merge per-component data efficiently" → **small-to-large**.
- "Equations like a==b, a!=b consistent?" → **weighted/relational DSU**.

---

## Example Problems

### Medium — Satisfiability of Equality Equations
Union all `a==b` pairs, then verify no `a!=b` pair shares a root.
```cpp
// process "==" first with unite(); then for each "!=", if connected(a,b) → false.
```
**Explanation.** Equalities partition variables into groups; an inequality within a group is a contradiction. O(n·α).

### Medium — Evaluate Division (weighted DSU)
Union variables with the division ratio as edge weight; a query's answer is the product of weights along the path to the common root (or −1 if disconnected).
**Explanation.** Weighted union-find turns ratio chains into O(α) root-relative values.

### Hard — Number of Islands II (online)
Add land cells one at a time; after each addition, union with existing land neighbors and report the current island count.
```cpp
// For each added cell: make it its own set (++count), then unite with up to 4 land neighbors,
// decrementing count on each successful union.
```
**Explanation.** Union-find handles the incremental additions in near-constant time per operation — DFS/BFS would recompute each time. O(k·α).

---

*Next →* [../Advanced/segment_tree.md](../Advanced/segment_tree.md)
