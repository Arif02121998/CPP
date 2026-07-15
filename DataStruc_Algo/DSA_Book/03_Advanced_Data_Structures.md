# Chapter 3 — Advanced Data Structures

[← Back to Table of Contents](README.md)

---

## 3.1 Trees

### A. Conceptual Layer

**Definition.** A tree is a **hierarchical**, connected, acyclic structure of **nodes**. One node is the **root**; every other node has exactly one **parent** and zero or more **children**. Nodes with no children are **leaves**.

**Why it exists.** Linear structures (arrays, lists) model sequences. The real world is full of **hierarchies** — file systems, org charts, DOM, decision processes. Trees also enable **O(log n)** search/insert/delete when balanced, beating linear structures while staying ordered (unlike hash tables).

**Terminology:**
- **Depth** of a node = edges from the root to it. **Height** of a node = edges to its deepest leaf. **Height of tree** = height of root.
- **Subtree** = a node plus all its descendants.
- **Binary tree** = each node has ≤ 2 children (`left`, `right`).

**Real-world analogy.** A family tree / company org chart: one root (CEO), each person reports to exactly one manager, branches spread downward.

### B. Traversals (the foundation of all tree algorithms)

```
        1
       / \
      2   3
     / \
    4   5
```

| Traversal | Order | Result | Use |
|---|---|---|---|
| **Pre-order** | Root → Left → Right | 1 2 4 5 3 | copy/serialize tree |
| **In-order** | Left → Root → Right | 4 2 5 1 3 | **sorted output for BST** |
| **Post-order** | Left → Right → Root | 4 5 2 3 1 | delete tree, evaluate expr |
| **Level-order (BFS)** | level by level | 1 2 3 4 5 | shortest path, by depth |

```cpp
struct TreeNode {
    int val;
    TreeNode* left  = nullptr;
    TreeNode* right = nullptr;
    TreeNode(int v) : val(v) {}
};

void inorder(TreeNode* n) {                 // DFS, recursive
    if (!n) return;
    inorder(n->left);
    std::cout << n->val << ' ';             // visit between children
    inorder(n->right);
}

#include <queue>
void levelOrder(TreeNode* root) {           // BFS, uses a queue
    if (!root) return;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        TreeNode* n = q.front(); q.pop();
        std::cout << n->val << ' ';
        if (n->left)  q.push(n->left);
        if (n->right) q.push(n->right);
    }
}
```

---

### 3.1.1 Binary Search Tree (BST)

**Definition.** A binary tree with the **BST property**: for every node, all keys in its **left** subtree are **smaller**, all keys in its **right** subtree are **larger**. This ordering makes search a guided descent.

**Why it exists.** Combines the **ordering** of a sorted array with the **fast insertion** of a linked list. In-order traversal yields sorted data for free, and you can answer range/successor queries — things a hash table can't.

**How search works (step-by-step).** To find `k`: start at root; if `k < node`, go left; if `k > node`, go right; repeat. Each comparison halves the search space *if balanced* → O(log n).

```
Search 40 in:        50
                    /  \
                  30    70
                 / \   / \
               20  40 60  80
50 → 40<50 go left → 30 → 40>30 go right → 40 ✓  (3 comparisons)
```

**Complexity:**

| Operation | Balanced | Degenerate (sorted insert) |
|---|---|---|
| Search/Insert/Delete | O(log n) | O(n) |

> ⚠️ **The fatal flaw:** inserting sorted data (1,2,3,4,5) makes the BST a straight line (a linked list) → O(n). This is *why* self-balancing trees (AVL, Red-Black) exist.

```cpp
class BST {
    struct Node { int key; Node *l=nullptr, *r=nullptr; Node(int k):key(k){} };
    Node* root = nullptr;

    Node* insert(Node* n, int k) {
        if (!n) return new Node(k);
        if (k < n->key)      n->l = insert(n->l, k);
        else if (k > n->key) n->r = insert(n->r, k);
        // equal keys ignored (set semantics)
        return n;
    }
    bool find(Node* n, int k) const {
        while (n) {
            if (k == n->key) return true;
            n = (k < n->key) ? n->l : n->r;
        }
        return false;
    }
    Node* minNode(Node* n) { while (n->l) n = n->l; return n; }

    Node* remove(Node* n, int k) {
        if (!n) return nullptr;
        if (k < n->key)      n->l = remove(n->l, k);
        else if (k > n->key) n->r = remove(n->r, k);
        else {                                   // found node to delete
            if (!n->l) { Node* r = n->r; delete n; return r; }   // 0/1 child
            if (!n->r) { Node* l = n->l; delete n; return l; }
            Node* s = minNode(n->r);             // 2 children: inorder successor
            n->key = s->key;
            n->r = remove(n->r, s->key);
        }
        return n;
    }
public:
    void insert(int k) { root = insert(root, k); }
    bool find(int k) const { return find(root, k); }
    void remove(int k) { root = remove(root, k); }
};
```

