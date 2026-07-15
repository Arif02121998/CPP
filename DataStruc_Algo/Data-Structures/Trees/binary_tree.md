# File: Trees/binary_tree.md

[← Repository Index](../README.md) · **Trees:** binary_tree · [traversals](traversals.md) · [binary_search_tree](binary_search_tree.md) · [balanced_trees](balanced_trees.md) · [heap](heap.md) · [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [advanced_tree_problems](advanced_tree_problems.md)

---

## Conceptual Understanding

**Definition.** A **binary tree** is a hierarchical structure of nodes where each node has at most **two children** (left and right). One node is the **root**; nodes with no children are **leaves**.

**Why it exists.** Trees model hierarchy and enable **logarithmic** divide-and-conquer when balanced — searching, sorting, range queries, parsing, decision-making. The binary branching factor is the simplest case and the foundation for BSTs, heaps, segment trees, and tries.

**Problem it solves.** Representing nested/hierarchical relationships (file systems, org charts, expression trees) and enabling O(log n) operations when balanced.

**Real-world analogy.** A family tree or an org chart: each person reports to one parent and may have descendants below.

---

## Key Terminology & Properties

- **Depth** of a node: edges from the root to it. **Height** of a node: edges to its deepest leaf. **Height of tree** = height of root.
- **Level**: all nodes at the same depth.
- **Full** binary tree: every node has 0 or 2 children.
- **Complete** binary tree: all levels full except possibly the last, filled left-to-right (heaps are complete).
- **Perfect** binary tree: all internal nodes have 2 children and all leaves are at the same level → exactly $2^{h+1}-1$ nodes.
- **Balanced**: height is O(log n) (e.g., AVL, Red-Black).
- **Degenerate/skewed**: each node has one child → behaves like a linked list (height n).

💡 A binary tree with `n` nodes has height between $\lfloor \log_2 n \rfloor$ (balanced) and $n-1$ (skewed). This gap is *why* balancing matters.

---

## Internal Working

### Linked representation (general binary trees)
Each node stores a value and two child pointers.
```
        1
       / \
      2   3
     / \   \
    4   5   6
```

### Array representation (complete trees / heaps)
Store level by level; for node at index `i` (0-based): left = `2i+1`, right = `2i+2`, parent = `(i-1)/2`. Compact, no pointers — ideal for **heaps** and **segment trees**.

---

## Operations

| Operation | Typical cost (balanced) | Skewed worst case |
|---|---|---|
| Traverse all nodes | O(n) | O(n) |
| Height computation | O(n) | O(n) |
| Insert/search (unordered tree) | O(n) | O(n) |
| Insert/search (BST, balanced) | O(log n) | O(n) |

(Search/insert efficiency depends on the *kind* of binary tree — see [binary_search_tree.md](binary_search_tree.md).)

---

## C++ Implementation

```cpp
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// Height (number of nodes on the longest root→leaf path)
int height(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(height(root->left), height(root->right));
}

// Count nodes
int countNodes(TreeNode* root) {
    if (!root) return 0;
    return 1 + countNodes(root->left) + countNodes(root->right);
}

// Check if height-balanced (every node's subtrees differ in height by ≤ 1)
// Returns height, or -1 if unbalanced — O(n) single pass.
int balancedHeight(TreeNode* root) {
    if (!root) return 0;
    int l = balancedHeight(root->left);  if (l == -1) return -1;
    int r = balancedHeight(root->right); if (r == -1) return -1;
    if (std::abs(l - r) > 1) return -1;
    return 1 + std::max(l, r);
}
bool isBalanced(TreeNode* root) { return balancedHeight(root) != -1; }
```

**STL note.** The standard library has **no generic tree** type. `std::map`/`std::set` are balanced BSTs (red-black) but expose only the *map/set* interface, not raw tree access. For custom trees you manage nodes yourself (raw or `std::unique_ptr` for ownership).

**When to use STL vs custom.** Use `std::map/std::set` when you need an ordered associative container. Build a custom node tree for problem-specific structures (expression trees, segment trees, interview tree problems).

---

## Tradeoffs
- ✅ Natural hierarchy; O(log n) operations when balanced; flexible shapes.
- ❌ Pointer chasing → poor cache locality (vs arrays); easy to leak/dangle without smart pointers.
- ❌ Without balancing, operations degrade to O(n).
- **Memory:** two pointers per node overhead; array form removes pointers but needs (near-)complete shape.

---

## Use Cases
- Hierarchical data: file systems, DOM/HTML, org charts, scene graphs.
- Search structures: BST, AVL, red-black, B-trees.
- Heaps (priority queues), segment/Fenwick trees (range queries), tries (strings).
- Expression/parse trees in compilers.
- Decision trees, Huffman coding.

---

## Pitfalls
- ⚠️ **Confusing height vs depth** (and 0- vs 1-based counting). State your convention.
- ⚠️ Null-pointer dereference — always check `if (!root)` first in recursion.
- ⚠️ Memory leaks; prefer `std::unique_ptr<TreeNode>` or a deleting destructor.
- ⚠️ Deep recursion on skewed trees → stack overflow; consider iterative traversal.
- ⚠️ Assuming balance — a "binary tree" isn't automatically balanced.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Hierarchy / parent-child / nested" → **tree**.
- "Compute height/depth/diameter/balanced?" → **post-order recursion** (compute children first).
- "Level by level / shortest path in tree" → **BFS by levels** (see [traversals.md](traversals.md)).
- "Path from root / root-to-leaf sums" → **DFS with running state**.
- "Lowest common ancestor" → **recursive DFS** (see [advanced_tree_problems.md](advanced_tree_problems.md)).

---

## Example Problems

### Easy — Maximum Depth of Binary Tree
```cpp
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(maxDepth(root->left), maxDepth(root->right));
}
```
**Explanation.** Depth = 1 + deeper child's depth; pure post-order recursion. O(n).

### Medium — Diameter of Binary Tree
Longest path (in edges) between any two nodes.
```cpp
int diameter(TreeNode* root, int& best) {
    if (!root) return 0;
    int l = diameter(root->left, best);
    int r = diameter(root->right, best);
    best = std::max(best, l + r);          // path through this node
    return 1 + std::max(l, r);             // height contributed upward
}
int diameterOfBinaryTree(TreeNode* root) {
    int best = 0; diameter(root, best); return best;
}
```
**Explanation.** At each node the longest through-path is `leftHeight + rightHeight`; track the global max while returning height. O(n).

### Hard — Binary Tree Maximum Path Sum
Path may start/end anywhere; node values can be negative.
```cpp
int maxGain(TreeNode* root, int& best) {
    if (!root) return 0;
    int l = std::max(0, maxGain(root->left, best));   // drop negative branches
    int r = std::max(0, maxGain(root->right, best));
    best = std::max(best, root->val + l + r);          // path through root
    return root->val + std::max(l, r);                 // best one-sided extension
}
int maxPathSum(TreeNode* root) {
    int best = INT_MIN; maxGain(root, best); return best;
}
```
**Explanation.** Each node returns the best single-branch gain (clamped at 0); the answer considers the two-branch path at every node. O(n).

---

*Next →* [traversals.md](traversals.md)
