# File: Trees/balanced_trees.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · [traversals](traversals.md) · [binary_search_tree](binary_search_tree.md) · balanced_trees · [heap](heap.md) · [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [advanced_tree_problems](advanced_tree_problems.md)

---

## Conceptual Understanding

**Definition.** A **self-balancing BST** automatically restructures itself on insert/delete so its height stays **O(log n)**, guaranteeing worst-case logarithmic search/insert/delete. The main variants are **AVL trees**, **Red-Black trees**, **B-trees/B+-trees**, and probabilistic structures like **Treaps** and **Skip Lists**.

**Why it exists.** A plain BST degrades to O(n) on sorted/adversarial input ([binary_search_tree.md](binary_search_tree.md)). Balancing removes that worst case so performance is predictable regardless of insertion order.

**Problem it solves.** Guaranteed O(log n) ordered-set/map operations — the backbone of `std::map/std::set`, database indexes, and filesystem structures.

**Real-world analogy.** A well-managed bookshelf: as you add books, you periodically redistribute them so no single shelf becomes overloaded and finding a book stays quick.

---

## How Balancing Works

Both AVL and Red-Black trees use **rotations** — O(1) local restructurings that change height while preserving the BST ordering.

```
Left rotation around x:

    x                 y
     \               / \
      y     →       x   T3
     / \             \
   T2   T3            T2
```
A **right rotation** is the mirror. Insert/delete may trigger a constant number of rotations to restore the invariant.

### AVL trees
- **Invariant:** for every node, |height(left) − height(right)| ≤ 1 (balance factor ∈ {−1, 0, +1}).
- **Strictly balanced** → height ≤ ~1.44·log₂n → **faster lookups**.
- More rotations on insert/delete (stricter to maintain) → **slower updates**.

### Red-Black trees
- **Invariants:** nodes are red/black; root & leaves (nil) black; no red node has a red child; every root→leaf path has the same number of black nodes.
- These imply height ≤ 2·log₂(n+1) → **looser balance**, fewer rotations on update.
- The standard choice for general-purpose libraries (`std::map/std::set`, Java `TreeMap`).

### B-trees / B+-trees
- **Multi-way** balanced trees (each node holds many keys and children) tuned for **block/disk** access — minimize I/O by matching node size to a disk page.
- Used by databases and filesystems (NTFS, ext4, most SQL indexes).

### Treap & Skip List
- **Treap:** BST by key + heap by random priority → expected O(log n), very simple to implement.
- **Skip List:** layered linked lists giving expected O(log n); easy to make concurrent.

---

## Comparison

| Structure | Height bound | Lookup | Insert/Delete | Notes |
|---|---|---|---|---|
| AVL | ~1.44 log n | fastest | more rotations | read-heavy workloads |
| Red-Black | 2 log n | fast | fewer rotations | general-purpose (STL) |
| B-tree | log_b n | fast | fast | disk/DB, high fan-out |
| Treap | exp. log n | exp. fast | exp. fast | simple, randomized |
| Skip List | exp. log n | exp. fast | exp. fast | easy concurrency |

💡 **Rule of thumb:** AVL when lookups dominate; Red-Black when updates are frequent; B-tree for disk; Treap/Skip List when you want simplicity or concurrency.

---

## Complexity (all guaranteed/expected)

| Operation | Time | Space |
|---|---|---|
| search / insert / delete | O(log n) | — |
| min / max / succ / pred | O(log n) | — |
| in-order iteration | O(n) | — |
| Overall | | O(n) |

---

## C++ Implementation

In practice you use the standard library — these *are* balanced (red-black) trees:
```cpp
#include <map>
#include <set>

std::map<int,std::string> m;       // ordered key→value, O(log n) ops
m[3] = "three"; m[1] = "one";
auto it = m.lower_bound(2);         // first key ≥ 2 → range/successor queries
for (auto& [k,v] : m) { /* visits keys ascending */ }

std::set<int> s{5,2,9};
s.insert(7);
bool present = s.count(2);
auto lo = s.lower_bound(3);         // floor/ceiling navigation
```

