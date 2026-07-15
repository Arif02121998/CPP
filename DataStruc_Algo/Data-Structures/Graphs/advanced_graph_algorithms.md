# File: Graphs/advanced_graph_algorithms.md

[← Repository Index](../README.md) · **Graphs:** [graph_basics](graph_basics.md) · [bfs](bfs.md) · [dfs](dfs.md) · [shortest_path](shortest_path.md) · [mst](mst.md) · [topological_sort](topological_sort.md) · advanced_graph_algorithms

---

## Conceptual Understanding

**Definition.** This file covers the **deeper graph toolkit**: strongly connected components (Tarjan/Kosaraju), bridges & articulation points, network flow (max-flow/min-cut), bipartite matching, Euler paths, and A* search.

**Why it matters.** Many hard problems reduce to one of these classic algorithms. Recognizing "this is a min-cut" or "this is bipartite matching" is the crux — the implementation is then standard.

**Real-world analogy.** Beyond basic navigation, these answer structural questions: which links are critical (bridges), how much can flow through a network (max-flow), who can be paired with whom (matching).

---

## 1. Strongly Connected Components (SCC)

**Definition.** In a directed graph, an SCC is a maximal set of vertices mutually reachable from each other. Condensing each SCC to a node yields a DAG.

**Tarjan's algorithm (single DFS).** Track `disc[u]` (discovery time) and `low[u]` (lowest reachable disc via the DFS subtree/back edges). A vertex with `low[u] == disc[u]` is the root of an SCC; pop the stack down to it.
```cpp
#include <vector>
#include <stack>
#include <algorithm>
struct Tarjan {
    const std::vector<std::vector<int>>& adj;
    std::vector<int> disc, low, comp; std::vector<bool> onStk;
    std::stack<int> stk; int timer=0, sccCount=0;
    Tarjan(const std::vector<std::vector<int>>& g)
      : adj(g), disc(g.size(),-1), low(g.size(),-1), comp(g.size(),-1), onStk(g.size(),false) {}
    void dfs(int u){
        disc[u]=low[u]=timer++; stk.push(u); onStk[u]=true;
        for(int v:adj[u]){
            if(disc[v]==-1){ dfs(v); low[u]=std::min(low[u],low[v]); }
            else if(onStk[v]) low[u]=std::min(low[u],disc[v]);   // back edge
        }
        if(low[u]==disc[u]){                                     // SCC root
            while(true){ int v=stk.top(); stk.pop(); onStk[v]=false; comp[v]=sccCount; if(v==u) break; }
            ++sccCount;
        }
    }
    int run(){ for(int i=0;i<(int)adj.size();++i) if(disc[i]==-1) dfs(i); return sccCount; }
};
```
**Kosaraju's** is a simpler 2-pass alternative: DFS to get finish order, transpose the graph, DFS in reverse finish order. Both O(V+E).

---

## 2. Bridges & Articulation Points

A **bridge** is an edge whose removal disconnects the graph; an **articulation point** is such a vertex. Found with the same `disc`/`low` DFS.
- **Bridge:** edge `(u,v)` with `low[v] > disc[u]` (v's subtree can't reach u or above without this edge).
- **Articulation point:** root with ≥2 DFS children, or non-root `u` with a child `v` where `low[v] >= disc[u]`.
```cpp
// Inside DFS over an undirected graph tracking parent:
//   if (low[v] > disc[u]) → (u,v) is a bridge
//   if (low[v] >= disc[u] && u != root) → u is an articulation point
```
**Use:** finding critical connections/single points of failure in networks. O(V+E).

---

## 3. Network Flow — Max-Flow / Min-Cut

**Max-Flow Min-Cut theorem:** the maximum flow from source to sink equals the capacity of the minimum cut separating them.

**Edmonds-Karp** (BFS-based Ford-Fulkerson): repeatedly find an augmenting path via BFS and push flow along it. O(V·E²).
**Dinic's algorithm:** level graph + blocking flows → O(V²·E) (much faster in practice; O(E√V) on unit-capacity/bipartite graphs).

**Applications:** bipartite matching, project selection, image segmentation, edge/vertex-disjoint paths, scheduling.

```
Idea: while an augmenting path s→t with spare capacity exists,
      push the minimum residual capacity along it and update residuals.
```

---

## 4. Bipartite Matching

