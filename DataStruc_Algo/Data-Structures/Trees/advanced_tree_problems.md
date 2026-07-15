# File: Trees/advanced_tree_problems.md

[← Repository Index](../README.md) · **Trees:** [binary_tree](binary_tree.md) · [traversals](traversals.md) · [binary_search_tree](binary_search_tree.md) · [balanced_trees](balanced_trees.md) · [heap](heap.md) · [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · advanced_tree_problems

---

## Conceptual Understanding

**Definition.** This file collects the **high-frequency hard tree problems** and the reusable techniques behind them: post-order DP on trees, lowest common ancestor (LCA), tree construction from traversals, path/subtree aggregation, and serialization.

**Why a dedicated file.** Most "hard" tree problems reduce to a small set of ideas — **process children first (post-order), return a compact summary upward, and track a global answer**. Master these and the rest are variations.

**Real-world analogy.** Auditing a corporate hierarchy: each manager reports a rolled-up summary to their boss; a company-wide statistic emerges from these local rollups.

---

## Core Techniques

### 1. Post-order DP ("return one thing, update a global")
Compute something for each subtree by combining children's results; keep a global best.
Used by: diameter, max path sum, balanced check, house robber III, longest univalue path.

### 2. Range-bounds recursion
Pass down an allowed `(min, max)` interval to validate/build BSTs. Used by: validate BST, BST from preorder.

### 3. LCA (lowest common ancestor)
The deepest node that is an ancestor of both targets.

### 4. Construct from traversals
Pre/post-order fixes the root; in-order splits left/right subtrees.

---

## Problem 1 — Lowest Common Ancestor (Binary Tree)

**Optimized recursion:** O(n)/O(h).
```cpp
struct TreeNode { int val; TreeNode *left,*right; };
TreeNode* lca(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    TreeNode* L = lca(root->left,  p, q);
    TreeNode* R = lca(root->right, p, q);
    if (L && R) return root;          // p and q split here → this is the LCA
    return L ? L : R;                 // both on one side (or neither)
}
```
**Explanation.** If both targets appear in different subtrees of a node, that node is the LCA; otherwise the answer is wherever both were found.

**BST version** is simpler — descend while both keys are on the same side:
```cpp
TreeNode* lcaBST(TreeNode* r, TreeNode* p, TreeNode* q) {
    while (r) {
        if (p->val < r->val && q->val < r->val) r = r->left;
        else if (p->val > r->val && q->val > r->val) r = r->right;
        else return r;               // split point
    }
    return nullptr;
}
```

💡 For **many** LCA queries, preprocess with **binary lifting** (O(n log n) build, O(log n) per query) or Euler tour + sparse table (O(1) per query) — see [../Advanced/sparse_table.md](../Advanced/sparse_table.md).

---

## Problem 2 — Construct Tree from Traversals

Build from **preorder + inorder**: preorder[0] is the root; its index in inorder splits left/right.
```cpp
#include <vector>
#include <unordered_map>
TreeNode* build(std::vector<int>& pre, std::vector<int>& in) {
    std::unordered_map<int,int> pos;               // value → index in inorder
    for (int i = 0; i < (int)in.size(); ++i) pos[in[i]] = i;
    int pi = 0;
    std::function<TreeNode*(int,int)> go = [&](int l, int r) -> TreeNode* {
        if (l > r) return nullptr;
        int rootVal = pre[pi++];
        TreeNode* node = new TreeNode{rootVal, nullptr, nullptr};
        int m = pos[rootVal];
        node->left  = go(l, m - 1);                // build left before right (preorder)
        node->right = go(m + 1, r);
        return node;
    };
    return go(0, in.size() - 1);
}
```
**Explanation.** Preorder gives roots in order; inorder locates each root to partition remaining nodes. O(n).

---

## Problem 3 — Path Sum Problems

### Path Sum III — count root-agnostic downward paths summing to target
Prefix-sum along the current root path + hashmap → O(n).
```cpp
#include <unordered_map>
int dfs(TreeNode* node, long long cur, int target,
        std::unordered_map<long long,int>& seen) {
    if (!node) return 0;
    cur += node->val;
    int cnt = seen.count(cur - target) ? seen[cur - target] : 0;
    ++seen[cur];
    cnt += dfs(node->left, cur, target, seen)
         + dfs(node->right, cur, target, seen);
    --seen[cur];                       // backtrack: leave this path
    return cnt;
}
int pathSum(TreeNode* root, int target) {
    std::unordered_map<long long,int> seen{{0,1}};
    return dfs(root, 0, target, seen);
}
```
**Explanation.** The same prefix-sum trick as subarray-sum-equals-K, applied along each root-to-node path; backtracking removes a node's contribution when leaving it.

---

## Problem 4 — Serialize / Deserialize
Covered in [traversals.md](traversals.md) (pre-order with null markers). Essential for storing/transmitting trees. O(n).

---

## Problem 5 — House Robber III (tree DP)
Can't rob two directly-linked nodes; maximize loot. Post-order returns a pair {rob-this, skip-this}.
```cpp
#include <algorithm>
std::pair<int,int> rob(TreeNode* n) {                 // {withNode, withoutNode}
    if (!n) return {0, 0};
    auto L = rob(n->left);
    auto R = rob(n->right);
    int withNode    = n->val + L.second + R.second;   // must skip children
    int withoutNode = std::max(L.first, L.second) + std::max(R.first, R.second);
    return {withNode, withoutNode};
}
int rob(TreeNode* root) { auto r = rob(root); return std::max(r.first, r.second); }
```
**Explanation.** Classic tree DP: each node reports the best if it's robbed vs not, so the parent can decide. O(n).

---

## Complexity Summary

| Problem | Time | Space |
|---|---|---|
| LCA (binary tree) | O(n) | O(h) |
| LCA (BST) | O(h) | O(1) |
| Build from pre+in | O(n) | O(n) |
| Path Sum III | O(n) | O(h) |
| House Robber III | O(n) | O(h) |
| Serialize/deserialize | O(n) | O(n) |

---

## Tradeoffs
- ✅ Post-order tree DP solves a huge class of problems in one O(n) pass.
- ❌ Recursion depth is the tree height — skewed trees risk stack overflow (consider iterative or explicit stack).
- **Single query vs many:** simple recursion for one LCA; binary lifting/Euler+sparse-table for many.

---

## Use Cases
- Compilers (expression trees), file systems, XML/JSON hierarchies.
- Genealogy/ontology LCA queries; network routing common-ancestor.
- Auto-complete and prefix structures (see [../Tries/trie.md](../Tries/trie.md)).
- Any hierarchical aggregation/decision problem.

---

## Pitfalls
- ⚠️ Returning the wrong summary upward (mixing "path through node" with "extension upward" — as in max path sum / diameter).
- ⚠️ Forgetting to **backtrack** shared state (prefix-sum map, path vector).
- ⚠️ Assuming a BST when it's a general binary tree (LCA methods differ).
- ⚠️ Stack overflow on skewed trees.
- ⚠️ Duplicate values breaking traversal-based construction (need unique keys or indices).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Best path/subtree value, negatives allowed" → **post-order DP + global max**.
- "Common ancestor / distance between nodes" → **LCA**.
- "Rebuild the tree from orders" → **preorder/postorder root + inorder split**.
- "Count paths with a sum" → **prefix sums along root path**.
- "Can't pick adjacent nodes" → **tree DP with {take, skip} pairs**.

---

## Example Problems Recap
- **Easy:** LCA of a BST; invert binary tree; same tree / symmetric tree.
- **Medium:** construct from pre+in; path sum III; count good nodes; right-side view.
- **Hard:** binary tree max path sum ([binary_tree.md](binary_tree.md)); serialize/deserialize; recover BST ([binary_search_tree.md](binary_search_tree.md)); LCA with binary lifting.

---

*Next →* [../Graphs/graph_basics.md](../Graphs/graph_basics.md)
