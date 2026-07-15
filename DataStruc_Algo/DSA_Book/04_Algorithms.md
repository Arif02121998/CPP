# Chapter 4 — Algorithms

[← Back to Table of Contents](README.md)

Each algorithm follows: **Concept → Problem Pattern → Step-by-Step → C++ → Complexity → Variants.**

---

## 4.1 Searching — Binary Search & Variants

### A. Concept

Binary search finds a target in a **sorted** collection by repeatedly **halving** the search space. Compare the middle; if it matches, done; if the target is smaller, search the left half; else the right half.

**Why it works.** Sorted order guarantees that one comparison eliminates *half* the remaining candidates. Each step throws away 50%, so after `log₂ n` steps the space is exhausted.

### B. Problem Pattern
- Data is **sorted** (or can be sorted, or has a monotonic property).
- You're searching for a value, a boundary, or "the smallest/largest x satisfying a condition."
- 💡 **Binary search on the answer:** when checking "is X feasible?" is monotonic (if X works, X+1 works), binary-search the answer space even without an explicit array.

### C. Step-by-Step (search 7 in [1,3,5,7,9,11])
```
lo=0, hi=5         mid=2 → a[2]=5 < 7 → go right, lo=3
lo=3, hi=5         mid=4 → a[4]=9 > 7 → go left,  hi=3
lo=3, hi=3         mid=3 → a[3]=7 = 7 → FOUND at index 3
```

### D. C++ Implementation

```cpp
#include <vector>

// Classic: returns index of target, or -1
int binarySearch(const std::vector<int>& a, int target) {
    int lo = 0, hi = (int)a.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;     // avoids overflow vs (lo+hi)/2
        if (a[mid] == target) return mid;
        if (a[mid] < target) lo = mid + 1;
        else                 hi = mid - 1;
    }
    return -1;
}

// lower_bound: first index with a[i] >= target (insertion point)
int lowerBound(const std::vector<int>& a, int target) {
    int lo = 0, hi = a.size();             // note: hi = size (half-open)
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < target) lo = mid + 1;
        else                 hi = mid;
    }
    return lo;
}

// Binary search on the answer: smallest x in [lo,hi] with feasible(x) true
int searchAnswer(int lo, int hi, auto feasible) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (feasible(mid)) hi = mid;
        else               lo = mid + 1;
    }
    return lo;
}
```

**STL:** `std::binary_search` (bool), `std::lower_bound` / `std::upper_bound` (iterators). Use them in real code.

### E. Complexity. Time **O(log n)**, space **O(1)** iterative / O(log n) recursive.

### F. Variants
- **lower_bound / upper_bound** — leftmost/rightmost boundary.
- **First/last occurrence** of a duplicate value.
- **Rotated sorted array** search (modified binary search).
- **Binary search on answer** — min/max feasible value (e.g., "minimum capacity to ship in D days").
- ⚠️ Pitfalls: `mid` overflow (use `lo + (hi-lo)/2`); infinite loops from wrong `lo/hi` updates; off-by-one in boundary variants.

---

## 4.2 Sorting

> 💡 **Comparison sorts cannot beat O(n log n)** in the worst case (information-theoretic lower bound). To go faster you must exploit structure (counting/radix on bounded integers).

| Algorithm | Best | Average | Worst | Space | Stable | Notes |
|---|---|---|---|---|---|---|
| Bubble | O(n) | O(n²) | O(n²) | O(1) | ✅ | teaching only |
| Insertion | O(n) | O(n²) | O(n²) | O(1) | ✅ | great for small/nearly-sorted |
| Selection | O(n²) | O(n²) | O(n²) | O(1) | ❌ | min swaps |
| **Merge** | O(n log n) | O(n log n) | O(n log n) | O(n) | ✅ | stable, predictable, linked lists |
| **Quick** | O(n log n) | O(n log n) | O(n²) | O(log n) | ❌ | fastest in practice |
| **Heap** | O(n log n) | O(n log n) | O(n log n) | O(1) | ❌ | in-place, guaranteed |
| Counting | O(n+k) | O(n+k) | O(n+k) | O(k) | ✅ | small integer range |
| Radix | O(d·n) | O(d·n) | O(d·n) | O(n+k) | ✅ | fixed-width integers |

### 4.2.1 Merge Sort (Divide & Conquer, stable)

