# Data Structures — A Professional Knowledge Repository (C++ Edition)

> A folder-based, interview-ready reference covering **core to advanced** data structures with deep conceptual understanding, internal working, modern C++17 implementations, complexity analysis, tradeoffs, and problem-solving patterns.

> 📚 Companion to the narrative [DSA Study Book](../DSA_Book/README.md) and [DSA Patterns](../DSA_Patterns/README.md). This repository is the **reference encyclopedia** — one topic per file, structured identically so you can jump straight to what you need.

---

## How Every File Is Structured

Each file follows the same template so the whole repo reads consistently:

1. **Conceptual Understanding** — definition, *why it exists*, the problem it solves, real-world analogy.
2. **Internal Working** — step-by-step mechanics, memory layout, text-based visualizations.
3. **Operations** — insert / delete / search / traversal, step by step.
4. **Complexity** — time & space, best/worst cases.
5. **C++ Implementation** — clean C++14/17, STL equivalent, when to use STL vs custom.
6. **Tradeoffs** — performance vs memory, simplicity vs flexibility.
7. **Use Cases** — real-world & system-level.
8. **Pitfalls** — common mistakes & edge cases.
9. **Problem Patterns** — where it's used, recognition hints, common problem types.
10. **Example Problems** — Easy / Medium / Hard with brute force + optimized + explanation.

Markers used throughout: 💡 key insight · ⚠️ pitfall · 🎯 pattern signal.

---

## Repository Map

### [Arrays/](Arrays/basics.md)
- [basics.md](Arrays/basics.md) — fundamentals, memory layout, static vs dynamic
- [operations.md](Arrays/operations.md) — insert/delete/search/rotate/resize
- [prefix_sum.md](Arrays/prefix_sum.md) — prefix sums, difference arrays, 2D
- [sliding_window_patterns.md](Arrays/sliding_window_patterns.md) — fixed & dynamic windows
- [advanced_problems.md](Arrays/advanced_problems.md) — Kadane, Dutch flag, trapping rain water…

### [Linked-Lists/](Linked-Lists/singly_linked_list.md)
- [singly_linked_list.md](Linked-Lists/singly_linked_list.md)
- [doubly_linked_list.md](Linked-Lists/doubly_linked_list.md)
- [circular_linked_list.md](Linked-Lists/circular_linked_list.md)
- [fast_slow_pointer.md](Linked-Lists/fast_slow_pointer.md)
- [advanced_problems.md](Linked-Lists/advanced_problems.md)

### [Stacks/](Stacks/stack_basics.md)
- [stack_basics.md](Stacks/stack_basics.md)
- [stack_implementation.md](Stacks/stack_implementation.md)
- [monotonic_stack.md](Stacks/monotonic_stack.md)
- [applications.md](Stacks/applications.md)

### [Queues/](Queues/queue.md)
- [queue.md](Queues/queue.md)
- [deque.md](Queues/deque.md)
- [priority_queue.md](Queues/priority_queue.md)
- [monotonic_queue.md](Queues/monotonic_queue.md)

### [Trees/](Trees/binary_tree.md)
- [binary_tree.md](Trees/binary_tree.md)
- [traversals.md](Trees/traversals.md)
- [binary_search_tree.md](Trees/binary_search_tree.md)
- [balanced_trees.md](Trees/balanced_trees.md)
- [heap.md](Trees/heap.md)
- [segment_tree.md](Trees/segment_tree.md)
- [fenwick_tree.md](Trees/fenwick_tree.md)
- [advanced_tree_problems.md](Trees/advanced_tree_problems.md)

### [Graphs/](Graphs/graph_basics.md)
- [graph_basics.md](Graphs/graph_basics.md)
- [bfs.md](Graphs/bfs.md)
- [dfs.md](Graphs/dfs.md)
- [shortest_path.md](Graphs/shortest_path.md)
- [mst.md](Graphs/mst.md)
- [topological_sort.md](Graphs/topological_sort.md)
- [advanced_graph_algorithms.md](Graphs/advanced_graph_algorithms.md)

### [Hashing/](Hashing/hash_tables.md)
- [hash_tables.md](Hashing/hash_tables.md)
- [collision_handling.md](Hashing/collision_handling.md)
- [unordered_maps.md](Hashing/unordered_maps.md)
- [advanced_hashing.md](Hashing/advanced_hashing.md)

### [Tries/](Tries/trie.md)
- [trie.md](Tries/trie.md)
- [applications.md](Tries/applications.md)

### [Disjoint-Set/](Disjoint-Set/union_find.md)
- [union_find.md](Disjoint-Set/union_find.md)
- [optimizations.md](Disjoint-Set/optimizations.md)

### [Advanced/](Advanced/segment_tree.md)
- [segment_tree.md](Advanced/segment_tree.md) — lazy propagation, variants
- [fenwick_tree.md](Advanced/fenwick_tree.md) — 2D BIT, range-update
- [suffix_array.md](Advanced/suffix_array.md)
- [suffix_tree.md](Advanced/suffix_tree.md)
- [sparse_table.md](Advanced/sparse_table.md)
- [bit_manipulation.md](Advanced/bit_manipulation.md)
- [advanced_data_structures.md](Advanced/advanced_data_structures.md) — HLD, persistent DS, treap, more

---

## Suggested Reading Order

1. **Arrays → Linked-Lists → Stacks → Queues** (linear foundations)
2. **Hashing** (the universal optimizer)
3. **Trees → Heaps** (hierarchies & priority)
4. **Graphs** (relationships)
5. **Tries → Disjoint-Set** (specialized)
6. **Advanced** (segment/Fenwick/sparse/suffix, bit tricks)

---

## Conventions
- Code targets **C++17** (`g++ -std=c++17 -O2`).
- Complexity is worst-case unless noted; `α(n)` = inverse Ackermann ≈ O(1).
- "STL equivalent" shows the production-grade container you'd actually use.

*Start with* → [Arrays/basics.md](Arrays/basics.md)
