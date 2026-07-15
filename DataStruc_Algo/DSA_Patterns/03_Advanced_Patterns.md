# Part III — Advanced Patterns

[← Back to Table of Contents](README.md)

The heavy hitters: tree/graph traversals, backtracking, heaps, DP, greedy, and bit tricks. Same anatomy throughout.

---

## 3.1 Tree BFS (Level-Order)

### Intuition
Process a tree **level by level** using a queue. The size of the queue at the start of each round is exactly the number of nodes on that level.

### 🎯 When to use (signals)
- "Level order," "by depth/row," "right side view," "zigzag," "minimum depth."
- Anything that groups nodes by distance from the root.

### Template
```cpp
#include <queue>
#include <vector>
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> res;
    if (!root) return res;
    std::queue<TreeNode*> q; q.push(root);
    while (!q.empty()) {
        int sz = q.size();                  // freeze this level's size
        std::vector<int> level;
        for (int i = 0; i < sz; ++i) {
            TreeNode* n = q.front(); q.pop();
            level.push_back(n->val);
            if (n->left)  q.push(n->left);
            if (n->right) q.push(n->right);
        }
        res.push_back(level);
    }
    return res;
}
```

### Complexity. O(n) time, O(w) space (max width).

### Variations. Zigzag (reverse alternate levels), right-side view (last of each level), min/max depth, connect level-order siblings, average of levels.

### ⚠️ Pitfalls. Capture `q.size()` **before** the inner loop. Handle `root == nullptr`.

---

## 3.2 Tree DFS (Recursive Choices)

### Intuition
Recurse into subtrees, combining child results on the way back up (post-order) or carrying state down (pre-order). Most tree problems are "what do I need from my children?"

### 🎯 When to use (signals)
- Path sums, height/diameter, "does a path exist," subtree properties, LCA, validate BST.
- Anything naturally defined recursively on subtrees.

### Templates
```cpp
// Top-down (carry state down)
void dfs(TreeNode* n, int depth) {
    if (!n) return;
    // use depth / path here
    dfs(n->left, depth + 1);
    dfs(n->right, depth + 1);
}
// Bottom-up (combine child results)
int height(TreeNode* n) {
    if (!n) return 0;
    return 1 + std::max(height(n->left), height(n->right));
}
```

### Worked example — diameter of a binary tree
Bottom-up: each node returns its height; the diameter through a node = leftHeight + rightHeight.
```cpp
int diameter(TreeNode* root) {
    int best = 0;
    std::function<int(TreeNode*)> h = [&](TreeNode* n) -> int {
        if (!n) return 0;
        int L = h(n->left), R = h(n->right);
        best = std::max(best, L + R);       // path through n
        return 1 + std::max(L, R);
    };
    h(root);
    return best;
}
```

### Complexity. O(n) time, O(h) recursion stack.

### Variations. Max path sum, path sum to target, count paths summing to K (prefix-sum on tree), validate BST (bounds), LCA, serialize/deserialize, balanced check.

### ⚠️ Pitfalls. Decide pre/in/post-order deliberately. Deep trees can stack-overflow → iterate with an explicit stack. For "any path" problems, separate the *return value* from the *answer you update*.

---

## 3.3 Graph Traversal & Connectivity

### Intuition
BFS/DFS over a graph visits every reachable node once. Mark visited to avoid cycles. Foundation for connectivity, shortest unweighted paths, flood fill, bipartite checks.

### 🎯 When to use (signals)
- "Number of islands/regions/provinces," "connected components."
- "Can I reach X," shortest steps (unweighted → BFS).
- Grid problems (4/8-directional movement).

### Templates
```cpp
// BFS shortest distance (unweighted)
std::vector<int> bfs(const std::vector<std::vector<int>>& adj, int src) {
    std::vector<int> dist(adj.size(), -1);
    std::queue<int> q; dist[src] = 0; q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) if (dist[v] == -1) { dist[v] = dist[u]+1; q.push(v); }
    }
    return dist;
}
// DFS flood fill on a grid
void dfs(std::vector<std::vector<char>>& g, int r, int c) {
    if (r<0||c<0||r>=(int)g.size()||c>=(int)g[0].size()||g[r][c]!='1') return;
    g[r][c] = '0';
    dfs(g,r+1,c); dfs(g,r-1,c); dfs(g,r,c+1); dfs(g,r,c-1);
}
```

