# Mastering DSA Problem-Solving Patterns (C++ Edition)

> A pattern-first companion book. The goal is not to memorize problems — it is to **recognize the underlying pattern** in any problem and know **exactly which technique to apply and why**.

> 📘 Companion to the main [DSA Study Book](../DSA_Book/README.md). That book teaches the *structures and algorithms*; this book teaches the *thinking* — how to map an unseen problem to a known pattern in seconds.

---

## Why Patterns?

There are millions of coding problems but only ~25 recurring **patterns**. Strong problem solvers don't memorize solutions; they:

1. **Read the signals** in a problem (constraints, keywords, structure).
2. **Match** those signals to a pattern.
3. **Adapt** the pattern's template to the specifics.

This book trains that pipeline: *Signal → Pattern → Template → Adapt → Verify.*

---

## How to Use This Book

- **First pass:** read Foundations, then skim each pattern's "When to use" box.
- **Practice pass:** for each pattern, implement the template from memory, then solve 5–8 problems.
- **Revision pass:** use the [Pattern Recognition](04_Pattern_Recognition.md) decision tables and the [Interview](05_Interview_Preparation.md) drills.
- Every pattern follows the same anatomy (below) so you can compare them directly.

### Pattern Anatomy (used for every pattern)
1. **Intuition** — the core idea and *why* it works.
2. **When to use (signals)** — keywords, constraints, structure that trigger it.
3. **Template** — the reusable C++ skeleton.
4. **Worked example** — a representative problem, dry-run + code.
5. **Complexity** — time/space.
6. **Variations** — how the pattern bends to related problems.
7. **Pitfalls** — the mistakes that cost points.

---

## Table of Contents

### Part I — [Foundations of Problem Solving](01_Foundations_of_Problem_Solving.md)
1. What a "pattern" really is
2. The universal problem-solving framework
3. Reading constraints → guessing complexity
4. Brute force → optimized evolution
5. The mental toolkit (invariants, state, transitions)
6. How to practice patterns effectively

### Part II — [Core Patterns](02_Core_Patterns.md)
1. Two Pointers
2. Sliding Window (fixed & dynamic)
3. Fast & Slow Pointers (cycle detection)
4. Prefix Sum / Difference Array
5. Binary Search (on array & on answer)
6. Hashing & Frequency Counting
7. Stack & Monotonic Stack
8. In-place Linked List Reversal
9. Merge Intervals
10. Cyclic Sort

### Part III — [Advanced Patterns](03_Advanced_Patterns.md)
1. Tree BFS (level-order)
2. Tree DFS (recursive choices)
3. Graph Traversal & Connectivity
4. Topological Sort (Kahn / DFS)
5. Backtracking
6. Subsets / Combinations / Permutations
7. Top-K Elements (Heap)
8. K-way Merge
9. Two Heaps
10. Union-Find (DSU)
11. Trie (prefix)
12. Dynamic Programming patterns
13. Greedy patterns
14. Bit Manipulation
15. Monotonic Deque (sliding-window extremum)

### Part IV — [Pattern Recognition Techniques](04_Pattern_Recognition.md)
1. Signal → Pattern lookup tables
2. Constraint → complexity → pattern
3. Decision flowcharts
4. Disambiguating similar patterns
5. Common transformations & reductions

### Part V — [Interview Preparation](05_Interview_Preparation.md)
1. The interview problem-solving loop
2. Pattern-grouped problem sets (easy → hard)
3. Speaking your reasoning
4. Edge cases & traps checklist
5. A study schedule

---

## Conventions
- Code targets **C++17** (`g++ -std=c++17 -O2`).
- 💡 = key insight · ⚠️ = common trap · 🎯 = signal to recognize the pattern.

---

*Start here →* [Part I: Foundations of Problem Solving](01_Foundations_of_Problem_Solving.md)
