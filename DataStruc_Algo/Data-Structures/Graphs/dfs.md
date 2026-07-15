# File: Graphs/dfs.md

[← Repository Index](../README.md) · **Graphs:** [graph_basics](graph_basics.md) · [bfs](bfs.md) · dfs · [shortest_path](shortest_path.md) · [mst](mst.md) · [topological_sort](topological_sort.md) · [advanced_graph_algorithms](advanced_graph_algorithms.md)

---

## Conceptual Understanding

**Definition.** **Depth-First Search** explores a graph by going **as deep as possible** along each branch before backtracking, using a **stack** (implicit via recursion or explicit).

**Why it exists.** DFS naturally exposes structure: connectivity, cycles, topological order, bridges/articulation points, strongly connected components. Its backtracking form powers combinatorial search.

**Problem it solves.** Connectivity/components, cycle detection, topological sorting, path existence, and the scaffolding for advanced graph decompositions.

**Real-world analogy.** Exploring a maze by always taking an unexplored corridor and only backtracking when you hit a dead end.

---

## Internal Working

Visit a vertex, recurse into an unvisited neighbor, and backtrack when stuck. A **visited** array prevents infinite loops on cycles.

```
Graph: 0-1, 0-2, 1-3, 2-3
DFS from 0 (neighbors in order):
0 → 1 → 3 → 2 → back
Visit order: 0, 1, 3, 2
```

**Edge classification (directed):** tree, back (→ ancestor, indicates a cycle), forward, cross edges. A **back edge** is the signature of a cycle.

---

## C++ Implementation

### Recursive
```cpp
#include <vector>
void dfs(int u, const std::vector<std::vector<int>>& adj, std::vector<bool>& seen) {
    seen[u] = true;
    // pre-order work on u
    for (int v : adj[u])
        if (!seen[v]) dfs(v, adj, seen);
    // post-order work on u
}
```

### Iterative (explicit stack — avoids stack overflow)
```cpp
#include <stack>
void dfsIter(int src, const std::vector<std::vector<int>>& adj) {
    std::vector<bool> seen(adj.size(), false);
    std::stack<int> st; st.push(src);
    while (!st.empty()) {
        int u = st.top(); st.pop();
        if (seen[u]) continue;
        seen[u] = true;
        for (int v : adj[u]) if (!seen[v]) st.push(v);
    }
}
```

### Cycle detection
```cpp
// Undirected: a visited neighbor that isn't the parent = cycle.
bool hasCycleUndirected(int u, int parent,
        const std::vector<std::vector<int>>& adj, std::vector<bool>& seen) {
    seen[u] = true;
    for (int v : adj[u]) {
        if (!seen[v]) { if (hasCycleUndirected(v, u, adj, seen)) return true; }
        else if (v != parent) return true;         // back edge
    }
    return false;
}

// Directed: use 3 colors (0=unvisited,1=in-stack,2=done); a '1' neighbor = cycle.
bool hasCycleDirected(int u, const std::vector<std::vector<int>>& adj, std::vector<int>& color) {
    color[u] = 1;                                   // on recursion stack
    for (int v : adj[u]) {
        if (color[v] == 1) return true;             // back edge → cycle
        if (color[v] == 0 && hasCycleDirected(v, adj, color)) return true;
    }
    color[u] = 2;                                   // fully processed
    return false;
}
```

**STL note.** Recursion uses the call stack; iterative uses `std::stack`. Prefer iterative for very deep graphs to avoid overflow.

---

## Complexity

| | Time | Space |
|---|---|---|
| Adjacency list | O(V + E) | O(V) (recursion/visited) |
| Adjacency matrix | O(V²) | O(V) |
| Recursion depth | up to O(V) | (stack overflow risk on deep graphs) |

---

## Tradeoffs
- ✅ Low memory on deep, narrow graphs (O(depth)); elegant recursion.
- ✅ Reveals structural properties (cycles, SCCs, bridges, topological order).
- ❌ Does **not** give shortest paths in unweighted graphs (use BFS).
- ❌ Deep recursion risks stack overflow — use the iterative form.

**DFS vs BFS:** DFS for connectivity, ordering, and combinatorial search; BFS for shortest unweighted paths and level structure.

---

## Use Cases
- Connected components, path existence, flood fill.
- Cycle detection (directed & undirected).
- Topological sort ([topological_sort.md](topological_sort.md)).
- Strongly connected components (Tarjan/Kosaraju), bridges & articulation points ([advanced_graph_algorithms.md](advanced_graph_algorithms.md)).
- Backtracking: permutations, subsets, N-Queens, Sudoku, word search.
- Tree/graph serialization, maze solving.

---

## Pitfalls
- ⚠️ Missing/incorrect visited handling → infinite loop on cycles.
- ⚠️ Undirected cycle check must skip the **parent** edge; directed needs **3-color** (in-stack) tracking, not just visited.
- ⚠️ Stack overflow on deep recursion (long chains, big grids) — go iterative.
- ⚠️ Doing pre-order work when the problem needs post-order (e.g., topological order uses post-order/finish times).
- ⚠️ Not iterating all components for disconnected graphs.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Connected / reachable / components / islands" → **DFS or BFS**.
- "Detect a cycle" → **DFS** (undirected: parent check; directed: 3-color).
- "Order with dependencies / build order" → **DFS post-order (topological)**.
- "All paths / combinations / place items with constraints" → **DFS backtracking**.
- "Strongly connected / bridges / articulation" → **DFS with discovery/low times**.

---

## Example Problems

### Easy — Number of Islands (DFS)
DFS each land cell, sinking its whole island. O(R·C).
```cpp
void sink(std::vector<std::vector<char>>& g, int r, int c) {
    if (r<0||r>=(int)g.size()||c<0||c>=(int)g[0].size()||g[r][c]!='1') return;
    g[r][c] = '0';
    sink(g,r+1,c); sink(g,r-1,c); sink(g,r,c+1); sink(g,r,c-1);
}
```
**Explanation.** Each DFS launch removes one island; count launches. O(R·C).

### Medium — Course Schedule (cycle detection in a DAG)
Model prerequisites as directed edges; a cycle means the schedule is impossible. Use the 3-color DFS above. O(V+E).

### Hard — Word Search II / Number of Distinct Islands
DFS backtracking with pruning (often combined with a [trie](../Tries/trie.md) for Word Search II) to explore board paths efficiently.

---

*Next →* [shortest_path.md](shortest_path.md)
