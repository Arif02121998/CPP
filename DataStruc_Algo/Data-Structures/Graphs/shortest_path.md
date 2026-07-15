# File: Graphs/shortest_path.md

[← Repository Index](../README.md) · **Graphs:** [graph_basics](graph_basics.md) · [bfs](bfs.md) · [dfs](dfs.md) · shortest_path · [mst](mst.md) · [topological_sort](topological_sort.md) · [advanced_graph_algorithms](advanced_graph_algorithms.md)

---

## Conceptual Understanding

**Definition.** **Shortest-path** algorithms find the minimum-cost route between vertices in a weighted graph. Which one you use depends on the edge weights and whether you need single-source or all-pairs distances.

**Why it exists.** "Cheapest/fastest route" is one of the most common real problems — navigation, networks, currency arbitrage, scheduling. Different weight structures (non-negative, negative, 0/1, unweighted) demand different algorithms for correctness and efficiency.

**Real-world analogy.** GPS routing: minimize total travel time over a road network where each road segment has a cost.

---

## Choosing the Right Algorithm

| Situation | Algorithm | Time |
|---|---|---|
| Unweighted | **BFS** | O(V+E) |
| Weights ∈ {0,1} | **0-1 BFS** (deque) | O(V+E) |
| Non-negative weights, single source | **Dijkstra** | O(E log V) |
| Negative weights allowed, single source | **Bellman-Ford** | O(V·E) |
| All-pairs, small V | **Floyd-Warshall** | O(V³) |
| DAG (any weights) | **Topological + relax** | O(V+E) |
| Heuristic goal search | **A*** | depends on heuristic |

💡 **Decision flow:** unweighted → BFS; non-negative → Dijkstra; negatives → Bellman-Ford; all pairs & dense → Floyd-Warshall; DAG → topological relaxation.

---

## Dijkstra's Algorithm

**Idea.** Grow a set of finalized vertices; always finalize the nearest unfinalized vertex (greedy). Correct **only with non-negative weights** because once popped, a vertex's distance can't improve.

```cpp
#include <vector>
#include <queue>
#include <climits>

std::vector<long long> dijkstra(int src, const std::vector<std::vector<std::pair<int,int>>>& adj) {
    int n = adj.size();
    std::vector<long long> dist(n, LLONG_MAX);
    using P = std::pair<long long,int>;                       // {distance, node}
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;   // min-heap
    dist[src] = 0; pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;                            // stale entry (lazy deletion)
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;                        // relax
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
```
⚠️ `std::priority_queue` has no decrease-key; the standard workaround is to push a new pair and **skip stale entries** with `if (d > dist[u]) continue;`.
⚠️ Dijkstra is **wrong with negative edges** — use Bellman-Ford.

---

## 0-1 BFS
When weights are only 0 or 1, use a **deque**: push 0-weight relaxations to the **front**, 1-weight to the **back**. Achieves O(V+E) — faster than Dijkstra for this special case. See [../Queues/deque.md](../Queues/deque.md).
```cpp
// dist[v] = dist[u] + w;  if (w==0) dq.push_front(v); else dq.push_back(v);
```

---

## Bellman-Ford

**Idea.** Relax **all** edges V−1 times; handles negative weights and **detects negative cycles** (a further relaxation on the V-th pass means a negative cycle exists).
```cpp
struct Edge { int u, v, w; };
bool bellmanFord(int n, int src, const std::vector<Edge>& edges, std::vector<long long>& dist) {
    dist.assign(n, LLONG_MAX);
    dist[src] = 0;
    for (int i = 0; i < n - 1; ++i)
        for (auto& e : edges)
            if (dist[e.u] != LLONG_MAX && dist[e.u] + e.w < dist[e.v])
                dist[e.v] = dist[e.u] + e.w;
    for (auto& e : edges)                                   // one more pass
        if (dist[e.u] != LLONG_MAX && dist[e.u] + e.w < dist[e.v])
            return false;                                   // negative cycle
    return true;
}
```

---

## Floyd-Warshall (all-pairs)

