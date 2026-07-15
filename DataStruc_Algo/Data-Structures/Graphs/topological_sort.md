# File: Graphs/topological_sort.md

[← Repository Index](../README.md) · **Graphs:** [graph_basics](graph_basics.md) · [bfs](bfs.md) · [dfs](dfs.md) · [shortest_path](shortest_path.md) · [mst](mst.md) · topological_sort · [advanced_graph_algorithms](advanced_graph_algorithms.md)

---

## Conceptual Understanding

**Definition.** A **topological sort** of a **directed acyclic graph (DAG)** is a linear ordering of its vertices such that for every directed edge `u → v`, `u` comes **before** `v`. It exists **iff** the graph has no cycle.

**Why it exists.** Any system of **dependencies/prerequisites** needs a valid execution order: build systems, course schedules, task pipelines, spreadsheet recalculation. Topological sort produces such an order (or proves none exists — a cycle).

**Problem it solves.** Ordering under "must come before" constraints, and cycle detection in directed graphs.

**Real-world analogy.** Getting dressed: socks before shoes, shirt before jacket. Many valid orders exist, but never shoes-before-socks.

---

## Internal Working — Two Approaches

### Kahn's Algorithm (BFS, in-degree)
1. Compute in-degree of every vertex.
2. Queue all vertices with in-degree 0.
3. Repeatedly pop a vertex, append to order, and decrement neighbors' in-degrees; enqueue any that reach 0.
4. If the order contains fewer than V vertices → a **cycle** exists.

```
Edges: 5→0, 5→2, 4→0, 4→1, 2→3, 3→1
in-degree 0: {4,5}  → pop → reduce → ...
A valid order: 4 5 2 3 1 0
```

### DFS + finish times
Run DFS; **push each vertex onto a stack after** exploring all its descendants (post-order). The reversed stack is a topological order. Detect cycles with the 3-color method.

---

## C++ Implementation

### Kahn's Algorithm
```cpp
#include <vector>
#include <queue>
std::vector<int> topoKahn(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> indeg(n, 0);
    for (int u = 0; u < n; ++u) for (int v : adj[u]) ++indeg[v];
    std::queue<int> q;
    for (int i = 0; i < n; ++i) if (indeg[i] == 0) q.push(i);
    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : adj[u]) if (--indeg[v] == 0) q.push(v);
    }
    if ((int)order.size() != n) return {};      // cycle → no valid ordering
    return order;
}
```

### DFS version
```cpp
bool dfs(int u, const std::vector<std::vector<int>>& adj,
         std::vector<int>& color, std::vector<int>& order) {
    color[u] = 1;                                // on stack
    for (int v : adj[u]) {
        if (color[v] == 1) return false;         // back edge → cycle
        if (color[v] == 0 && !dfs(v, adj, color, order)) return false;
    }
    color[u] = 2;
    order.push_back(u);                          // post-order
    return true;
}
std::vector<int> topoDFS(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> color(n, 0), order;
    for (int i = 0; i < n; ++i)
        if (color[i] == 0 && !dfs(i, adj, color, order)) return {};  // cycle
    std::reverse(order.begin(), order.end());    // reverse post-order
    return order;
}
```

**STL note.** Kahn's uses `std::queue` and an in-degree array; DFS uses recursion/`std::stack`.

**Which to use.** Kahn's when you also want cycle detection cleanly and can process greedily (and it naturally supports **lexicographically smallest** order with a `priority_queue`). DFS when you already traverse with DFS or need finish-time structure (also used for SCCs).

---

## Complexity

| Approach | Time | Space |
|---|---|---|
| Kahn's (BFS) | O(V + E) | O(V) |
| DFS + finish | O(V + E) | O(V) (recursion) |

---

## Tradeoffs
- ✅ Linear time; simultaneously detects cycles.
- ✅ Kahn's gives incremental "ready tasks"; easy to get lexicographically smallest order (min-heap).
- ❌ Only defined for DAGs; a cycle means no valid ordering.
- ❌ The order is generally **not unique** (many valid topological orders).

**Kahn vs DFS:** Kahn is iterative (no stack-overflow risk) and intuitive for scheduling; DFS integrates with other DFS-based analyses.

---

## Use Cases
- Build systems / package managers (compile/install order).
- Course scheduling with prerequisites.
- Task/job scheduling, workflow engines, spreadsheet recalculation.
- Dependency resolution, symbol resolution in compilers.
- Deadlock detection (cycle in a resource graph).
- DAG shortest/longest path (relax vertices in topological order).

---

## Pitfalls
- ⚠️ Running topological sort on a graph **with a cycle** — always check `order.size() == V`.
- ⚠️ Forgetting to reverse the DFS post-order.
- ⚠️ Not handling **disconnected** DAGs (start from every unvisited/in-degree-0 vertex).
- ⚠️ Confusing in-degree and out-degree in Kahn's.
- ⚠️ Assuming a unique answer when many valid orders exist.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Order tasks/courses with prerequisites" → **topological sort**.
- "Is it possible to finish all / detect a dependency cycle" → **Kahn's (count) or DFS 3-color**.
- "Lexicographically smallest valid order" → **Kahn's with a min-heap**.
- "Longest path in a DAG / critical path" → **topological order + DP relaxation**.
- "Recompute values in dependency order" → **topological sort**.

---

## Example Problems

### Easy — Course Schedule
Can all courses be finished? True iff the prerequisite DAG has no cycle → Kahn's produces all V vertices. O(V+E).

### Medium — Course Schedule II
Return an actual valid order (or empty if impossible) — exactly `topoKahn` above. O(V+E).

### Hard — Alien Dictionary
Infer letter ordering from a sorted word list: compare adjacent words to derive edges `c1 → c2` at the first differing character, then topologically sort the letters.
```cpp
// For each adjacent word pair, find first differing char → add edge; then Kahn's.
// Watch the invalid case: a prefix appearing after its longer word.
```
**Explanation.** Adjacent sorted words reveal pairwise letter precedence; a topological order of the letter graph is the alphabet (empty if contradictory/cyclic). O(total characters).

---

*Next →* [advanced_graph_algorithms.md](advanced_graph_algorithms.md)