### Complexity. O(V + E) (grid: O(rows × cols)).

### Variations. Number of islands, max area of island, rotting oranges (multi-source BFS), word ladder, bipartite check (2-coloring), clone graph, surrounded regions, shortest bridge.

### ⚠️ Pitfalls. Always mark visited (or you loop forever). Loop over **all** nodes for disconnected graphs. For shortest *weighted* path use Dijkstra, not BFS. Multi-source BFS: push all sources first.

---

## 3.4 Topological Sort (Kahn / DFS)

### Intuition
Order the nodes of a **DAG** so every edge `u→v` has `u` before `v`. Models dependency resolution. Kahn's method repeatedly removes nodes with in-degree 0; leftover nodes mean a cycle.

### 🎯 When to use (signals)
- "Dependencies," "prerequisites," "build/compile order," "course schedule."
- Detect a cycle in a **directed** graph.

### Template (Kahn / BFS)
```cpp
std::vector<int> topoSort(int n, const std::vector<std::vector<int>>& adj) {
    std::vector<int> indeg(n, 0), order;
    for (int u = 0; u < n; ++u) for (int v : adj[u]) ++indeg[v];
    std::queue<int> q;
    for (int u = 0; u < n; ++u) if (!indeg[u]) q.push(u);
    while (!q.empty()) {
        int u = q.front(); q.pop(); order.push_back(u);
        for (int v : adj[u]) if (--indeg[v] == 0) q.push(v);
    }
    return order.size() == (size_t)n ? order : std::vector<int>{};  // empty ⇒ cycle
}
```

### Complexity. O(V + E).

### Variations. Course schedule I/II, alien dictionary (derive letter order), parallel-course minimum semesters (BFS levels), detect cycle in directed graph.

### ⚠️ Pitfalls. A non-DAG has **no** topological order — detect the cycle (order shorter than n). Build in-degrees correctly (edge direction matters).

---

## 3.5 Backtracking

### Intuition
Incrementally build a candidate; **prune** as soon as it can't succeed; **undo** the last choice and try the next. A systematic, smart brute force over a decision tree. Template: *choose → explore → un-choose.*

### 🎯 When to use (signals)
- "Generate all / enumerate," "all valid configurations."
- Constraint satisfaction: N-Queens, Sudoku, word search, graph coloring.
- Small n (≤ ~12–20) — exponential search is acceptable.

### Template
```cpp
void backtrack(State& s, Result& res) {
    if (isComplete(s)) { res.add(s); return; }
    for (auto& choice : choices(s)) {
        if (!valid(s, choice)) continue;   // prune
        apply(s, choice);                  // choose
        backtrack(s, res);                 // explore
        undo(s, choice);                   // un-choose
    }
}
```

### Worked example — word search in a grid
```cpp
bool exist(std::vector<std::vector<char>>& g, const std::string& w) {
    int R = g.size(), C = g[0].size();
    std::function<bool(int,int,int)> dfs = [&](int r,int c,int i)->bool{
        if (i == (int)w.size()) return true;
        if (r<0||c<0||r>=R||c>=C||g[r][c]!=w[i]) return false;
        char tmp = g[r][c]; g[r][c] = '#';        // mark used (choose)
        bool found = dfs(r+1,c,i+1)||dfs(r-1,c,i+1)
                   ||dfs(r,c+1,i+1)||dfs(r,c-1,i+1);
        g[r][c] = tmp;                            // restore (un-choose)
        return found;
    };
    for (int r=0;r<R;++r) for (int c=0;c<C;++c)
        if (dfs(r,c,0)) return true;
    return false;
}
```

### Complexity. Exponential (O(2ⁿ), O(n!)) — pruning cuts the constant/branches, not the worst-case class.

