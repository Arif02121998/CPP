# File: Disjoint-Set/union_find.md

[← Repository Index](../README.md) · **Disjoint-Set:** union_find · [optimizations](optimizations.md)

---

## Conceptual Understanding

**Definition.** A **disjoint-set** (a.k.a. **union-find**) maintains a collection of **non-overlapping sets** and supports two operations near-instantly: **find** (which set does x belong to?) and **union** (merge two sets). Each set is identified by a **representative** (root).

**Why it exists.** Many problems repeatedly ask "are these two elements connected?" and "connect these two." A naive graph traversal is O(V+E) per query; union-find answers both in **near-constant amortized time**.

**Problem it solves.** Dynamic connectivity: grouping, merging, and connectivity queries as edges/relations arrive — without recomputing from scratch.

**Real-world analogy.** Merging friend circles: to check if two people are in the same circle you find each circle's "leader"; to merge circles you point one leader at the other.

---

## Internal Working

Represent each set as a tree; every node points to its **parent**, and the root points to itself. Two elements are in the same set iff they share a root.

```
parent: 0→0  1→0  2→1  3→3  4→3

sets:   {0,1,2}   {3,4}
tree:     0          3
         / \         |
        1   ...      4
        |
        2
find(2) climbs 2→1→0 (root)
```

- **find(x):** follow parent pointers to the root.
- **union(a, b):** attach one root under the other.

Without optimization these trees can become tall (O(n) find). Two optimizations make it nearly O(1) — detailed in [optimizations.md](optimizations.md):
- **Union by rank/size:** always attach the smaller tree under the larger.
- **Path compression:** during `find`, repoint visited nodes directly to the root.

Together they give amortized **O(α(n))**, where α is the inverse Ackermann function (≤ 4 for all practical n).

---

## Operations

| Operation | Amortized time (optimized) |
|---|---|
| `find(x)` | O(α(n)) ≈ O(1) |
| `union(a,b)` | O(α(n)) ≈ O(1) |
| `connected(a,b)` | O(α(n)) |
| build (n singletons) | O(n) |

---

## C++ Implementation (with both optimizations)

```cpp
#include <vector>
#include <numeric>

class DSU {
    std::vector<int> parent, rank_;
    int components;
public:
    DSU(int n) : parent(n), rank_(n, 0), components(n) {
        std::iota(parent.begin(), parent.end(), 0);   // each node is its own root
    }
    int find(int x) {                                  // path compression
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];             // path halving
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {                          // union by rank
        int ra = find(a), rb = find(b);
        if (ra == rb) return false;                     // already connected
        if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
        parent[rb] = ra;
        if (rank_[ra] == rank_[rb]) ++rank_[ra];
        --components;
        return true;
    }
    bool connected(int a, int b) { return find(a) == find(b); }
    int count() const { return components; }            // number of disjoint sets
};
```

**STL note.** No standard union-find. Boost has `disjoint_sets`, but the ~20-line class above is the norm in interviews and competitive programming.

**When to use union-find vs DFS/BFS.** Union-find shines for **incremental/online** connectivity (edges arrive over time) and for **Kruskal's MST**. Use DFS/BFS when you need actual paths, distances, or a one-shot component labeling.

---

## Tradeoffs
- ✅ Near-constant union/find; tiny code; ideal for dynamic connectivity and MST.
- ✅ Tracks component count and sizes cheaply.
- ❌ Supports only **union** (merging), not efficient **splitting** (deletion of edges) — that needs other structures (e.g., link-cut trees or offline processing).
- ❌ Doesn't give paths/distances — only connectivity.
- **vs DFS/BFS:** union-find is better for many incremental queries; traversal is better for path/structure info.

---

## Use Cases
- **Kruskal's MST** (cycle detection while adding edges) — [../Graphs/mst.md](../Graphs/mst.md).
- Connected components / number of islands (union adjacent land).
- Dynamic connectivity ("are these two connected now?").
- Cycle detection in an **undirected** graph (union endpoints; if already same set → cycle).
- Account/email merging, friend circles, equivalence classes.
- Percolation, image segmentation, network connectivity.

---

## Pitfalls
- ⚠️ Forgetting **path compression** or **union by rank/size** → O(n) trees, TLE.
- ⚠️ Union-ing raw elements instead of their **roots** (`unite` must call `find` first).
- ⚠️ Using `parent[a] = b` directly instead of linking roots.
- ⚠️ Off-by-one / needing coordinate compression or a map when elements aren't 0..n−1.
- ⚠️ Expecting to **undo** unions or split sets (not supported by the basic structure).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Are these two connected / in the same group?" → **union-find**.
- "Merge groups / count connected components dynamically" → **union-find**.
- "Detect a cycle in an undirected graph while adding edges" → **union-find**.
- "Minimum cost to connect everything" → **Kruskal (union-find)**.
- "Group accounts/emails/strings by shared attribute" → **union-find over ids**.

---

## Example Problems

### Easy — Number of Connected Components in an Undirected Graph
Union each edge's endpoints; the answer is `dsu.count()`. O(E·α).
```cpp
DSU dsu(n);
for (auto& e : edges) dsu.unite(e[0], e[1]);
int comps = dsu.count();
```
**Explanation.** Each successful union reduces the component count by one.

### Medium — Redundant Connection
Return the first edge that connects two already-connected vertices (i.e., creates a cycle).
```cpp
std::vector<int> findRedundant(std::vector<std::vector<int>>& edges) {
    DSU dsu(edges.size() + 1);
    for (auto& e : edges)
        if (!dsu.unite(e[0], e[1])) return e;   // endpoints already connected → cycle edge
    return {};
}
```
**Explanation.** In a tree-plus-one-edge, the extra edge is the one whose endpoints are already in the same set. O(n·α).

### Hard — Accounts Merge
Union emails that share an account owner (map each email to an id), then group emails by root and sort. O(N·α + sorting).
**Explanation.** Emails connected through common accounts form one set; union-find merges them, then each root's set is one merged account.

---

*Next →* [optimizations.md](optimizations.md)