**Concept.** Split the array in half, recursively sort each half, then **merge** the two sorted halves in linear time. Works because merging two sorted lists is O(n).

**Step-by-step:** `[5,2,4,1]` → split `[5,2]`,`[4,1]` → sort → `[2,5]`,`[1,4]` → merge → `[1,2,4,5]`.

```cpp
#include <vector>
void merge(std::vector<int>& a, int l, int m, int r) {
    std::vector<int> tmp;
    tmp.reserve(r - l + 1);
    int i = l, j = m + 1;
    while (i <= m && j <= r)
        tmp.push_back(a[i] <= a[j] ? a[i++] : a[j++]);   // <= keeps it stable
    while (i <= m) tmp.push_back(a[i++]);
    while (j <= r) tmp.push_back(a[j++]);
    for (int k = 0; k < (int)tmp.size(); ++k) a[l + k] = tmp[k];
}
void mergeSort(std::vector<int>& a, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(a, l, m);
    mergeSort(a, m + 1, r);
    merge(a, l, m, r);
}
```
**Complexity:** O(n log n) always; O(n) space. **Use when** stability matters, worst-case guarantees are needed, or sorting linked lists / external (on-disk) data.

### 4.2.2 Quick Sort (Divide & Conquer, in-place)

**Concept.** Pick a **pivot**, **partition** so smaller elements go left and larger go right, then recurse on each side. The pivot lands in its final position each step.

**Why O(n²) worst case.** If the pivot is always the min/max (e.g., already-sorted input with a naive pivot), partitions are maximally unbalanced. **Fix:** randomized or median-of-three pivot.

```cpp
#include <vector>
#include <cstdlib>
int partition(std::vector<int>& a, int lo, int hi) {
    int pivot = a[hi];                    // Lomuto scheme
    int i = lo - 1;
    for (int j = lo; j < hi; ++j)
        if (a[j] < pivot) std::swap(a[++i], a[j]);
    std::swap(a[i + 1], a[hi]);
    return i + 1;                          // pivot's final index
}
void quickSort(std::vector<int>& a, int lo, int hi) {
    if (lo >= hi) return;
    int p = lo + std::rand() % (hi - lo + 1);  // randomize to avoid O(n^2)
    std::swap(a[p], a[hi]);
    int pi = partition(a, lo, hi);
    quickSort(a, lo, pi - 1);
    quickSort(a, pi + 1, hi);
}
```
**Complexity:** O(n log n) avg, O(n²) worst, O(log n) stack. **Fastest in practice** (cache-friendly, low constants) — the basis of `std::sort` (introsort: quicksort + heapsort fallback + insertion for small ranges).

### 4.2.3 Heap Sort (in-place, guaranteed)

**Concept.** Build a max-heap (O(n)), then repeatedly swap the root (max) to the end and sift down the reduced heap.

```cpp
void heapify(std::vector<int>& a, int n, int i) {
    int largest = i, l = 2*i+1, r = 2*i+2;
    if (l < n && a[l] > a[largest]) largest = l;
    if (r < n && a[r] > a[largest]) largest = r;
    if (largest != i) { std::swap(a[i], a[largest]); heapify(a, n, largest); }
}
void heapSort(std::vector<int>& a) {
    int n = a.size();
    for (int i = n/2 - 1; i >= 0; --i) heapify(a, n, i);   // build heap O(n)
    for (int i = n - 1; i > 0; --i) {
        std::swap(a[0], a[i]);                              // max to end
        heapify(a, i, 0);
    }
}
```
**Complexity:** O(n log n) always, O(1) space. Not stable; slower than quicksort in practice (poor cache behavior) but gives worst-case guarantee in-place.

**STL:** Always use `std::sort` (introsort, O(n log n) guaranteed), `std::stable_sort` (stable, O(n log n)), `std::partial_sort` (top-k), `std::nth_element` (O(n) selection).

---

## 4.3 Recursion & Backtracking

### A. Concept

**Backtracking** builds a solution **incrementally**, abandoning ("pruning") a partial candidate as soon as it cannot lead to a valid complete solution, then **undoing** the last choice and trying the next. It's a systematic, smart brute force over a decision tree.

**The template:** *choose → explore → un-choose.*