### Variations. N-Queens, Sudoku solver, combination sum, palindrome partitioning, generate parentheses, restore IP addresses, graph coloring.

### ⚠️ Pitfalls. Forgetting to **undo** state. Weak pruning → TLE. Duplicate results → sort + skip equal siblings. Mutating shared state without restoring.

---

## 3.6 Subsets / Combinations / Permutations

### Intuition
A specialized backtracking family. **Subsets:** include/exclude each element. **Combinations:** choose k of n (order irrelevant). **Permutations:** all orderings (order matters).

### 🎯 When to use (signals)
- "All subsets / power set," "all combinations of size k," "all permutations."
- Counting/enumerating arrangements; small n.

### Templates
```cpp
// Subsets
void subsets(const std::vector<int>& a, int start,
             std::vector<int>& path, std::vector<std::vector<int>>& out) {
    out.push_back(path);
    for (int i = start; i < (int)a.size(); ++i) {
        path.push_back(a[i]);
        subsets(a, i + 1, path, out);
        path.pop_back();
    }
}
// Permutations
void permute(std::vector<int>& a, int k, std::vector<std::vector<int>>& out) {
    if (k == (int)a.size()) { out.push_back(a); return; }
    for (int i = k; i < (int)a.size(); ++i) {
        std::swap(a[k], a[i]);
        permute(a, k + 1, out);
        std::swap(a[k], a[i]);
    }
}
```

### Complexity. Subsets O(2ⁿ), permutations O(n!) — output size dominates.

### Variations. Subsets with duplicates (sort + skip), combination sum I/II, permutations with duplicates, letter combinations of a phone number, k-combinations.

### ⚠️ Pitfalls. Use `start` to avoid reusing earlier elements (combinations). Handle duplicates by sorting and skipping `a[i]==a[i-1]` at the same depth.

---

## 3.7 Top-K Elements (Heap)

### Intuition
To find the K largest/smallest among n, keep a heap of size **K** instead of sorting everything. A min-heap of size K holds the K largest seen so far (its top is the smallest of them).

### 🎯 When to use (signals)
- "K largest/smallest/closest/most-frequent."
- Streaming data where full sorting is wasteful.

### Template
```cpp
#include <queue>
std::vector<int> kLargest(const std::vector<int>& a, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<>> mh;  // min-heap
    for (int x : a) { mh.push(x); if ((int)mh.size() > k) mh.pop(); }
    std::vector<int> res;
    while (!mh.empty()) { res.push_back(mh.top()); mh.pop(); }
    return res;
}
```

### Complexity. O(n log k) time, O(k) space — better than O(n log n) full sort when k ≪ n.

### Variations. Kth largest element, top-K frequent (map + heap or bucket sort), K closest points to origin, sort characters by frequency, reorganize string.

### ⚠️ Pitfalls. K **largest** uses a **min**-heap (and vice versa) — the most common mix-up. For K-th element alone, `std::nth_element` is O(n) average.

---

## 3.8 K-way Merge

### Intuition
Merge K sorted lists/arrays by always taking the smallest current head. A min-heap of the K heads gives the global minimum in O(log K) per step.

### 🎯 When to use (signals)
- "Merge K sorted lists/arrays."
- "Smallest range covering elements from K lists," "Kth smallest in a sorted matrix."

### Template
```cpp
#include <queue>
ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b){ return a->val > b->val; };
    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> pq(cmp);
    for (auto l : lists) if (l) pq.push(l);
    ListNode dummy(0); ListNode* tail = &dummy;
    while (!pq.empty()) {
        ListNode* n = pq.top(); pq.pop();
        tail->next = n; tail = n;
        if (n->next) pq.push(n->next);
    }
    return dummy.next;
}
```

### Complexity. O(N log K), where N = total elements.

### Variations. Merge K sorted arrays, smallest range covering K lists, Kth smallest in sorted matrix, find K pairs with smallest sums.

### ⚠️ Pitfalls. Push only non-null heads. Custom comparator direction (min-heap). Memory if copying instead of linking.

---