**STL equivalents.** `std::set` / `std::map` / `std::multiset` / `std::multimap` are balanced BSTs (Red-Black trees) — **always use these in real code.** They guarantee O(log n) and keep keys sorted.

```cpp
#include <set>
std::set<int> s = {50, 30, 70};
s.insert(40);
bool has = s.count(40);              // O(log n)
auto it = s.lower_bound(35);         // first ≥ 35  → 40  (range queries!)
// In-order iteration gives sorted order automatically:
for (int x : s) std::cout << x << ' ';   // 30 40 50 70
```

**When to use STL vs custom.** Use `std::set`/`std::map`. Implement a BST yourself only to learn or when you need a custom augmentation (e.g., order-statistics tree).

---

### 3.1.2 AVL Tree (Self-Balancing BST)

**Definition.** A BST that maintains the **AVL invariant**: for every node, the heights of its two subtrees differ by **at most 1** (balance factor ∈ {−1, 0, +1}). After each insert/delete, it **rotates** to restore balance, guaranteeing height **O(log n)**.

**Why it exists.** To kill the degenerate-BST problem. By rebalancing on every modification, AVL guarantees O(log n) worst-case — no adversarial input can make it linear.

**Balance factor** = height(left) − height(right). When it becomes ±2, rebalance via **rotations:**

```
Right rotation (fixes Left-Left case):
      z                              y
     / \                           /   \
    y   T4   right-rotate(z)      x     z
   / \         ───────────>      / \   / \
  x   T3                       T1  T2 T3 T4
 / \
T1  T2
```

Four cases and their fixes:
- **LL** (left-heavy, inserted in left-left) → single **right** rotation.
- **RR** → single **left** rotation.
- **LR** → **left** rotation on child, then **right** on node.
- **RL** → **right** on child, then **left** on node.

```cpp
struct AVLNode {
    int key, height = 1;
    AVLNode *l = nullptr, *r = nullptr;
    AVLNode(int k) : key(k) {}
};
int h(AVLNode* n) { return n ? n->height : 0; }
int balance(AVLNode* n) { return n ? h(n->l) - h(n->r) : 0; }
void update(AVLNode* n) { n->height = 1 + std::max(h(n->l), h(n->r)); }

AVLNode* rotateRight(AVLNode* z) {
    AVLNode* y = z->l; AVLNode* T3 = y->r;
    y->r = z; z->l = T3;
    update(z); update(y);
    return y;                       // new subtree root
}
AVLNode* rotateLeft(AVLNode* z) {
    AVLNode* y = z->r; AVLNode* T2 = y->l;
    y->l = z; z->r = T2;
    update(z); update(y);
    return y;
}
AVLNode* insert(AVLNode* n, int k) {
    if (!n) return new AVLNode(k);
    if (k < n->key) n->l = insert(n->l, k);
    else if (k > n->key) n->r = insert(n->r, k);
    else return n;
    update(n);
    int b = balance(n);
    if (b >  1 && k < n->l->key) return rotateRight(n);            // LL
    if (b < -1 && k > n->r->key) return rotateLeft(n);            // RR
    if (b >  1 && k > n->l->key) { n->l = rotateLeft(n->l);  return rotateRight(n); } // LR
    if (b < -1 && k < n->r->key) { n->r = rotateRight(n->r); return rotateLeft(n); }  // RL
    return n;
}
```

**AVL vs Red-Black.** AVL is **more strictly balanced** → faster lookups, but more rotations on insert/delete. Red-Black (what `std::map` uses) is looser → fewer rotations, better for write-heavy workloads. **Use AVL for read-heavy, RB for write-heavy.**

**Pitfalls:** ⚠️ forgetting to update heights after rotation; ⚠️ mixing up the four rotation cases; ⚠️ deletion needs rebalancing up the whole path, not just at the deleted node.