### B. Problem Pattern
- Generate **all** combinations/permutations/subsets.
- Constraint-satisfaction (N-Queens, Sudoku, graph coloring).
- "Find a path / arrangement that satisfies …".
- Keywords: "all possible", "enumerate", "valid configurations".

### C. Step-by-Step (subsets of [1,2,3])
Decision tree: at each element, *include* or *exclude* it → 2³ = 8 subsets. Backtracking walks this tree, adding to the path on the way down and removing on the way back up.

### D. C++ Implementation

```cpp
#include <vector>

// All subsets (power set)
void subsets(const std::vector<int>& nums, int start,
             std::vector<int>& path, std::vector<std::vector<int>>& out) {
    out.push_back(path);                       // record current subset
    for (int i = start; i < (int)nums.size(); ++i) {
        path.push_back(nums[i]);               // choose
        subsets(nums, i + 1, path, out);       // explore
        path.pop_back();                       // un-choose (backtrack)
    }
}

// All permutations
void permute(std::vector<int>& nums, int k,
             std::vector<std::vector<int>>& out) {
    if (k == (int)nums.size()) { out.push_back(nums); return; }
    for (int i = k; i < (int)nums.size(); ++i) {
        std::swap(nums[k], nums[i]);           // choose position k
        permute(nums, k + 1, out);             // explore
        std::swap(nums[k], nums[i]);           // backtrack
    }
}

// N-Queens — count valid placements
bool safe(std::vector<int>& col, int r, int c) {
    for (int i = 0; i < r; ++i)
        if (col[i] == c || abs(col[i] - c) == abs(i - r)) return false;
    return true;                               // pruning: reject conflicts early
}
int nQueens(int n, int r, std::vector<int>& col) {
    if (r == n) return 1;
    int count = 0;
    for (int c = 0; c < n; ++c)
        if (safe(col, r, c)) {
            col[r] = c;                        // choose
            count += nQueens(n, r + 1, col);   // explore
            // implicit un-choose: overwrite next iteration
        }
    return count;
}
```

### E. Complexity. Often **exponential** (O(2ⁿ), O(n!)) — that's inherent to enumeration. **Pruning** cuts the constant factor / branches dramatically but rarely changes the worst-case class.

### F. Variants. Combination sum, palindrome partitioning, word search, Sudoku solver, generating parentheses. Optimizations: pruning, ordering choices, bitmask state, memoization (→ turns into DP).

---

## 4.4 Greedy Algorithms

### A. Concept

A greedy algorithm builds a solution by **always taking the locally optimal choice**, never reconsidering. It works **only when** local optima compose into a global optimum.

**When it works — two properties:**
1. **Greedy-choice property:** a globally optimal solution can be reached by local optimal choices.
2. **Optimal substructure:** an optimal solution contains optimal solutions to subproblems.

> ⚠️ Greedy is **fast but often wrong**. You must *prove* it (exchange argument) or it'll fail edge cases. Example: greedy coin change works for {1,5,10,25} but **fails** for {1,3,4} making 6 (greedy: 4+1+1=3 coins; optimal: 3+3=2 coins).

### B. Problem Pattern
- Optimization ("maximize/minimize") with a natural ordering.
- Interval scheduling, Huffman coding, MST, fractional knapsack.
- Keywords: "minimum number of", "maximum profit", "earliest finish".

### C. Step-by-Step (Activity Selection)
Sort activities by **finish time**; greedily pick each activity that starts after the last chosen one finishes. Earliest-finish-first leaves maximum room for the rest — provably optimal.

### D. C++ Implementation

```cpp
#include <vector>
#include <algorithm>

// Activity selection: max non-overlapping intervals
int maxActivities(std::vector<std::pair<int,int>> v) {  // {start, finish}
    std::sort(v.begin(), v.end(),
              [](auto& a, auto& b){ return a.second < b.second; }); // by finish
    int count = 0, lastEnd = INT_MIN;
    for (auto& [s, f] : v)
        if (s >= lastEnd) { ++count; lastEnd = f; }     // take it
    return count;
}

// Fractional knapsack: take highest value/weight ratio first
double fractionalKnapsack(std::vector<std::pair<int,int>> items, int W) {
    std::sort(items.begin(), items.end(), [](auto& a, auto& b){
        return (double)a.first/a.second > (double)b.first/b.second;
    }); // {value, weight}, by ratio desc
    double total = 0;
    for (auto& [val, wt] : items) {
        if (W >= wt) { total += val; W -= wt; }
        else { total += val * ((double)W / wt); break; } // take fraction
    }
    return total;
}
```