## 3.9 Two Heaps

### Intuition
Maintain a **max-heap** for the lower half and a **min-heap** for the upper half of a stream, balanced in size. The median is at the heap tops — O(1) query, O(log n) insert.

### 🎯 When to use (signals)
- "Median of a data stream," "running median."
- Problems needing the middle/balance of a dynamic set.

### Template
```cpp
#include <queue>
class MedianFinder {
    std::priority_queue<int> lo;                                   // max-heap (lower half)
    std::priority_queue<int, std::vector<int>, std::greater<>> hi; // min-heap (upper half)
public:
    void addNum(int x) {
        lo.push(x);
        hi.push(lo.top()); lo.pop();           // balance values
        if (hi.size() > lo.size()) { lo.push(hi.top()); hi.pop(); } // balance sizes
    }
    double findMedian() {
        return lo.size() > hi.size() ? lo.top()
                                     : (lo.top() + hi.top()) / 2.0;
    }
};
```

### Complexity. addNum O(log n), findMedian O(1).

### Variations. Sliding-window median, IPO (maximize capital), schedule tasks by two priorities.

### ⚠️ Pitfalls. Keep the size invariant (`lo` has equal or one more). Route the new value through both heaps to keep halves ordered.

---

## 3.10 Union-Find (DSU)

### Intuition
Track disjoint sets with near-O(1) **find** (which set?) and **union** (merge). Path compression + union by rank/size keep the trees flat.

### 🎯 When to use (signals)
- "Connected components," "are X and Y connected," "merge groups/accounts."
- Cycle detection in an **undirected** graph; Kruskal's MST.

### Template
```cpp
#include <numeric>
struct DSU {
    std::vector<int> p, r;
    DSU(int n): p(n), r(n,0) { std::iota(p.begin(), p.end(), 0); }
    int find(int x){ return p[x]==x ? x : p[x]=find(p[x]); }  // path compression
    bool unite(int a,int b){
        a=find(a); b=find(b);
        if(a==b) return false;
        if(r[a]<r[b]) std::swap(a,b);
        p[b]=a; if(r[a]==r[b]) ++r[a];
        return true;
    }
};
```

### Complexity. ~O(α(n)) ≈ O(1) per operation.

### Variations. Number of provinces, redundant connection (cycle), accounts merge, number of islands (union of cells), Kruskal's MST, smallest equivalent strings.

### ⚠️ Pitfalls. Apply **both** optimizations (else O(n)). DSU can't split/un-union. To count components, count distinct roots.

---

## 3.11 Trie (Prefix Tree)

### Intuition
A tree where each path from the root spells a prefix; nodes mark word ends. Makes **prefix** queries O(L), independent of how many words are stored.

### 🎯 When to use (signals)
- "Prefix," "autocomplete," "dictionary," "startsWith," "word search II."
- Many strings with shared prefixes; word games.

### Template
```cpp
#include <unordered_map>
struct Trie {
    struct Node { std::unordered_map<char,Node*> nxt; bool end=false; };
    Node* root = new Node();
    void insert(const std::string& w){
        Node* c = root;
        for(char ch : w){ if(!c->nxt.count(ch)) c->nxt[ch]=new Node(); c=c->nxt[ch]; }
        c->end = true;
    }
    bool startsWith(const std::string& p) const {
        const Node* c = root;
        for(char ch : p){ auto it=c->nxt.find(ch); if(it==c->nxt.end()) return false; c=it->second; }
        return true;
    }
};
```

### Complexity. insert/search/startsWith O(L); space O(total chars × alphabet).

### Variations. Implement Trie, word search II (Trie + DFS on grid), replace words, design add/search words (`.` wildcard), maximum XOR (binary trie).

### ⚠️ Pitfalls. Memory blow-up — use a fixed `Node*[26]` for lowercase to speed up. Deletion must not remove nodes shared by other words.

---

## 3.12 Dynamic Programming Patterns

### Intuition
Solve overlapping subproblems once and reuse. Define a **state**, a **transition**, base cases, and a fill order. DP isn't one pattern but a family — recognize the sub-pattern.