---

## 3.2 Heaps

### A. Conceptual Layer

**Definition.** A heap is a **complete binary tree** (all levels full except possibly the last, filled left-to-right) satisfying the **heap property**:
- **Max-heap:** every parent ≥ its children → max at root.
- **Min-heap:** every parent ≤ its children → min at root.

**Why it exists.** When you repeatedly need the **extreme element** (max or min) but don't need full sorting. A heap gives O(1) peek and O(log n) insert/extract — far cheaper than re-sorting or scanning every time.

**Key insight: heaps live in arrays.** Because the tree is *complete*, it maps perfectly onto an array with no pointers:
- node at index `i` → left child `2i+1`, right child `2i+2`, parent `(i-1)/2`.

```
Array: [50, 30, 40, 10, 20, 35]   (max-heap)
                50              index 0
              /    \
            30      40          index 1,2
           /  \    /
          10  20  35            index 3,4,5
```

**Operations:**

| Operation | Complexity | How |
|---|---|---|
| peek (top) | O(1) | root = index 0 |
| insert (push) | O(log n) | add at end, **sift up** |
| extract (pop) | O(log n) | swap root with last, remove, **sift down** |
| build heap | O(n) | heapify bottom-up (not O(n log n)!) |

### B. Internal Working

**Insert (sift-up):** place new element at the end (next array slot, keeps completeness), then bubble it up while it violates the heap property with its parent.

**Extract-max (sift-down):** the root is the answer; move the last element to the root, then push it down, swapping with the larger child until the property holds.

> 💡 **Build-heap is O(n), not O(n log n).** Most nodes are near the bottom and sift down only a little; the math sums to O(n). This is why heapsort's build phase is cheap.

### C. C++ Implementation

```cpp
#include <vector>
#include <algorithm>

class MaxHeap {
    std::vector<int> h;
    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (h[i] <= h[p]) break;
            std::swap(h[i], h[p]);
            i = p;
        }
    }
    void siftDown(int i) {
        int n = h.size();
        while (true) {
            int largest = i, l = 2*i+1, r = 2*i+2;
            if (l < n && h[l] > h[largest]) largest = l;
            if (r < n && h[r] > h[largest]) largest = r;
            if (largest == i) break;
            std::swap(h[i], h[largest]);
            i = largest;
        }
    }
public:
    void push(int x) { h.push_back(x); siftUp(h.size() - 1); }
    int  top() const { return h.front(); }
    void pop() {
        h.front() = h.back();
        h.pop_back();
        if (!h.empty()) siftDown(0);
    }
    bool empty() const { return h.empty(); }
};
```

**STL equivalents.** `std::priority_queue` (max-heap by default). Low-level: `std::make_heap`, `std::push_heap`, `std::pop_heap` operate on any random-access range.

```cpp
#include <queue>
std::priority_queue<int> maxh;                                   // max-heap
std::priority_queue<int, std::vector<int>, std::greater<>> minh; // min-heap

// Top-K largest with a min-heap of size k — O(n log k)
std::vector<int> topK(const std::vector<int>& a, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<>> mh;
    for (int x : a) {
        mh.push(x);
        if ((int)mh.size() > k) mh.pop();   // drop the smallest
    }
    std::vector<int> res;
    while (!mh.empty()) { res.push_back(mh.top()); mh.pop(); }
    return res;
}
```

### D–F. Tradeoffs, Use Cases, Pitfalls
- ✅ O(1) peek, O(log n) update, O(n) build, array-backed (cache-friendly), no pointers.
- ❌ **No efficient search** for arbitrary elements (O(n)); not sorted (only the root is ordered).
- **Use cases:** priority queues, Dijkstra/Prim, heapsort, top-K, median (two heaps), event simulation, Huffman.
- ⚠️ A heap is **partially ordered** — don't expect sorted iteration. ⚠️ Default PQ is **max**-heap; use `greater<>` for min. ⚠️ Searching/removing a non-top element is O(n).

---

## 3.3 Graphs

### A. Conceptual Layer

**Definition.** A graph `G = (V, E)` is a set of **vertices** `V` connected by **edges** `E`. Edges may be **directed** (one-way) or **undirected** (two-way), and **weighted** or unweighted.

**Why it exists.** Graphs model **arbitrary relationships** — anything that's "connected to" something else. Trees are a special case (acyclic, connected). When relationships aren't hierarchical (social networks, maps, dependencies), you need graphs.

