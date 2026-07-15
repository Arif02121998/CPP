# File: Trees/binary_search_tree.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · [traversals](traversals.md) · binary_search_tree · [balanced_trees](balanced_trees.md) · [heap](heap.md) · [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [advanced_tree_problems](advanced_tree_problems.md)

---

## Conceptual Understanding

**Definition.** A **binary search tree (BST)** is a binary tree with the **ordering invariant**: for every node, all keys in its **left** subtree are **less**, and all keys in its **right** subtree are **greater** (assuming distinct keys). This makes search a guided descent.

**Why it exists.** It combines the **fast search of a sorted array** (binary search) with the **fast insert/delete of a linked list**. When balanced, search/insert/delete are all O(log n), and in-order traversal yields sorted order for free.

**Problem it solves.** A dynamic ordered set/map — fast lookup *and* fast updates *and* ordered iteration / range queries / predecessor-successor.

**Real-world analogy.** A guess-the-number game: each comparison tells you to go left (smaller) or right (larger), halving the search space.

---

## Internal Working

The invariant turns search into "compare and descend":
```
        8
       / \
      3   10
     / \    \
    1   6    14
       / \   /
      4   7 13
```
Searching for 7: 7<8→left, 7>3→right, 7>6→right → found. Each step discards a subtree.

⚠️ The invariant is **global**, not just parent-vs-child: every node in the left subtree (not just the immediate left child) must be smaller. (Common validation bug.)

---

## Operations

| Operation | Balanced | Skewed (worst) |
|---|---|---|
| search | O(log n) | O(n) |
| insert | O(log n) | O(n) |
| delete | O(log n) | O(n) |
| min / max | O(log n) | O(n) |
| predecessor / successor | O(log n) | O(n) |
| in-order (sorted) | O(n) | O(n) |

💡 Inserting **sorted data** into a plain BST creates a skewed (linked-list) tree — the motivation for self-balancing trees ([balanced_trees.md](balanced_trees.md)).

### Deletion — three cases
1. **Leaf:** just remove it.
2. **One child:** replace the node with that child.
3. **Two children:** replace the node's value with its **in-order successor** (smallest in the right subtree), then delete that successor.

---

## C++ Implementation

```cpp
struct Node { int key; Node *left=nullptr, *right=nullptr; Node(int k):key(k){} };

Node* search(Node* root, int key) {
    while (root && root->key != key)
        root = (key < root->key) ? root->left : root->right;
    return root;                       // nullptr if absent
}

Node* insert(Node* root, int key) {
    if (!root) return new Node(key);
    if (key < root->key)      root->left  = insert(root->left, key);
    else if (key > root->key) root->right = insert(root->right, key);
    // equal → ignore (set semantics)
    return root;
}

Node* findMin(Node* n) { while (n->left) n = n->left; return n; }

Node* deleteNode(Node* root, int key) {
    if (!root) return nullptr;
    if (key < root->key)      root->left  = deleteNode(root->left, key);
    else if (key > root->key) root->right = deleteNode(root->right, key);
    else {                                            // found
        if (!root->left)  { Node* r = root->right; delete root; return r; }
        if (!root->right) { Node* l = root->left;  delete root; return l; }
        Node* succ = findMin(root->right);            // two children
        root->key = succ->key;
        root->right = deleteNode(root->right, succ->key);
    }
    return root;
}
```

**STL equivalent.** `std::set` / `std::map` (and `multiset`/`multimap`) are **balanced** BSTs (red-black trees) with guaranteed O(log n) and ordered iteration:
```cpp
#include <set>
std::set<int> s;            // ordered, unique
s.insert(8); s.insert(3);
auto it = s.lower_bound(5); // first key ≥ 5  → predecessor/successor queries
bool has = s.count(3);      // membership
```

