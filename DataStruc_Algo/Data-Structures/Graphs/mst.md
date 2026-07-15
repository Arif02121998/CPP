# File: Graphs/mst.md

[← Repository Index](../README.md) · **Graphs:** [graph_basics](graph_basics.md) · [bfs](bfs.md) · [dfs](dfs.md) · [shortest_path](shortest_path.md) · mst · [topological_sort](topological_sort.md) · [advanced_graph_algorithms](advanced_graph_algorithms.md)

---

## Conceptual Understanding

**Definition.** A **Minimum Spanning Tree (MST)** of a connected, weighted, undirected graph is a subset of edges that connects **all** vertices with **no cycles** and the **minimum total edge weight**. It has exactly V−1 edges.

**Why it exists.** Many problems ask: "connect everything as cheaply as possible." MST gives the provably optimal answer via greedy choices, thanks to the **cut property**.

**Problem it solves.** Least-cost connectivity: network/utility layout, clustering, approximation for TSP.

**Real-world analogy.** Laying the minimum length of cable to connect every house to the electrical grid without redundant loops.

---

## Foundational Theorem — The Cut Property

For any partition (cut) of the vertices into two sets, the **minimum-weight edge crossing the cut** is in *some* MST. Both Kruskal's and Prim's are greedy algorithms justified by this property.

💡 This is why "always take the cheapest safe edge" produces a globally optimal tree.

---

## Kruskal's Algorithm (edge-centric)

**Idea.** Sort all edges ascending; add each edge if it connects two **different** components (checked with **union-find**), skipping edges that would form a cycle. Stop at V−1 edges.

```cpp
#include <vector>
#include <algorithm>
struct DSU {
    std::vector<int> p, r;
    DSU(int n): p(n), r(n,0) { for (int i=0;i<n;++i) p[i]=i; }
    int find(int x){ return p[x]==x ? x : p[x]=find(p[x]); }  // path compression
    bool unite(int a,int b){
        a=find(a); b=find(b); if(a==b) return false;
        if(r[a]<r[b]) std::swap(a,b);
        p[b]=a; if(r[a]==r[b]) ++r[a];
        return true;
    }
};
struct Edge { int u, v, w; };

long long kruskal(int n, std::vector<Edge>& edges) {
    std::sort(edges.begin(), edges.end(), [](const Edge&a,const Edge&b){ return a.w<b.w; });
    DSU dsu(n);
    long long total = 0; int used = 0;
    for (auto& e : edges)
        if (dsu.unite(e.u, e.v)) {         // connects two components → safe edge
            total += e.w;
            if (++used == n - 1) break;
        }
    return total;                          // (used < n-1 ⇒ graph disconnected)
}
```
**Best for sparse graphs.** O(E log E) dominated by the sort. (Union-find details: [../Disjoint-Set/union_find.md](../Disjoint-Set/union_find.md).)

---

## Prim's Algorithm (vertex-centric)

**Idea.** Grow the tree from a start vertex; repeatedly add the **cheapest edge leaving the current tree** using a min-heap.

```cpp
#include <vector>
#include <queue>
long long prim(int src, const std::vector<std::vector<std::pair<int,int>>>& adj) {
    int n = adj.size();
    std::vector<bool> inMST(n, false);
    using P = std::pair<int,int>;                       // {weight, vertex}
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0, src});
    long long total = 0;
    while (!pq.empty()) {
        auto [w, u] = pq.top(); pq.pop();
        if (inMST[u]) continue;                         // skip stale
        inMST[u] = true; total += w;
        for (auto [v, wt] : adj[u])
            if (!inMST[v]) pq.push({wt, v});
    }
    return total;
}
```
**Best for dense graphs** (an O(V²) array version avoids the heap). Heap version: O(E log V).

---

## Kruskal vs Prim

| Aspect | Kruskal | Prim |
|---|---|---|
| Approach | sort edges, union-find | grow from a vertex, heap |
| Data structure | disjoint-set | priority queue |
| Time | O(E log E) | O(E log V) |
| Best for | **sparse** graphs | **dense** graphs |
| Naturally handles | forest / disconnected detection | one component at a time |

💡 Both give the same total weight (MSTs may differ if weights tie).

---

## Complexity

| Algorithm | Time | Space |
|---|---|---|
| Kruskal | O(E log E) | O(V) (DSU) |
| Prim (heap) | O(E log V) | O(V+E) |
| Prim (matrix) | O(V²) | O(V²) |

---

## Tradeoffs
- ✅ Greedy and optimal (cut property); simple to implement.
- ❌ Only for undirected graphs; "minimum arborescence" (directed MST) needs Chu-Liu/Edmonds.
- **Kruskal vs Prim:** choose by density; Kruskal reuses union-find (also handy for offline connectivity).

---

## Use Cases
- Network design: laying cable/pipe/road at minimum cost.
- Clustering (remove the k−1 most expensive MST edges → k clusters — single-linkage clustering).
- Approximation algorithms (2-approx for metric TSP uses an MST).
- Image segmentation, maze generation (random-weight MST).

---

## Pitfalls
- ⚠️ Applying MST to a **directed** graph (undefined — need Edmonds' algorithm).
- ⚠️ Forgetting the graph may be **disconnected** (no spanning tree; `used < n−1`).
- ⚠️ Kruskal without union-find cycle checks → invalid tree.
- ⚠️ Prim: not skipping stale heap entries (`inMST[u]`).
- ⚠️ Confusing MST with shortest-path tree — they are different (MST minimizes total weight, not per-vertex distance).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Connect all nodes at minimum total cost" → **MST**.
- "Minimum cost to make everything connected / all points" → **MST (often Kruskal)**.
- "Cluster into k groups by proximity" → **MST then cut k−1 edges**.
- "Critical/redundant connections" → MST + bridges ([advanced_graph_algorithms.md](advanced_graph_algorithms.md)).

---

## Example Problems

### Easy — Min Cost to Connect All Points (Manhattan)
Build a complete graph with Manhattan-distance edges; run Prim/Kruskal. O(V² log V) or O(E log E).
```cpp
// edge weight between points i,j = |xi-xj| + |yi-yj|; then Kruskal/Prim.
```
**Explanation.** Directly an MST over points; Prim's O(V²) form is natural for the dense complete graph.

### Medium — Connecting Cities With Minimum Cost
Given weighted connections, return the MST total or −1 if the graph can't be fully connected. Kruskal + union-find. O(E log E).

### Hard — Optimize Water Distribution in a Village
Model "build a well in a house" as an edge from a virtual node 0 to that house with the well's cost; then MST over the augmented graph. O(E log E).
**Explanation.** A virtual super-source converts the "well vs pipe" choice into standard MST edges.

---

*Next →* [topological_sort.md](topological_sort.md)