**Real-world analogy.** A road map: cities = vertices, roads = edges, distances = weights. Or a social network: people = vertices, friendships = edges.

**Terminology:** degree (edges at a vertex), path, cycle, connected component, DAG (directed acyclic graph), dense vs sparse.

### B. Representations (the key design decision)

| Representation | Space | Edge exists? | Neighbors | Best for |
|---|---|---|---|---|
| **Adjacency Matrix** | O(V²) | O(1) | O(V) | dense graphs, fast edge lookup |
| **Adjacency List** | O(V + E) | O(degree) | O(degree) | **sparse graphs (most real ones)** |
| **Edge List** | O(E) | O(E) | O(E) | Kruskal's MST, simple storage |

```
Graph:   0 — 1
         |   |
         2 — 3

Adjacency Matrix:        Adjacency List:
   0 1 2 3               0: [1, 2]
0 [0 1 1 0]              1: [0, 3]
1 [1 0 0 1]              2: [0, 3]
2 [1 0 0 1]              3: [1, 2]
3 [0 1 1 0]
```

> 💡 **Real-world graphs are sparse** (E ≪ V²): use an **adjacency list**. Use a matrix only when the graph is dense or you need O(1) edge-existence checks (e.g., Floyd-Warshall).

### C. C++ Implementation

```cpp
#include <vector>

// Adjacency list — the default choice
class Graph {
    int V;
    std::vector<std::vector<int>> adj;       // adj[u] = neighbors of u
public:
    explicit Graph(int v) : V(v), adj(v) {}
    void addEdge(int u, int w, bool directed = false) {
        adj[u].push_back(w);
        if (!directed) adj[w].push_back(u);
    }
    const std::vector<int>& neighbors(int u) const { return adj[u]; }
    int size() const { return V; }
};

// Weighted adjacency list
struct Edge { int to, weight; };
std::vector<std::vector<Edge>> wadj;         // wadj[u] = {{to,w},...}
```

(Traversals and graph algorithms — BFS, DFS, Dijkstra, MST — are covered in [Chapter 4](04_Algorithms.md).)

### D–F.
- ✅ Models any relationship; flexible.
- ❌ Algorithms can be complex; matrix wastes space on sparse graphs.
- **Use cases:** maps/GPS, social networks, web crawling, dependency resolution (build systems, package managers), network routing, recommendation engines.
- ⚠️ For undirected graphs, add the edge **both ways**. ⚠️ Watch for **disconnected components** (loop over all vertices). ⚠️ Self-loops and parallel edges. ⚠️ Directed cycles break topological sort.

---

## 3.4 Hash Tables

### A. Conceptual Layer

**Definition.** A hash table stores **key→value** pairs and uses a **hash function** to map a key to an array index (bucket), giving **average O(1)** insert/search/delete.

**Why it exists.** Arrays give O(1) access *by integer index*. Hash tables generalize this to O(1) access *by any key* (strings, objects). They are the fastest general-purpose lookup structure — the reason `O(1)` "just use a hash map" is the most common interview optimization.

**Real-world analogy.** A coat check: you hand over a coat (key), get a numbered tag (hash), and the attendant places it on hook #tag. Retrieval is instant — no scanning all coats.

**Operations:** insert, search, delete — **O(1) average**, **O(n) worst** (all keys collide).

### B. Internal Working

1. **Hash function** maps key → integer, then `% capacity` → bucket index. A good hash spreads keys **uniformly**.
2. **Collisions** (two keys → same bucket) are inevitable (pigeonhole). Two resolution strategies:

**Chaining** — each bucket holds a linked list (or vector) of entries:
```
bucket 0: → (key1,v1) → (key4,v4)
bucket 1: → (key2,v2)
bucket 2: (empty)
bucket 3: → (key3,v3)
```

**Open addressing** — on collision, probe other slots (linear/quadratic/double hashing) until an empty one:
```
insert k → idx busy → try idx+1, idx+2, ... (linear probing)
```

3. **Load factor** α = entries / buckets. When α exceeds a threshold (~0.7–1.0), the table **rehashes** (doubles capacity, re-inserts all) to keep operations O(1). This is amortized like vector growth.

> 💡 The "O(1)" is *amortized average* under a good hash function and bounded load factor. A poor hash or adversarial keys degrade it to O(n).