**When to use STL vs custom.** Use `std::set/std::map` for ordered associative needs (they're balanced, you can't accidentally skew them). Hand-roll a BST only for learning, or when you must augment nodes (e.g., subtree sizes for order-statistics) — though even then an order-statistics tree or a Fenwick tree may be simpler.

💡 **BST vs hash table:** use a hash table (`unordered_map`) for pure membership/lookup (O(1) average, no order); use a BST when you need **sorted order, range queries, or predecessor/successor**.

---

## Tradeoffs
- ✅ O(log n) search/insert/delete *when balanced*; ordered iteration; range and rank queries.
- ❌ Plain BSTs degrade to O(n) on sorted/adversarial input → need balancing.
- ❌ Pointer-based → worse cache locality than arrays/hash tables.
- **vs sorted array:** BST has O(log n) insert/delete (array is O(n)); array has O(1) random access and better locality.

---

## Use Cases
- Ordered sets/maps (`std::set/std::map`).
- Range queries: "all keys in [lo, hi]", count/sum in a range.
- Predecessor/successor, floor/ceiling, k-th smallest (with subtree sizes).
- Database indexes (B-trees generalize BSTs for disk).
- Interval scheduling, sweep-line event ordering.

---

## Pitfalls
- ⚠️ **Validating with only parent-child checks** — must enforce the global range (use min/max bounds or in-order monotonicity).
- ⚠️ Inserting sorted input into a plain BST → O(n) skew. Use a balanced tree.
- ⚠️ Deletion of a two-child node: must use in-order successor/predecessor, not arbitrary replacement.
- ⚠️ Duplicate-key policy must be defined (ignore, count, or go right).
- ⚠️ Memory leaks on delete; recursion depth on skewed trees.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Validate BST" → **in-order must be strictly increasing**, or recurse with (min,max) bounds.
- "k-th smallest/largest" → **in-order with a counter** (or augmented subtree sizes).
- "Insert/delete/search in a sorted dynamic set" → **BST / `std::set`**.
- "Predecessor / successor / floor / ceiling" → **BST descent** or `lower_bound`.
- "Range count/sum of keys" → **augmented BST / Fenwick tree**.

---

## Example Problems

### Easy — Search in a BST
See `search` above — O(h).

### Medium — Validate Binary Search Tree
```cpp
#include <climits>
bool valid(TreeNode* n, long lo, long hi) {
    if (!n) return true;
    if (n->val <= lo || n->val >= hi) return false;
    return valid(n->left, lo, n->val) && valid(n->right, n->val, hi);
}
bool isValidBST(TreeNode* root) { return valid(root, LONG_MIN, LONG_MAX); }
```
**Explanation.** Carry an allowed open interval down; each node tightens the bound for its children. Catches the "deep descendant violates order" bug. O(n).

### Medium — K-th Smallest Element in a BST
```cpp
int kthSmallest(TreeNode* root, int k) {
    std::stack<TreeNode*> st; TreeNode* cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }
        cur = st.top(); st.pop();
        if (--k == 0) return cur->val;        // in-order visit count
        cur = cur->right;
    }
    return -1;
}
```
**Explanation.** In-order visits keys ascending; stop at the k-th. O(h + k).

### Hard — Recover BST (two nodes swapped)
In-order traversal detects the two out-of-order nodes; swap their values back.
```cpp
void recoverTree(TreeNode* root) {
    TreeNode *first=nullptr, *second=nullptr, *prev=nullptr;
    std::stack<TreeNode*> st; TreeNode* cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }
        cur = st.top(); st.pop();
        if (prev && prev->val > cur->val) {     // inversion
            if (!first) first = prev;
            second = cur;
        }
        prev = cur; cur = cur->right;
    }
    if (first && second) std::swap(first->val, second->val);
}
```
**Explanation.** A correct BST has strictly increasing in-order output; the two swapped nodes create one or two inversions, which pinpoint them. O(n), O(h).

---

*Next →* [balanced_trees.md](balanced_trees.md)
