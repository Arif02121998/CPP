# File: Graphs/graph_basics.md

[← Repository Index](../README.md) · **Graphs:** graph_basics · [bfs](bfs.md) · [dfs](dfs.md) · [shortest_path](shortest_path.md) · [mst](mst.md) · [topological_sort](topological_sort.md) · [advanced_graph_algorithms](advanced_graph_algorithms.md)

---

## Conceptual Understanding

**Definition.** A **graph** `G = (V, E)` is a set of **vertices** (nodes) connected by **edges** (relationships). Unlike trees, graphs allow **cycles**, **disconnected components**, and arbitrary connectivity.

**Why it exists.** Graphs model *anything with relationships*: roads, social networks, dependencies, the web, circuits, state machines. Most real-world "connection" problems are graph problems.

**Problem it solves.** Reachability, shortest/cheapest paths, connectivity, ordering under constraints, matching, flow — a vast algorithmic toolbox built on one abstraction.

**Real-world analogy.** A map of cities (vertices) linked by roads (edges); roads may be one-way (directed) and have distances (weights).

---

## Terminology

- **Directed vs undirected:** edges have a direction or not.
- **Weighted vs unweighted:** edges carry a cost or not.
- **Degree:** number of incident edges (in-degree / out-degree for directed).
- **Path / cycle:** sequence of edges; a cycle returns to its start.
- **Connected component:** maximal set of mutually reachable vertices (undirected). **Strongly connected** (directed): mutual reachability both ways.
- **DAG:** directed acyclic graph — no cycles (enables topological order).
- **Dense vs sparse:** |E| near |V|² vs near |V| — drives representation choice.
- **Tree:** a connected acyclic undirected graph with |V|−1 edges.

---

## Internal Working — Representations

### Adjacency list (most common)
For each vertex, a list of its neighbors. Space O(V + E). Ideal for **sparse** graphs.
```
0: [1, 2]
1: [2]
2: [0, 3]
3: []
```

### Adjacency matrix
`adj[u][v] = 1` (or weight) if edge exists. Space O(V²). O(1) edge lookup; good for **dense** graphs or when you frequently test specific edges.

### Edge list
A flat list of `(u, v, w)`. Used by Kruskal's MST and Bellman-Ford.

| Representation | Space | Edge exists? | Iterate neighbors | Best for |
|---|---|---|---|---|
| Adjacency list | O(V+E) | O(deg) | O(deg) | sparse graphs (most) |
| Adjacency matrix | O(V²) | O(1) | O(V) | dense / frequent edge tests |
| Edge list | O(E) | O(E) | — | Kruskal, Bellman-Ford |

💡 Default to an **adjacency list**; switch to a matrix only when the graph is dense or you need O(1) edge existence tests (e.g., Floyd-Warshall).

---

## C++ Implementation

### Building an adjacency list
```cpp
#include <vector>

int V = 4;
std::vector<std::vector<int>> adj(V);          // unweighted
void addEdge(std::vector<std::vector<int>>& g, int u, int v, bool directed=false) {
    g[u].push_back(v);
    if (!directed) g[v].push_back(u);
}

// Weighted: store {neighbor, weight}
std::vector<std::vector<std::pair<int,int>>> wadj(V);
void addW(int u, int v, int w, bool directed=false) {
    wadj[u].push_back({v, w});
    if (!directed) wadj[v].push_back({u, w});
}
```

### Adjacency matrix
```cpp
const int INF = 1e9;
std::vector<std::vector<int>> mat(V, std::vector<int>(V, INF));
// mat[u][v] = w;  mat[u][u] = 0;
```

**STL note.** No standard graph type — you compose `std::vector`, `std::pair`, `std::unordered_map`. Boost Graph Library exists but is rarely needed for interviews/most projects.

**When to use which representation.** Adjacency list for almost all traversal/shortest-path work; matrix for Floyd-Warshall or dense graphs; edge list for Kruskal/Bellman-Ford.

---

## Complexity Reference (V vertices, E edges)

| Task | Adjacency list | Adjacency matrix |
|---|---|---|
| BFS / DFS | O(V+E) | O(V²) |
| Dijkstra (heap) | O(E log V) | O(V²) |
| Bellman-Ford | O(V·E) | — |
| Floyd-Warshall | — | O(V³) |
| Kruskal | O(E log E) | — |
| Prim (heap) | O(E log V) | O(V²) |
| Topological sort | O(V+E) | — |

---

## Tradeoffs
- **List vs matrix:** list saves memory on sparse graphs and iterates neighbors fast; matrix gives O(1) edge tests but O(V²) memory.
- **Directed vs undirected:** undirected edges are added both ways; forgetting this is a common bug.
- **Storing weights:** pairs/structs in the list, or values in the matrix.

---

## Use Cases
- Maps & navigation (roads, transit) → shortest path.
- Social/communication networks → connectivity, communities.
- Dependency/build systems, task scheduling → topological sort, cycle detection.
- Networks & infrastructure → MST, max-flow.
- Compilers (CFG), game AI (pathfinding), recommendation systems.

---

## Pitfalls
- ⚠️ Forgetting to add **both directions** for undirected edges.
- ⚠️ Choosing a matrix for a large sparse graph → O(V²) memory blowup.
- ⚠️ Not tracking **visited** → infinite loops on cycles.
- ⚠️ Ignoring **disconnected components** (loop over all start vertices).
- ⚠️ Off-by-one with 0- vs 1-based vertex labels.
- ⚠️ Self-loops and parallel (multi-)edges if the problem allows them.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Connections / network / dependencies / map" → **graph**.
- "Reachability / components / islands" → **BFS/DFS** ([bfs.md](bfs.md), [dfs.md](dfs.md)).
- "Shortest path" → unweighted **BFS**; weighted **Dijkstra/Bellman-Ford** ([shortest_path.md](shortest_path.md)).
- "Order under prerequisites / detect cycle in a DAG" → **topological sort** ([topological_sort.md](topological_sort.md)).
- "Cheapest way to connect everything" → **MST** ([mst.md](mst.md)).
- "Group/merge connectivity queries" → **union-find** ([../Disjoint-Set/union_find.md](../Disjoint-Set/union_find.md)).

---

## Example Problems

### Easy — Find if Path Exists in Graph
Build an adjacency list, BFS/DFS/union-find from source to destination. O(V+E).

### Medium — Number of Connected Components
Count DFS/BFS launches over unvisited vertices (or union-find sets).
```cpp
#include <vector>
void dfs(int u, std::vector<std::vector<int>>& g, std::vector<bool>& seen) {
    seen[u] = true;
    for (int v : g[u]) if (!seen[v]) dfs(v, g, seen);
}
int countComponents(int n, std::vector<std::vector<int>>& edges) {
    std::vector<std::vector<int>> g(n);
    for (auto& e : edges) { g[e[0]].push_back(e[1]); g[e[1]].push_back(e[0]); }
    std::vector<bool> seen(n, false);
    int comps = 0;
    for (int i = 0; i < n; ++i) if (!seen[i]) { ++comps; dfs(i, g, seen); }
    return comps;
}
```
**Explanation.** Each unvisited vertex starts a new component; one traversal marks its whole component. O(V+E).

### Hard — Clone Graph
DFS/BFS copying each node once, using a `map<Node*, Node*>` from originals to clones to handle cycles. O(V+E).

---

*Next →* [bfs.md](bfs.md)