### C. C++ Implementation

```cpp
#include <vector>
#include <list>
#include <utility>

// Hash table with separate chaining
template <typename K, typename V>
class HashMap {
    std::vector<std::list<std::pair<K, V>>> buckets;
    size_t count = 0;
    static constexpr double MAX_LOAD = 0.75;

    size_t idx(const K& k) const { return std::hash<K>{}(k) % buckets.size(); }
    void rehash() {
        std::vector<std::list<std::pair<K, V>>> old = std::move(buckets);
        buckets.assign(old.size() * 2, {});
        count = 0;
        for (auto& chain : old)
            for (auto& kv : chain) put(kv.first, kv.second);
    }
public:
    HashMap() : buckets(8) {}
    void put(const K& key, const V& val) {
        auto& chain = buckets[idx(key)];
        for (auto& kv : chain)
            if (kv.first == key) { kv.second = val; return; }  // update
        chain.emplace_back(key, val);
        if (++count > MAX_LOAD * buckets.size()) rehash();
    }
    V* get(const K& key) {
        for (auto& kv : buckets[idx(key)])
            if (kv.first == key) return &kv.second;
        return nullptr;                                        // not found
    }
    bool erase(const K& key) {
        auto& chain = buckets[idx(key)];
        for (auto it = chain.begin(); it != chain.end(); ++it)
            if (it->first == key) { chain.erase(it); --count; return true; }
        return false;
    }
};
```

**STL equivalents.**
- `std::unordered_map` / `std::unordered_set` — hash tables, **O(1) average**.
- `std::map` / `std::set` — balanced BSTs, **O(log n)** but **sorted**.

```cpp
#include <unordered_map>
std::unordered_map<std::string, int> freq;
for (const std::string& w : words) ++freq[w];   // word count — O(1) avg per word
if (freq.count("hello")) /* ... */;
```

**When to use which:**

| Need | Use |
|---|---|
| Fastest lookup, no ordering | `unordered_map` / `unordered_set` |
| Sorted iteration, range/`lower_bound` queries | `map` / `set` |
| Guaranteed worst-case bounds | `map` (O(log n)) over hash (O(n) worst) |

### D. Tradeoffs
- ✅ O(1) average operations; flexible keys.
- ❌ No ordering; O(n) worst case; memory overhead (buckets, pointers); rehash pauses; cache-unfriendly (chaining).

### E. Use Cases
- Frequency counting, deduplication, caching/memoization.
- Database indexing, symbol tables in compilers.
- Two-sum and "have I seen this?" problems, sets, joins.

### F. Pitfalls
- ⚠️ **Custom key types need a hash specialization** (and `operator==`). Forgetting this won't compile or behaves wrong.
- ⚠️ `map[key]` **inserts a default value** if the key is absent — use `.count()`/`.find()` to *check* without inserting.
- ⚠️ **Iteration order is unspecified** for `unordered_map` — never depend on it.
- ⚠️ A bad hash function collapses everything to O(n) (hash flooding / DoS risk).

---

## 3.5 Tries (Prefix Trees)

### A. Conceptual Layer

**Definition.** A trie is a tree where each **edge** represents a character and each **path from the root** spells a string prefix. Nodes mark whether a complete word ends there.

**Why it exists.** Storing many strings in a hash set answers "is this exact word present?" but **not** "which words start with `pre`?". Tries make **prefix** queries first-class: autocomplete, spell-check, and prefix counting become O(L) where L = word length, *independent of how many words are stored*.

**Real-world analogy.** A dictionary organized by spelling: to find all words starting with "car", you go c→a→r and read everything below — cart, carbon, care…

```
Insert: cat, car, card, dog
              (root)
             /      \
            c        d
            |        |
            a        o
           / \       |
          t*  r      g*
              |
            (r ends "car")*
              |
              d*
* = end of word
```

**Operations:** insert / search / startsWith / delete — **O(L)** per operation (L = key length). Space can be large: O(total characters × alphabet) worst case.

### C. C++ Implementation

