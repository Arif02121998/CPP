# File: Trees/traversals.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · traversals · [binary_search_tree](binary_search_tree.md) · [balanced_trees](balanced_trees.md) · [heap](heap.md) · [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [advanced_tree_problems](advanced_tree_problems.md)

---

## Conceptual Understanding

**Definition.** A **traversal** is a systematic visit of every node in a tree exactly once. The two families are **depth-first** (pre-, in-, post-order) and **breadth-first** (level-order).

**Why it exists.** Different tasks need different visit orders: in-order yields sorted output for a BST; pre-order serializes/clones a tree; post-order frees/aggregates children before parents; level-order processes by depth. Choosing the right traversal often *is* the algorithm.

**Real-world analogy.** Reading an org chart: you can go department-by-department deep (DFS) or rank-by-rank across the whole company (BFS).

---

## The Four Orders

Given:
```
        1
       / \
      2   3
     / \
    4   5
```

| Order | Rule | Result |
|---|---|---|
| **Pre-order** | Node → Left → Right | 1 2 4 5 3 |
| **In-order** | Left → Node → Right | 4 2 5 1 3 |
| **Post-order** | Left → Right → Node | 4 5 2 3 1 |
| **Level-order** | top→bottom, left→right | 1 2 3 4 5 |

💡 **When to use which:**
- **In-order on a BST → sorted order** (the single most useful fact about BSTs).
- **Pre-order** → copy/serialize (root first lets you rebuild top-down).
- **Post-order** → delete a tree, compute subtree aggregates (children before parent).
- **Level-order** → shortest path in an unweighted tree, "by depth" problems.

---

## Internal Working

DFS uses a **stack** (implicit via recursion or explicit). BFS uses a **queue**. The visit position of the *node* relative to its children defines pre/in/post.

```
DFS (recursion) → call stack remembers where to resume
BFS (queue)     → FIFO expands one full level before the next
```

---

## C++ Implementation

### Recursive DFS
```cpp
#include <vector>
struct TreeNode { int val; TreeNode *left, *right; };

void preorder(TreeNode* r, std::vector<int>& out) {
    if (!r) return;
    out.push_back(r->val);           // visit node
    preorder(r->left, out);
    preorder(r->right, out);
}
void inorder(TreeNode* r, std::vector<int>& out) {
    if (!r) return;
    inorder(r->left, out);
    out.push_back(r->val);           // visit between children
    inorder(r->right, out);
}
void postorder(TreeNode* r, std::vector<int>& out) {
    if (!r) return;
    postorder(r->left, out);
    postorder(r->right, out);
    out.push_back(r->val);           // visit after children
}
```

### Iterative in-order (explicit stack)
```cpp
#include <stack>
std::vector<int> inorderIter(TreeNode* root) {
    std::vector<int> out;
    std::stack<TreeNode*> st;
    TreeNode* cur = root;
    while (cur || !st.empty()) {
        while (cur) { st.push(cur); cur = cur->left; }   // go far left
        cur = st.top(); st.pop();
        out.push_back(cur->val);                          // visit
        cur = cur->right;                                 // then right subtree
    }
    return out;
}
```

### BFS level-order (queue)
```cpp
#include <queue>
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> res;
    if (!root) return res;
    std::queue<TreeNode*> q; q.push(root);
    while (!q.empty()) {
        int sz = q.size();                 // snapshot this level's size
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
💡 Capturing `sz = q.size()` before the loop is the key trick to process exactly one level at a time.

### Morris in-order — O(1) space
Threads temporary links to a node's in-order predecessor, avoiding the stack.
```cpp
std::vector<int> morrisInorder(TreeNode* root) {
    std::vector<int> out;
    TreeNode* cur = root;
    while (cur) {
        if (!cur->left) { out.push_back(cur->val); cur = cur->right; }
        else {
            TreeNode* pred = cur->left;
            while (pred->right && pred->right != cur) pred = pred->right;
            if (!pred->right) { pred->right = cur; cur = cur->left; }     // create thread
            else { pred->right = nullptr; out.push_back(cur->val); cur = cur->right; } // remove
        }
    }
    return out;
}
```

---

## Complexity

| Traversal | Time | Space |
|---|---|---|
| Recursive DFS | O(n) | O(h) recursion (h = height) |
| Iterative DFS | O(n) | O(h) stack |
| BFS level-order | O(n) | O(w) (w = max width, up to n/2) |
| Morris | O(n) | **O(1)** |

---

## Tradeoffs
- **Recursion vs iteration:** recursion is concise but risks stack overflow on skewed trees; iterative gives control and bounded stack.
- **DFS vs BFS:** DFS uses O(h) memory (good for deep narrow trees); BFS uses O(w) (can blow up on wide trees) but finds shallowest results first.
- **Morris:** O(1) space but temporarily mutates pointers (not thread-safe; tricky to get right).

---

## Use Cases
- In-order: validate/print BST in sorted order, k-th smallest.
- Pre-order: serialize, clone, prefix expression.
- Post-order: delete tree, evaluate expression tree, subtree DP (heights, sums).
- Level-order: shortest path in tree, right-side view, zigzag, connect level pointers.

---

## Pitfalls
- ⚠️ Forgetting the null check → crash.
- ⚠️ Recursion depth on skewed trees → stack overflow (prefer iterative/Morris).
- ⚠️ BFS without snapshotting `q.size()` mixes levels together.
- ⚠️ Morris: forgetting to **remove** the temporary thread corrupts the tree.
- ⚠️ Assuming in-order gives sorted order for a *non-BST* (only true for BSTs).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Sorted output / k-th smallest in BST" → **in-order**.
- "Serialize / deserialize / clone" → **pre-order**.
- "Aggregate over subtrees / delete" → **post-order**.
- "Level / depth / width / right view / zigzag" → **BFS by levels**.
- "O(1) extra space traversal" → **Morris**.

---

## Example Problems

### Easy — Binary Tree Level Order Traversal
See `levelOrder` above. O(n).

### Medium — Binary Tree Zigzag Level Order
Same BFS, but reverse alternate levels (or push to a deque from alternating ends).
```cpp
// inside the level loop, if (depth % 2) std::reverse(level.begin(), level.end());
```
**Explanation.** Standard level-order plus a parity flip per level.

### Hard — Serialize and Deserialize Binary Tree
Pre-order with explicit null markers makes reconstruction unambiguous.
```cpp
#include <string>
#include <sstream>
void ser(TreeNode* r, std::string& s) {
    if (!r) { s += "# "; return; }
    s += std::to_string(r->val) + " ";
    ser(r->left, s); ser(r->right, s);
}
TreeNode* deser(std::istringstream& in) {
    std::string t; in >> t;
    if (t == "#") return nullptr;
    TreeNode* n = new TreeNode{std::stoi(t), nullptr, nullptr};
    n->left  = deser(in);
    n->right = deser(in);
    return n;
}
```
**Explanation.** Pre-order records the root first; `#` markers encode missing children so the same recursion rebuilds the exact shape. O(n).

---

*Next →* [binary_search_tree.md](binary_search_tree.md)