**Idea.** DP over intermediate vertices `k`: the shortest `i→j` either avoids `k` or goes through it.
```cpp
// dist is a V×V matrix, dist[i][i]=0, dist[i][j]=w or INF
for (int k = 0; k < V; ++k)
  for (int i = 0; i < V; ++i)
    for (int j = 0; j < V; ++j)
      if (dist[i][k] < INF && dist[k][j] < INF)
        dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
```
$$dist_k[i][j] = \min(dist_{k-1}[i][j],\ dist_{k-1}[i][k] + dist_{k-1}[k][j])$$
Handles negative edges (no negative cycles); `dist[i][i] < 0` flags a negative cycle. O(V³).

---

## Complexity Summary

| Algorithm | Time | Space | Negatives | Output |
|---|---|---|---|---|
| BFS | O(V+E) | O(V) | (unweighted) | single-source |
| 0-1 BFS | O(V+E) | O(V) | 0/1 only | single-source |
| Dijkstra | O(E log V) | O(V) | ❌ | single-source |
| Bellman-Ford | O(V·E) | O(V) | ✅ (+ detect) | single-source |
| Floyd-Warshall | O(V³) | O(V²) | ✅ (+ detect) | all-pairs |

---

## Tradeoffs
- **Dijkstra vs Bellman-Ford:** Dijkstra is much faster but requires non-negative weights; Bellman-Ford is slower but handles negatives and detects negative cycles.
- **Dijkstra vs Floyd-Warshall:** run Dijkstra from every source (O(V·E log V)) for sparse graphs; Floyd-Warshall (O(V³)) is simpler and better for dense/all-pairs.
- **Heap Dijkstra vs O(V²) Dijkstra:** heap version wins on sparse graphs; the matrix O(V²) version can win on dense graphs.

---

## Use Cases
- Navigation/routing, network latency, game pathfinding (A*).
- Currency arbitrage detection (negative cycle via Bellman-Ford).
- Network routing protocols (distance-vector ≈ Bellman-Ford).
- Transit systems with 0/1 costs (0-1 BFS).

---

## Pitfalls
- ⚠️ Using Dijkstra with **negative edges** → wrong answers.
- ⚠️ Forgetting to skip **stale** heap entries (`d > dist[u]`).
- ⚠️ Integer overflow — use `long long` and guard `INF + w`.
- ⚠️ Bellman-Ford relaxing from an unreachable node (`dist[u]==INF`).
- ⚠️ Floyd-Warshall loop order must be **k outermost**.
- ⚠️ Not initializing `dist[src]=0` / diagonal `0`.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Cheapest/fastest route, non-negative costs" → **Dijkstra**.
- "Can costs be negative? / detect negative cycle" → **Bellman-Ford**.
- "All pairs of nodes / transitive" → **Floyd-Warshall**.
- "Weights are only 0 or 1" → **0-1 BFS**.
- "Unweighted minimum steps" → **BFS**.
- "Path on a DAG" → **topological order + relax**.

---

## Example Problems

### Easy — Network Delay Time
Single-source Dijkstra from the source; answer is the max finalized distance (or −1 if unreachable). O(E log V).

### Medium — Cheapest Flights Within K Stops
Bellman-Ford limited to K+1 relaxation rounds (or Dijkstra augmented with a stop count).
```cpp
int findCheapestPrice(int n, std::vector<std::vector<int>>& flights, int src, int dst, int K) {
    std::vector<int> dist(n, INT_MAX); dist[src] = 0;
    for (int i = 0; i <= K; ++i) {
        std::vector<int> tmp = dist;                    // relax using previous round only
        for (auto& f : flights) {
            int u=f[0], v=f[1], w=f[2];
            if (dist[u] != INT_MAX) tmp[v] = std::min(tmp[v], dist[u] + w);
        }
        dist = tmp;
    }
    return dist[dst] == INT_MAX ? -1 : dist[dst];
}
```
**Explanation.** Bounding the number of relaxation rounds bounds the number of stops; using a snapshot (`tmp`) prevents using more than K stops in one round. O(K·E).

### Hard — Swim in Rising Water / Path with Minimum Effort
Dijkstra-like where the "distance" is the max edge on the path (minimize the bottleneck) using a min-heap keyed by the running maximum. O(E log V).

---

*Next →* [mst.md](mst.md)