```cpp
#include <unordered_map>
#include <string>

class Trie {
    struct Node {
        std::unordered_map<char, Node*> next;
        bool end = false;
    };
    Node* root = new Node();
public:
    void insert(const std::string& w) {
        Node* cur = root;
        for (char c : w) {
            if (!cur->next.count(c)) cur->next[c] = new Node();
            cur = cur->next[c];
        }
        cur->end = true;
    }
    bool search(const std::string& w) const {
        const Node* n = walk(w);
        return n && n->end;
    }
    bool startsWith(const std::string& prefix) const {
        return walk(prefix) != nullptr;
    }
private:
    const Node* walk(const std::string& s) const {
        const Node* cur = root;
        for (char c : s) {
            auto it = cur->next.find(c);
            if (it == cur->next.end()) return nullptr;
            cur = it->second;
        }
        return cur;
    }
};
```

> 💡 For a fixed lowercase alphabet, replace the map with `Node* next[26]` for speed (array indexing instead of hashing). Tradeoff: more memory per node.

### D–F.
- ✅ O(L) prefix operations independent of dictionary size; shares common prefixes (memory-efficient for many shared prefixes); supports ordered traversal.
- ❌ High memory overhead per node (pointers/maps); cache-unfriendly.
- **Use cases:** autocomplete, spell-checkers, IP routing (longest-prefix match), dictionary/word games, T9 text, contact search.
- ⚠️ Memory blow-up with sparse data; deletion is tricky (must not remove nodes shared by other words); choose array vs map nodes based on alphabet size.

---

## 3.6 Segment Trees (and Fenwick / BIT)

### A. Conceptual Layer

**Definition.** A segment tree is a binary tree over an array where each node stores an **aggregate** (sum, min, max, gcd…) of a **contiguous range**. It answers **range queries** and **point/range updates** in **O(log n)**.

**Why it exists.** Given an array and many queries like "sum of elements `[l, r]`" *interleaved with updates*: a plain array gives O(n) per query, a prefix-sum array gives O(1) query but O(n) update. Segment trees give **O(log n) for both** — the sweet spot for dynamic range queries.

**Real-world analogy.** A management hierarchy where each manager knows the total of their whole team. To get a department total you ask a few managers (log n) instead of every employee (n).

```
Array: [1, 3, 5, 7, 9, 11]   range-sum segment tree:
                     [0..5]=36
                   /          \
            [0..2]=9          [3..5]=27
            /     \           /      \
       [0..1]=4   [2]=5   [3..4]=16  [5]=11
        /   \              /    \
     [0]=1 [1]=3        [3]=7  [4]=9
```

**Complexity:** build O(n), query O(log n), update O(log n), space O(n) (≈4n array).

### C. C++ Implementation (range sum, point update)

```cpp
#include <vector>

class SegmentTree {
    int n;
    std::vector<long long> tree;          // 1-indexed, size 4n
    void build(const std::vector<int>& a, int node, int lo, int hi) {
        if (lo == hi) { tree[node] = a[lo]; return; }
        int mid = (lo + hi) / 2;
        build(a, 2*node,   lo,    mid);
        build(a, 2*node+1, mid+1, hi);
        tree[node] = tree[2*node] + tree[2*node+1];   // combine
    }
    long long query(int node, int lo, int hi, int l, int r) {
        if (r < lo || hi < l) return 0;               // no overlap (identity)
        if (l <= lo && hi <= r) return tree[node];    // full overlap
        int mid = (lo + hi) / 2;                      // partial overlap
        return query(2*node, lo, mid, l, r)
             + query(2*node+1, mid+1, hi, l, r);
    }
    void update(int node, int lo, int hi, int idx, int val) {
        if (lo == hi) { tree[node] = val; return; }
        int mid = (lo + hi) / 2;
        if (idx <= mid) update(2*node, lo, mid, idx, val);
        else            update(2*node+1, mid+1, hi, idx, val);
        tree[node] = tree[2*node] + tree[2*node+1];
    }
public:
    explicit SegmentTree(const std::vector<int>& a)
        : n(a.size()), tree(4 * a.size()) { if (n) build(a, 1, 0, n-1); }
    long long query(int l, int r) { return query(1, 0, n-1, l, r); }
    void update(int idx, int val) { update(1, 0, n-1, idx, val); }
};
```

**Fenwick Tree (BIT)** — a lighter alternative for prefix sums: less memory, simpler, O(log n), but only for *invertible* operations (sum). Uses the lowbit trick `i & -i`.