### E. Complexity. Usually dominated by the **sort: O(n log n)**.

### F. Variants. Huffman coding, Dijkstra (greedy + PQ), Prim/Kruskal MST, job sequencing, gas station, jump game. ⚠️ Always verify greedy correctness — if it fails, reach for **DP**.

---

## 4.5 Dynamic Programming (DP)

### A. Concept

DP solves problems by combining solutions to **overlapping subproblems**, storing each subproblem's answer once (**memoization**) so it's never recomputed. It turns exponential recursion into polynomial time.

**Two prerequisites (same as greedy's #2 plus overlap):**
1. **Optimal substructure** — optimal solution built from optimal sub-solutions.
2. **Overlapping subproblems** — the same subproblems recur many times.

> 💡 **Greedy vs DP:** greedy commits to one choice; DP explores *all* choices but reuses results. Use DP when greedy's local choice isn't provably global.

**Two styles:**
- **Top-down (memoization):** recursion + cache. Natural, computes only needed states.
- **Bottom-up (tabulation):** iterative, fill a table from base cases. No recursion overhead, often more space-efficient.

### B. Problem Pattern
- "Number of ways", "min/max cost/path", "longest/shortest …".
- Choices at each step with overlapping futures.
- Can you define a **state** and a **transition** (recurrence)? Then it's DP.

### C. The DP recipe
1. **Define the state** — what parameters uniquely identify a subproblem? `dp[i]` = answer for prefix of length i.
2. **Transition** — express `dp[i]` from smaller states.
3. **Base cases.**
4. **Order** — ensure dependencies are computed first.
5. **Answer** — which state holds the final result.

### D. Worked Examples

```cpp
#include <vector>
#include <algorithm>
#include <string>

// 1) Fibonacci: naive O(2^n) → DP O(n)
long long fib(int n) {
    if (n < 2) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; ++i) { long long c = a + b; a = b; b = c; }
    return b;                                   // O(n) time, O(1) space
}

// 2) 0/1 Knapsack — classic 2D DP
int knapsack(const std::vector<int>& wt, const std::vector<int>& val, int W) {
    int n = wt.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(W + 1, 0));
    for (int i = 1; i <= n; ++i)
        for (int w = 0; w <= W; ++w) {
            dp[i][w] = dp[i-1][w];                              // skip item i
            if (wt[i-1] <= w)                                  // or take it
                dp[i][w] = std::max(dp[i][w], val[i-1] + dp[i-1][w - wt[i-1]]);
        }
    return dp[n][W];                            // O(nW) time and space
}

// 3) Longest Common Subsequence
int lcs(const std::string& a, const std::string& b) {
    int m = a.size(), n = b.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            dp[i][j] = (a[i-1] == b[j-1])
                     ? dp[i-1][j-1] + 1
                     : std::max(dp[i-1][j], dp[i][j-1]);
    return dp[m][n];                            // O(mn)
}

// 4) Coin change — min coins to make amount (where greedy fails)
int coinChange(const std::vector<int>& coins, int amount) {
    std::vector<int> dp(amount + 1, amount + 1);
    dp[0] = 0;
    for (int a = 1; a <= amount; ++a)
        for (int c : coins)
            if (c <= a) dp[a] = std::min(dp[a], dp[a - c] + 1);
    return dp[amount] > amount ? -1 : dp[amount];
}
```

### E. Complexity. Typically **(number of states) × (work per state)**. Knapsack: O(nW). LCS: O(mn). The art is minimizing states.

### F. Variants & optimizations
- **Space optimization:** if `dp[i]` only depends on `dp[i-1]`, keep two rows / one row (knapsack → 1D).
- **State compression:** bitmask DP (TSP: O(2ⁿ·n²)).
- Categories: 0/1 & unbounded knapsack, LIS, edit distance, matrix-chain, interval DP, tree DP, digit DP, DP on subsets.
- ⚠️ Common bugs: wrong state definition, missing base case, wrong iteration order, off-by-one in dimensions.

---

## 4.6 Graph Algorithms

Graph representations are in [Chapter 3.3](03_Advanced_Data_Structures.md#33-graphs).

### 4.6.1 BFS — Breadth-First Search

**Concept.** Explore level by level from a source using a **queue**. Visits all nodes at distance 1, then 2, etc. On **unweighted** graphs this finds **shortest paths** (fewest edges).

```cpp
#include <vector>
#include <queue>
std::vector<int> bfs(const std::vector<std::vector<int>>& adj, int src) {
    int n = adj.size();
    std::vector<int> dist(n, -1);
    std::queue<int> q;
    dist[src] = 0; q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u])
            if (dist[v] == -1) {               // unvisited
                dist[v] = dist[u] + 1;
                q.push(v);
            }
    }
    return dist;                                // shortest unweighted distances
}
```
**Complexity:** O(V + E). **Use for:** shortest path (unweighted), level-order, connected components, bipartite check, "minimum steps" puzzles.

### 4.6.2 DFS — Depth-First Search

**Concept.** Go as deep as possible before backtracking, using **recursion** (implicit stack) or an explicit stack.

```cpp
void dfs(int u, const std::vector<std::vector<int>>& adj, std::vector<bool>& vis) {
    vis[u] = true;
    // process u
    for (int v : adj[u])
        if (!vis[v]) dfs(v, adj, vis);
}
```
**Complexity:** O(V + E). **Use for:** cycle detection, topological sort, connected components, path existence, maze/flood-fill, tree/graph recursion.

> ⚠️ On large graphs (10⁶ nodes), recursive DFS can stack-overflow — convert to an explicit stack.

### 4.6.3 Topological Sort (DAG ordering)

**Concept.** Linear ordering of a DAG where every edge `u→v` has `u` before `v`. Models dependency resolution (build order, course prerequisites).

```cpp
// Kahn's algorithm (BFS-based) — also detects cycles
#include <vector>
#include <queue>
std::vector<int> topoSort(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> indeg(n, 0), order;
    for (int u = 0; u < n; ++u) for (int v : adj[u]) ++indeg[v];
    std::queue<int> q;
    for (int u = 0; u < n; ++u) if (indeg[u] == 0) q.push(u);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int v : adj[u]) if (--indeg[v] == 0) q.push(v);
    }
    return order.size() == (size_t)n ? order : std::vector<int>{}; // empty = cycle
}
```
**Complexity:** O(V + E). **Use for:** build systems, task scheduling, course schedule, dependency graphs.

### 4.6.4 Dijkstra — Shortest Path (non-negative weights)

**Concept.** Greedy + min-heap. Repeatedly extract the closest unfinalized node and **relax** its edges (improve neighbors' tentative distances). Correct because, with non-negative weights, the closest unfinalized node's distance is already final.

```cpp
#include <vector>
#include <queue>
#include <climits>
struct Edge { int to; long long w; };
std::vector<long long> dijkstra(const std::vector<std::vector<Edge>>& g, int src) {
    int n = g.size();
    std::vector<long long> dist(n, LLONG_MAX);
    using P = std::pair<long long,int>;        // {distance, node}
    std::priority_queue<P, std::vector<P>, std::greater<>> pq;  // MIN-heap
    dist[src] = 0; pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;             // stale entry, skip
        for (auto& e : g[u])
            if (dist[u] + e.w < dist[e.to]) {  // relax
                dist[e.to] = dist[u] + e.w;
                pq.push({dist[e.to], e.to});
            }
    }
    return dist;
}
```
**Complexity:** O((V + E) log V) with a binary heap. ⚠️ **Fails with negative edges** — use Bellman-Ford. ⚠️ Remember the **min**-heap.

### 4.6.5 Bellman-Ford — Shortest Path with negative edges

**Concept.** Relax **all** edges `V−1` times. After `k` rounds, shortest paths using ≤ k edges are correct; `V−1` rounds suffice (no shortest path has more than V−1 edges). A V-th relaxation that still improves → **negative cycle**.

```cpp
#include <vector>
#include <climits>
struct E { int u, v; long long w; };
bool bellmanFord(int n, const std::vector<E>& edges, int src,
                 std::vector<long long>& dist) {
    dist.assign(n, LLONG_MAX);
    dist[src] = 0;
    for (int i = 1; i < n; ++i)
        for (auto& e : edges)
            if (dist[e.u] != LLONG_MAX && dist[e.u] + e.w < dist[e.v])
                dist[e.v] = dist[e.u] + e.w;
    for (auto& e : edges)                       // V-th pass: detect neg cycle
        if (dist[e.u] != LLONG_MAX && dist[e.u] + e.w < dist[e.v])
            return false;                        // negative cycle exists
    return true;
}
```
**Complexity:** O(V·E). **Use when** edges can be negative or you must detect negative cycles (e.g., arbitrage).

### 4.6.6 Minimum Spanning Tree (MST)

A **spanning tree** connects all vertices with V−1 edges and no cycle. The **MST** minimizes total edge weight. Two greedy algorithms:

**Kruskal** — sort edges by weight, add each edge if it doesn't form a cycle (use **DSU**).
```cpp
#include <vector>
#include <algorithm>
// DSU from Chapter 3.7
struct KEdge { int u, v, w; };
long long kruskal(int n, std::vector<KEdge> edges) {
    std::sort(edges.begin(), edges.end(), [](auto&a, auto&b){ return a.w < b.w; });
    DSU dsu(n);
    long long cost = 0; int used = 0;
    for (auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {              // no cycle → take edge
            cost += e.w;
            if (++used == n - 1) break;
        }
    }
    return cost;
}
```
**Prim** — grow the tree from a start vertex, always adding the cheapest edge crossing the frontier (use a min-heap).
```cpp
#include <vector>
#include <queue>
long long prim(const std::vector<std::vector<Edge>>& g, int n) {
    std::vector<bool> inMST(n, false);
    using P = std::pair<long long,int>;
    std::priority_queue<P, std::vector<P>, std::greater<>> pq;
    long long cost = 0; pq.push({0, 0});
    while (!pq.empty()) {
        auto [w, u] = pq.top(); pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true; cost += w;
        for (auto& e : g[u])
            if (!inMST[e.to]) pq.push({e.w, e.to});
    }
    return cost;
}
```
**Complexity:** Kruskal O(E log E) — best for **sparse** graphs; Prim O(E log V) — best for **dense** graphs. **Use for:** network design (cables, roads), clustering, approximation algorithms.

---

## 4.7 Sliding Window

### A. Concept

Maintain a **window** (contiguous subarray/substring) that slides across the data, updating an aggregate **incrementally** instead of recomputing from scratch. Turns O(n²) brute force into **O(n)**.

### B. Problem Pattern
- Contiguous subarray/substring with a constraint ("longest substring without repeats", "max sum of size k", "smallest subarray ≥ target").
- **Fixed window:** size k given. **Variable window:** grow/shrink to satisfy a condition.

### C. Step-by-Step (max sum of k consecutive)
Compute the first window's sum; then slide: add the entering element, subtract the leaving one. Each element enters and leaves once → O(n).

### D. C++ Implementation

```cpp
#include <vector>
#include <unordered_map>
#include <algorithm>

// Fixed window: max sum of any k consecutive elements
int maxSumK(const std::vector<int>& a, int k) {
    int sum = 0;
    for (int i = 0; i < k; ++i) sum += a[i];
    int best = sum;
    for (int i = k; i < (int)a.size(); ++i) {
        sum += a[i] - a[i - k];                 // slide: add new, drop old
        best = std::max(best, sum);
    }
    return best;
}

// Variable window: longest substring without repeating characters
int longestUnique(const std::string& s) {
    std::unordered_map<char, int> last;          // char → last index seen
    int start = 0, best = 0;
    for (int end = 0; end < (int)s.size(); ++end) {
        if (last.count(s[end]) && last[s[end]] >= start)
            start = last[s[end]] + 1;            // shrink window past the repeat
        last[s[end]] = end;
        best = std::max(best, end - start + 1);
    }
    return best;
}
```
**Complexity:** O(n) time, O(1) or O(k) space. **Variants:** min-window-substring, max-window with monotonic deque, anagram search, "at most K distinct".

---

## 4.8 Two Pointers

### A. Concept

Use **two indices** that move toward each other (or in the same direction) to scan data in O(n) instead of O(n²). Works on **sorted** arrays or for in-place partitioning.

### B. Problem Pattern
- Sorted array + pair/triplet with target sum.
- In-place removal/dedup/partition.
- Palindrome check; merging two sorted sequences; fast-slow (cycle detection).

### D. C++ Implementation

```cpp
#include <vector>

// Two-sum on a SORTED array — O(n)
std::pair<int,int> twoSumSorted(const std::vector<int>& a, int target) {
    int l = 0, r = a.size() - 1;
    while (l < r) {
        int s = a[l] + a[r];
        if (s == target) return {l, r};
        if (s < target) ++l;                    // need larger → move left up
        else            --r;                    // need smaller → move right down
    }
    return {-1, -1};
}

// Remove duplicates from sorted array in-place — O(n), O(1)
int removeDuplicates(std::vector<int>& a) {
    if (a.empty()) return 0;
    int slow = 0;                                // last unique position
    for (int fast = 1; fast < (int)a.size(); ++fast)
        if (a[fast] != a[slow]) a[++slow] = a[fast];
    return slow + 1;                             // new length
}

// Floyd's cycle detection (fast & slow pointers)
struct ListNode { int val; ListNode* next; };
bool hasCycle(ListNode* head) {
    ListNode* slow = head; ListNode* fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;           // pointers met → cycle
    }
    return false;
}
```
**Complexity:** O(n) time, O(1) space. **Variants:** 3Sum (sort + two pointers), container-with-most-water, partition (Dutch national flag), merge step of merge sort.

---

## 4.9 Divide & Conquer

### A. Concept

**Divide** the problem into smaller independent subproblems, **conquer** each recursively, then **combine** their results. Complexity follows the Master Theorem ([Ch 1.3.5](01_Foundations.md#135-the-master-theorem-for-divide--conquer)).

### B. Problem Pattern
- Problem splits cleanly into independent halves.
- Combine step is efficient.
- Examples: merge sort, quick sort, binary search, closest pair, Karatsuba multiplication, Strassen, FFT.

### D. C++ Implementation

```cpp
// Maximum subarray via divide & conquer — O(n log n)
#include <vector>
#include <algorithm>
#include <climits>
int maxCrossing(const std::vector<int>& a, int l, int m, int r) {
    int leftSum = INT_MIN, s = 0;
    for (int i = m; i >= l; --i) { s += a[i]; leftSum = std::max(leftSum, s); }
    int rightSum = INT_MIN; s = 0;
    for (int i = m + 1; i <= r; ++i) { s += a[i]; rightSum = std::max(rightSum, s); }
    return leftSum + rightSum;
}
int maxSubarray(const std::vector<int>& a, int l, int r) {
    if (l == r) return a[l];                     // base case
    int m = l + (r - l) / 2;
    return std::max({ maxSubarray(a, l, m),       // entirely left
                      maxSubarray(a, m + 1, r),   // entirely right
                      maxCrossing(a, l, m, r) }); // crosses the middle
}
```
> 💡 The same problem (maximum subarray) is O(n) with **Kadane's DP** — a great example of how recognizing the right paradigm (DP) beats divide & conquer. Always ask whether subproblems *overlap* (→ DP) or are *independent* (→ D&C).

**Complexity:** depends on the recurrence; this one is O(n log n). **Variants:** counting inversions, closest pair of points (O(n log n)), median of two sorted arrays, fast exponentiation.

---

## Chapter Summary

| Paradigm | Core idea | Signature complexity | Use when |
|---|---|---|---|
| Binary Search | halve search space | O(log n) | sorted / monotonic |
| Sorting | order data | O(n log n) | preprocessing, enables other algos |
| Backtracking | choose-explore-undo | exponential | enumerate all solutions |
| Greedy | local optimum | O(n log n) | provable greedy-choice property |
| DP | reuse overlapping subproblems | poly (states×work) | overlapping subproblems |
| BFS/DFS | traverse graph | O(V+E) | connectivity, shortest unweighted |
| Dijkstra | greedy + heap | O((V+E) log V) | non-negative weighted shortest path |
| Bellman-Ford | relax V−1 times | O(VE) | negative edges / cycle detection |
| MST | greedy edges | O(E log V) | minimum connection cost |
| Sliding Window | incremental window | O(n) | contiguous subarray/substring |
| Two Pointers | converging indices | O(n) | sorted pairs, in-place |
| Divide & Conquer | split-solve-combine | per Master Theorem | independent subproblems |

---

*Next →* [Chapter 5: Problem-Solving Techniques](05_Problem_Solving.md)