### 🎯 When to use (signals)
- "Number of ways," "min/max cost," "longest/shortest" **with choices**.
- Brute-force recursion recomputes the same subproblems.
- Optimal substructure + overlapping subproblems.

### Sub-patterns & state designs
| Sub-pattern | State `dp[...]` | Example |
|---|---|---|
| Linear / 1-D | `dp[i]` = answer up to i | climbing stairs, house robber, LIS |
| Knapsack (0/1) | `dp[i][w]` = best using first i, capacity w | subset sum, partition equal sum |
| Unbounded knapsack | `dp[amt]` reusing items | coin change, rod cutting |
| Grid | `dp[i][j]` | unique paths, min path sum |
| Two-sequence | `dp[i][j]` over two strings | LCS, edit distance, regex match |
| Interval | `dp[i][j]` over a range | matrix-chain, burst balloons, palindrome partition |
| DP on subsets (bitmask) | `dp[mask]` | TSP, assignment |
| DP on trees | `dp[node][state]` | house robber III, tree diameter |

### Template (top-down memoization)
```cpp
std::vector<int> memo;          // sized to #states, init to -1
int solve(int i /*state*/) {
    if (baseCase(i)) return baseValue;
    int& m = memo[i];
    if (m != -1) return m;       // reuse
    int best = WORST;
    for (auto& choice : choices(i))
        best = combine(best, solve(next(i, choice)));
    return m = best;
}
```

### Worked example — coin change (min coins, where greedy fails)
```cpp
int coinChange(const std::vector<int>& coins, int amount) {
    std::vector<int> dp(amount + 1, amount + 1);  // "infinity"
    dp[0] = 0;
    for (int a = 1; a <= amount; ++a)
        for (int c : coins)
            if (c <= a) dp[a] = std::min(dp[a], dp[a - c] + 1);
    return dp[amount] > amount ? -1 : dp[amount];
}
```

### Complexity. (#states) × (work per state). Coin change O(amount × coins).

### Variations. See the table — plus space optimization (roll 2D→1D when `dp[i]` depends only on `dp[i-1]`).

### ⚠️ Pitfalls. Wrong/insufficient state. Missing base case. Wrong iteration order (dependencies first). Off-by-one in dimensions. For 0/1 knapsack with 1-D array, iterate capacity **descending**.

---

## 3.13 Greedy Patterns

### Intuition
Build the answer with **locally optimal** choices that provably compose into a global optimum. Fast, but only correct when the greedy-choice property holds — otherwise it silently fails.

### 🎯 When to use (signals)
- "Maximum/minimum number of …," "earliest finish," "least cost."
- A natural sort order makes the best choice obvious at each step.
- You can argue an **exchange argument** (swapping to the greedy choice never hurts).

### Template
```cpp
sort(items, byGreedyCriterion);   // e.g., by finish time / ratio
for (auto& item : items)
    if (compatible(item, state))  // take it if it doesn't break feasibility
        take(item, state);
```

### Worked example — interval scheduling (max non-overlapping)
```cpp
int maxActivities(std::vector<std::pair<int,int>> v) {     // {start, finish}
    std::sort(v.begin(), v.end(),
              [](auto&a, auto&b){ return a.second < b.second; }); // earliest finish
    int count = 0, lastEnd = INT_MIN;
    for (auto& [s, f] : v)
        if (s >= lastEnd) { ++count; lastEnd = f; }
    return count;
}
```

### Complexity. Usually O(n log n) (dominated by the sort).

### Variations. Activity selection, fractional knapsack, Huffman coding, jump game, gas station, task scheduler, minimum platforms, assign cookies, non-overlapping intervals.

### ⚠️ Pitfalls. **Prove it or it's wrong** — greedy coin change fails for `{1,3,4}` making 6. When greedy fails, switch to **DP**. Sorting by the wrong key breaks correctness.

---

## 3.14 Bit Manipulation

### Intuition
Use bits as flags or a compact set. XOR cancels pairs; `n & (n-1)` clears the lowest set bit; masks enumerate subsets. Often turns O(n) space into O(1) or enables bitmask DP.