```cpp
struct Fenwick {
    std::vector<long long> t;
    explicit Fenwick(int n) : t(n + 1, 0) {}
    void add(int i, long long delta) {            // 1-indexed
        for (; i < (int)t.size(); i += i & -i) t[i] += delta;
    }
    long long prefix(int i) {                     // sum [1..i]
        long long s = 0;
        for (; i > 0; i -= i & -i) s += t[i];
        return s;
    }
    long long range(int l, int r) { return prefix(r) - prefix(l - 1); }
};
```

### D–F.
- ✅ O(log n) dynamic range query + update; very flexible (any associative operation); lazy propagation enables O(log n) **range** updates.
- ❌ More code/memory than prefix sums; overkill if no updates (use prefix sums then).
- **Use cases:** range sum/min/max with updates, competitive programming, interval problems, range GCD, counting inversions.
- ⚠️ Allocate **4n** for safety; ⚠️ identity value must match the operation (0 for sum, +∞ for min); ⚠️ range updates need **lazy propagation**; ⚠️ BIT only handles invertible ops (not min/max).

---

## 3.7 Disjoint Set Union (Union-Find)

### A. Conceptual Layer

**Definition.** A DSU maintains a collection of **disjoint sets** and supports two operations: **find** (which set is `x` in? → representative/root) and **union** (merge two sets). With optimizations, both are **near O(1)** (inverse Ackermann α(n) ≤ 4 for all practical n).

**Why it exists.** Many problems reduce to **"are these two things connected?"** and **"connect them."** Doing this with graphs/BFS each time is O(V+E) per query. DSU answers both in essentially constant time, making it the backbone of Kruskal's MST and connectivity problems.

**Real-world analogy.** Social groups merging: each person points to a "group leader." To check if two people are in the same group, follow the chain to the leader. To merge groups, point one leader at the other.

```
Initially each its own set:   0  1  2  3  4
union(0,1), union(2,3):       0   2   4
                              |   |
                              1   3
union(1,3): connects the two trees → one component {0,1,2,3}
```

**Two optimizations make it fast:**
1. **Path compression** (in `find`): point every node directly to the root as you traverse → flattens trees.
2. **Union by rank/size**: attach the smaller tree under the larger → keeps trees shallow.

Together → **α(n) ≈ O(1)** amortized per operation.

### C. C++ Implementation

```cpp
#include <vector>
#include <numeric>

class DSU {
    std::vector<int> parent, rank_;
public:
    explicit DSU(int n) : parent(n), rank_(n, 0) {
        std::iota(parent.begin(), parent.end(), 0);   // each is its own root
    }
    int find(int x) {                                  // path compression
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int a, int b) {                         // union by rank
        int ra = find(a), rb = find(b);
        if (ra == rb) return false;                    // already connected
        if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
        parent[rb] = ra;
        if (rank_[ra] == rank_[rb]) ++rank_[ra];
        return true;
    }
    bool connected(int a, int b) { return find(a) == find(b); }
};
```

**STL equivalent.** None — DSU is not in the STL. Implement it (it's tiny). This exact ~20-line class is interview/CP gold.

### D–F.
- ✅ Near-constant union/find; trivial to implement; tiny memory (two int arrays).
- ❌ **Only supports union, not split/un-union**; no listing of set members without extra bookkeeping.
- **Use cases:** Kruskal's MST, connected components, cycle detection in undirected graphs, network connectivity, image segmentation (connected pixels), account/email merging, percolation.
- ⚠️ Always apply **both** optimizations — without them it degrades to O(n). ⚠️ Path compression mutates parent inside `find` (so `find` isn't `const`). ⚠️ DSU can't undo a union (use a rollback DSU with a stack if you need it).

---

## Chapter Summary

| Structure | Search | Insert | Delete | Ordered? | Killer feature |
|---|---|---|---|---|---|
| BST (balanced) | O(log n) | O(log n) | O(log n) | ✅ | sorted + range queries |
| AVL | O(log n) | O(log n) | O(log n) | ✅ | guaranteed balance |
| Heap | O(n) | O(log n) | O(log n)* | partial | O(1) min/max |
| Hash Table | O(1) avg | O(1) avg | O(1) avg | ❌ | fastest lookup |
| Trie | O(L) | O(L) | O(L) | ✅ | prefix queries |
| Segment Tree | O(log n) | O(log n) | — | by range | dynamic range queries |
| DSU | α(n) | α(n) | — | ❌ | connectivity |

\* heap delete = extract-min/max only.

---

*Next →* [Chapter 4: Algorithms](04_Algorithms.md)