**AVL insertion sketch (for understanding rotations):**
```cpp
struct Node { int key, height=1; Node *l=nullptr,*r=nullptr; Node(int k):key(k){} };
int h(Node* n){ return n? n->height : 0; }
int bf(Node* n){ return n? h(n->l)-h(n->r) : 0; }
void upd(Node* n){ n->height = 1 + std::max(h(n->l), h(n->r)); }

Node* rotR(Node* y){ Node* x=y->l; y->l=x->r; x->r=y; upd(y); upd(x); return x; }
Node* rotL(Node* x){ Node* y=x->r; x->r=y->l; y->l=x; upd(x); upd(y); return y; }

Node* insert(Node* n, int key){
    if(!n) return new Node(key);
    if(key < n->key) n->l = insert(n->l, key);
    else if(key > n->key) n->r = insert(n->r, key);
    else return n;
    upd(n);
    int b = bf(n);
    if(b > 1 && key < n->l->key) return rotR(n);              // LL
    if(b < -1 && key > n->r->key) return rotL(n);             // RR
    if(b > 1 && key > n->l->key){ n->l = rotL(n->l); return rotR(n); }  // LR
    if(b < -1 && key < n->r->key){ n->r = rotR(n->r); return rotL(n); } // RL
    return n;
}
```

**When to use STL vs custom.** Use `std::map/std::set` (or `std::unordered_map` if you don't need order). Implement a balanced tree yourself only for interviews/education, or for an **augmented order-statistics tree** (subtree sizes for k-th element / rank) — though GNU's `__gnu_pbds::tree` provides that ready-made.

---

## Tradeoffs
- ✅ Worst-case O(log n) — immune to adversarial input order.
- ✅ Ordered operations (range, successor, k-th) that hash tables can't do.
- ❌ More complex than plain BSTs; rotation logic is error-prone to hand-code.
- ❌ Pointer-based → worse cache behavior than arrays/B-trees.
- **AVL vs RB:** AVL faster reads, RB faster writes.
- **vs hash table:** balanced tree gives order & range queries; hash table gives faster average point lookups but no ordering.

---

## Use Cases
- `std::map/std::set`, Java `TreeMap/TreeSet` (Red-Black).
- Database & filesystem indexes (B/B+-trees).
- Order-statistics (k-th smallest, rank) via augmented trees.
- Interval trees, sweep-line/event ordering.
- Memory allocators, schedulers needing ordered keys.

---

## Pitfalls
- ⚠️ Hand-coding rotations is bug-prone — height/balance-factor updates and the four rotation cases (LL/RR/LR/RL) must be exact.
- ⚠️ Forgetting to update heights/colors after a rotation breaks the invariant.
- ⚠️ Deletion rebalancing is significantly harder than insertion — easy to leave the tree unbalanced.
- ⚠️ Choosing a balanced tree when a hash table suffices adds needless O(log n) overhead.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Need worst-case O(log n) ordered operations" → **balanced BST / `std::map`**.
- "k-th smallest / rank / count less-than with updates" → **order-statistics tree** (or Fenwick — [fenwick_tree.md](fenwick_tree.md)).
- "Range query + ordered iteration" → **balanced BST**.
- "On-disk / huge dataset index" → **B-tree**.
- "Insert and query floor/ceiling repeatedly" → **`std::set::lower_bound`**.

---

## Example Problems

### Easy — Use `std::set` for a running sorted collection
Insert numbers; query floor/ceiling with `lower_bound`/`upper_bound` in O(log n).

### Medium — Contains Duplicate III
"Are there i, j with |i−j| ≤ k and |nums[i]−nums[j]| ≤ t?" Maintain a sliding window of size k in a `std::set` and use `lower_bound` to find a near neighbor.
```cpp
#include <set>
bool containsNearbyAlmostDuplicate(std::vector<int>& nums, int k, int t) {
    std::set<long long> win;
    for (int i = 0; i < (int)nums.size(); ++i) {
        auto it = win.lower_bound((long long)nums[i] - t);   // smallest ≥ nums[i]-t
        if (it != win.end() && *it <= (long long)nums[i] + t) return true;
        win.insert(nums[i]);
        if (i >= k) win.erase(nums[i - k]);                  // keep window size ≤ k
    }
    return false;
}
```
**Explanation.** The balanced tree finds the closest candidate to `nums[i]` in O(log k); the window bounds the index distance. O(n log k).

### Hard — Count of Smaller Numbers After Self (order statistics)
For each element, count later elements smaller than it. Solvable with an **order-statistics structure** (augmented BST / Fenwick tree / `__gnu_pbds::tree`) in O(n log n). See [fenwick_tree.md](fenwick_tree.md) for the BIT approach.

---

*Next →* [heap.md](heap.md)