### 🎯 When to use (signals)
- "Single number" (XOR), "count set bits," "power of two."
- Subset enumeration over ≤ ~20 elements (bitmask).
- Space-constrained flags / state compression.

### Template / tricks
```cpp
x & 1                 // least significant bit (odd/even)
x >> 1                // divide by 2
x & (x - 1)           // clear lowest set bit (count bits by looping)
x & (-x)              // isolate lowest set bit
__builtin_popcount(x) // number of set bits
a ^ b ^ a == b        // XOR cancels pairs
1 << i                // i-th bit set
mask | (1<<i)         // set bit i ;  mask & ~(1<<i) clears it
(mask >> i) & 1       // test bit i
```

### Worked example — single number (every element twice except one)
```cpp
int singleNumber(const std::vector<int>& a) {
    int x = 0;
    for (int v : a) x ^= v;     // pairs cancel, the unique survives
    return x;                   // O(n) time, O(1) space
}
```

### Complexity. Typically O(n) or O(2ⁿ · n) for bitmask DP/subset enumeration.

### Variations. Single number II/III, counting bits, missing number (XOR), subsets via bitmask, TSP bitmask DP, maximum XOR (binary trie), Gray code, power of two/four.

### ⚠️ Pitfalls. Signed shifts / overflow (`1 << 31` — use `1u`/`1LL`). Operator precedence (`&` is lower than `==` — parenthesize). Off-by-one on bit indices.

---

## 3.15 Monotonic Deque (Sliding-Window Extremum)

### Intuition
A double-ended queue that stays monotonic, giving the **max/min of every window** in O(1) amortized. Indices are pushed once and popped once; the front is always the current extremum.

### 🎯 When to use (signals)
- "Maximum/minimum in each sliding window of size k."
- Window aggregate that a plain sliding window can't update in O(1).
- Optimizing certain DP transitions (sliding-window maximum DP).

### Template
```cpp
#include <deque>
std::vector<int> maxSlidingWindow(const std::vector<int>& a, int k) {
    std::deque<int> dq;                          // indices, values decreasing
    std::vector<int> res;
    for (int i = 0; i < (int)a.size(); ++i) {
        if (!dq.empty() && dq.front() <= i - k) dq.pop_front();   // drop out-of-window
        while (!dq.empty() && a[dq.back()] <= a[i]) dq.pop_back();// keep decreasing
        dq.push_back(i);
        if (i >= k - 1) res.push_back(a[dq.front()]);             // front = max
    }
    return res;
}
```

### Complexity. O(n) time, O(k) space.

### Variations. Sliding-window minimum, shortest subarray with sum ≥ K (deque on prefix sums), constrained subsequence sum, jump game VI (DP + deque).

### ⚠️ Pitfalls. Store **indices** to detect out-of-window elements. Keep strict vs non-strict monotonicity consistent. Pop from the correct end (front = expire, back = dominated).

---

## Part III Summary

| Pattern | Signal | Complexity |
|---|---|---|
| Tree BFS | level-order, by depth | O(n) |
| Tree DFS | subtree/path properties | O(n) |
| Graph traversal | components, reachability | O(V+E) |
| Topological sort | dependencies/ordering | O(V+E) |
| Backtracking | enumerate all/constraints | exponential |
| Subsets/Perms | all arrangements | O(2ⁿ)/O(n!) |
| Top-K (heap) | K largest/smallest/frequent | O(n log k) |
| K-way merge | merge K sorted | O(N log k) |
| Two heaps | running median | O(log n) ins |
| Union-Find | connectivity/merge | ~O(1) |
| Trie | prefix queries | O(L) |
| DP | overlapping subproblems | states×work |
| Greedy | provable local optimum | O(n log n) |
| Bit manipulation | flags/XOR/subset masks | O(n)/O(2ⁿ) |
| Monotonic deque | window max/min | O(n) |

---

*Next →* [Part IV: Pattern Recognition Techniques](04_Pattern_Recognition.md)