**Definition.** In a bipartite graph, find the maximum set of edges with no shared endpoints.
- **Hungarian/Kuhn's algorithm** (augmenting paths via DFS): O(V·E).
- Equivalent to max-flow with unit capacities.

**König's theorem:** in bipartite graphs, max matching = min vertex cover.
**Use:** job assignment, task-worker allocation, scheduling.

Check bipartiteness first with **BFS/DFS 2-coloring** (no odd cycles).

---

## 5. Euler Path / Circuit

**Euler path** visits every **edge** exactly once; an **Euler circuit** starts and ends at the same vertex.
- **Undirected:** Euler circuit iff every vertex has even degree (and edges connected); Euler path iff exactly 0 or 2 vertices have odd degree.
- **Directed:** in-degree = out-degree for a circuit (0/±1 imbalance for a path).
- **Hierholzer's algorithm** finds it in O(E).
**Use:** route inspection (Chinese Postman), DNA sequence assembly, "reconstruct itinerary."

---

## 6. A* Search

Best-first search that expands nodes by `f(n) = g(n) + h(n)` (cost so far + admissible heuristic to goal). With an admissible/consistent heuristic it finds the optimal path while exploring far fewer nodes than Dijkstra. **Use:** game pathfinding, robotics, puzzle solving.

---

## Complexity Summary

| Algorithm | Time |
|---|---|
| Tarjan / Kosaraju SCC | O(V+E) |
| Bridges / articulation points | O(V+E) |
| Edmonds-Karp max-flow | O(V·E²) |
| Dinic's max-flow | O(V²·E) |
| Kuhn's bipartite matching | O(V·E) |
| Hierholzer Euler path | O(E) |
| A* | O(E) with a good heuristic (worst case Dijkstra-like) |

---

## Tradeoffs
- ✅ Each solves a whole class of otherwise-hard problems in polynomial time.
- ❌ Implementations are intricate and bug-prone — know the invariants (`low`-link, residual graph, alternating paths).
- **Tarjan vs Kosaraju:** Tarjan is single-pass and faster; Kosaraju is conceptually simpler.
- **Edmonds-Karp vs Dinic:** Dinic is faster but more complex.

---

## Use Cases
- SCC: dependency cycles, 2-SAT, condensing directed graphs.
- Bridges/articulation: network reliability, critical infrastructure.
- Max-flow/min-cut: matching, segmentation, project selection, connectivity.
- Matching: assignment problems, scheduling.
- Euler: routing, sequencing, itinerary reconstruction.
- A*: heuristic pathfinding.

---

## Pitfalls
- ⚠️ SCC/bridge DFS: mixing up `disc` vs `low`; handling parallel edges (bridges) needs edge-id, not just parent.
- ⚠️ Max-flow: forgetting **residual/back edges** — you must be able to cancel flow.
- ⚠️ Matching: not verifying bipartiteness (odd cycle breaks it).
- ⚠️ Euler: ignoring connectivity of the edge-induced subgraph.
- ⚠️ A*: a non-admissible heuristic breaks optimality.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Mutually reachable groups / cycles in a directed graph / 2-SAT" → **SCC**.
- "Critical connections / single point of failure" → **bridges / articulation points**.
- "Maximum matching / assign A to B one-to-one" → **bipartite matching / max-flow**.
- "Max throughput / min edges to disconnect" → **max-flow / min-cut**.
- "Use every edge once / reconstruct itinerary" → **Euler path (Hierholzer)**.
- "Optimal path with a good distance estimate" → **A***.

---

## Example Problems

### Medium — Critical Connections in a Network (bridges)
Find all bridges with the `disc`/`low` DFS above. O(V+E).
```cpp
// Standard bridge-finding: for tree edge (u,v), if low[v] > disc[u], record {u,v}.
```
**Explanation.** A bridge is an edge whose subtree has no alternative route back — captured by `low[v] > disc[u]`.

### Medium — Is Graph Bipartite? (2-coloring)
BFS/DFS assigning alternating colors; conflict → not bipartite. O(V+E).

### Hard — Reconstruct Itinerary (Euler path)
Given airline tickets, build the lexicographically smallest itinerary using all tickets exactly once — Hierholzer's algorithm on a multigraph with a sorted adjacency (min-heap per node). O(E log E).
**Explanation.** An Euler path uses every edge (ticket) once; Hierholzer with lexicographic neighbor order yields the smallest valid route.

---

*Next →* [../Hashing/hash_tables.md](../Hashing/hash_tables.md)
