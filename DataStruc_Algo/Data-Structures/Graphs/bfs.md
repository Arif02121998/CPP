# File: Graphs/bfs.md

[← Repository Index](../README.md) · **Graphs:** [graph_basics](graph_basics.md) · bfs · [dfs](dfs.md) · [shortest_path](shortest_path.md) · [mst](mst.md) · [topological_sort](topological_sort.md) · [advanced_graph_algorithms](advanced_graph_algorithms.md)

---

## Conceptual Understanding

**Definition.** **Breadth-First Search** explores a graph **level by level**: it visits all vertices at distance 1 from the source, then all at distance 2, and so on, using a **queue** (FIFO).

**Why it exists.** Because it expands in order of increasing distance, BFS finds the **shortest path in an unweighted graph** (fewest edges) — something DFS cannot guarantee.

**Problem it solves.** Shortest paths in unweighted graphs, level/depth computations, minimum number of steps/moves, and reachability.

**Real-world analogy.** Ripples from a stone dropped in water: they reach nearby points first, then spread outward uniformly.

---

## Internal Working

Maintain a **queue** of the frontier and a **visited** set. Dequeue a vertex, enqueue its unvisited neighbors (marking them visited **when enqueued**).

```
Graph (undirected):  0-1, 0-2, 1-3, 2-3, 3-4
Start at 0:

Level 0: [0]
Level 1: [1, 2]
Level 2: [3]
Level 3: [4]
Visit order: 0, 1, 2, 3, 4
```

⚠️ Mark visited **at enqueue time**, not dequeue time — otherwise a vertex can be queued multiple times before being processed.

---

## Operations / Template

| Step | Action |
|---|---|
| init | queue ← source; mark source visited; dist[source]=0 |
| loop | pop u; for each neighbor v unvisited → mark, set dist, push |
| level | snapshot `queue.size()` to process one level at a time |

---

## C++ Implementation

### Standard BFS + shortest distance (unweighted)
```cpp
#include <vector>
#include <queue>

std::vector<int> bfs(int src, const std::vector<std::vector<int>>& adj) {
    int n = adj.size();
    std::vector<int> dist(n, -1);          // -1 = unreached
    std::queue<int> q;
    dist[src] = 0; q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u])
            if (dist[v] == -1) {           // first time we reach v = shortest
                dist[v] = dist[u] + 1;
                q.push(v);
            }
    }
    return dist;                            // dist[v] = fewest edges src→v
}
```

### Level-by-level (when you need the layer)
```cpp
while (!q.empty()) {
    int sz = q.size();                      // one full level
    for (int i = 0; i < sz; ++i) {
        int u = q.front(); q.pop();
        // process u at this level
        for (int v : adj[u]) if (!seen[v]) { seen[v] = true; q.push(v); }
    }
    // ++level;
}
```

### Grid BFS (very common)
```cpp
int dr[] = {1,-1,0,0}, dc[] = {0,0,1,-1};
// for each popped (r,c), try (r+dr[k], c+dc[k]) within bounds and unvisited
```

**STL note.** Built on `std::queue`. Use a `std::vector<bool>`/2D array for visited; a `std::vector<int>` for distances.

---

## Complexity

| | Time | Space |
|---|---|---|
| Adjacency list | O(V + E) | O(V) |
| Adjacency matrix | O(V²) | O(V) |
| Grid (R×C) | O(R·C) | O(R·C) |

---

## Tradeoffs
- ✅ Guarantees shortest path (fewest edges) in unweighted graphs.
- ✅ Finds nodes in increasing distance order; natural for "minimum steps."
- ❌ Uses O(width) memory — a wide graph queues many nodes at once (can exceed DFS's O(depth)).
- ❌ Does **not** handle weighted shortest paths (use Dijkstra/0-1 BFS).

**BFS vs DFS:** BFS for shortest unweighted paths and level info; DFS for connectivity, cycle detection, topological sort, and lower memory on deep narrow graphs.

---

## Use Cases
- Shortest path / minimum moves in unweighted graphs and grids (mazes, word ladder, knight moves).
- Level-order tree traversal.
- Connected components, bipartite checking (2-coloring).
- Multi-source spread (rotting oranges, nearest exit, 0/1 matrix distance).
- Flood fill.

---

## Multi-source BFS
Seed the queue with **all** sources at distance 0; BFS then computes each cell's distance to the *nearest* source in one pass. Ideal for "distance to nearest X" problems (rotting oranges, walls-and-gates, 01-matrix).

---

## Pitfalls
- ⚠️ Marking visited at **dequeue** instead of enqueue → duplicates, TLE.
- ⚠️ Using BFS for **weighted** shortest paths (wrong — use Dijkstra).
- ⚠️ Forgetting bounds checks in grid BFS.
- ⚠️ Not resetting/using a global visited across disconnected components when required.
- ⚠️ Confusing distance in edges (BFS) vs weighted distance.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Minimum number of steps/moves/transformations" (unweighted) → **BFS**.
- "Shortest path in a grid/maze" → **grid BFS**.
- "Distance to nearest source" → **multi-source BFS**.
- "Level by level / by rounds" → **BFS with level snapshot**.
- "Is the graph bipartite?" → **BFS 2-coloring**.
- Edge weights only 0/1 → **0-1 BFS with a deque** ([shortest_path.md](shortest_path.md)).

---

## Example Problems

### Easy — Flood Fill / Number of Islands
BFS each unvisited land cell, flooding its component. O(R·C). (Islands code in [../Queues/queue.md](../Queues/queue.md).)

### Medium — Rotting Oranges (multi-source BFS)
```cpp
#include <vector>
#include <queue>
int orangesRotting(std::vector<std::vector<int>>& g) {
    int R = g.size(), C = g[0].size(), fresh = 0, minutes = 0;
    std::queue<std::pair<int,int>> q;
    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c)
            if (g[r][c] == 2) q.push({r,c});
            else if (g[r][c] == 1) ++fresh;
    int dr[]={1,-1,0,0}, dc[]={0,0,1,-1};
    while (!q.empty() && fresh) {
        int sz = q.size();
        for (int i = 0; i < sz; ++i) {
            auto [r,c] = q.front(); q.pop();
            for (int k = 0; k < 4; ++k) {
                int nr=r+dr[k], nc=c+dc[k];
                if (nr>=0&&nr<R&&nc>=0&&nc<C&&g[nr][nc]==1) {
                    g[nr][nc]=2; --fresh; q.push({nr,nc});
                }
            }
        }
        ++minutes;
    }
    return fresh ? -1 : minutes;
}
```
**Explanation.** All initially-rotten oranges start at level 0; each BFS level = one minute; unreachable fresh oranges → −1. O(R·C).

### Hard — Word Ladder
Shortest transformation sequence between two words; each word is a node, edges connect words differing by one letter. BFS gives the minimum steps. O(N · L · 26).

---

*Next →* [dfs.md](dfs.md)
