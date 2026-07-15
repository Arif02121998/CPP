# Chapter 5 — Problem-Solving Techniques

[← Back to Table of Contents](README.md)

> The difference between a beginner and an expert is not how many algorithms they've memorized — it's how quickly they **map a new problem to a known pattern** and how systematically they **evolve a brute-force idea into an optimal one.**

---

## 5.1 A Universal Problem-Solving Framework

When you see any problem, run this loop:

1. **Understand & restate.** What are the inputs, outputs, and constraints? Restate in your own words. Ask: what does success look like?
2. **Examples & edge cases.** Work a small example by hand. List edge cases *now* (empty, single element, duplicates, negatives, overflow).
3. **Brute force first.** Find *any* correct solution, even O(2ⁿ). A correct slow answer beats a fast wrong one and reveals structure.
4. **Identify the bottleneck.** Where is the time going? Repeated work? Re-scanning? Recomputation?
5. **Map to a pattern.** Match the bottleneck to a known technique (see 5.2).
6. **Optimize.** Apply the pattern; trade space for time, precompute, or restructure.
7. **Verify.** Re-check edge cases, complexity, and correctness.

> 💡 **Constraints reveal the intended complexity.** This is the single most useful interview heuristic:

| Constraint on n | Intended complexity | Likely technique |
|---|---|---|
| n ≤ 10–12 | O(n!) / O(2ⁿ) | backtracking, bitmask, permutations |
| n ≤ 20–25 | O(2ⁿ) | subset enumeration, meet-in-the-middle |
| n ≤ 100–500 | O(n³) | DP, Floyd-Warshall |
| n ≤ 2,000–10⁴ | O(n²) | DP, nested loops |
| n ≤ 10⁵–10⁶ | O(n log n) / O(n) | sorting, sliding window, two pointers, heap |
| n ≤ 10⁷–10⁸ | O(n) / O(log n) | linear scan, math, binary search |
| n huge (10¹⁸) | O(log n) / O(1) | math, binary search on answer, fast exponentiation |

---

## 5.2 Pattern Recognition — The Core Skill

Most problems are disguised versions of ~15 patterns. Learn to spot the **signal words** and structure.

| If the problem says / has… | Reach for… |
|---|---|
| "sorted array", "find pair/triplet" | **Two pointers** / binary search |
| "contiguous subarray/substring", "window of size k" | **Sliding window** |
| "all combinations/permutations/subsets", "valid arrangements" | **Backtracking** |
| "number of ways", "min/max cost", "longest/shortest" with choices | **Dynamic programming** |
| "shortest path", "fewest steps", "levels" (unweighted) | **BFS** |
| "shortest path with weights" (non-negative) | **Dijkstra** |
| "connected", "groups", "merge sets", "cycle (undirected)" | **Union-Find** |
| "dependencies", "ordering", "prerequisites" | **Topological sort** |
| "top K", "K largest/smallest", "median of stream" | **Heap** |
| "prefix", "autocomplete", "dictionary words" | **Trie** |
| "range sum/min with updates" | **Segment tree / BIT** |
| "next greater/smaller element", "histogram" | **Monotonic stack** |
| "maximize value with capacity/weight" | **Knapsack DP / greedy** |
| "detect cycle in linked list", "find middle" | **Fast & slow pointers** |
| "overlapping intervals", "merge/schedule" | **Sort + greedy / sweep line** |
| "search in monotonic answer space" | **Binary search on the answer** |
| "seen before?", "frequency", "dedup" | **Hash map / hash set** |

**How to internalize patterns:** after solving a problem, *name the pattern* and add it to a personal list. After ~150 problems you'll recognize most patterns within seconds.

---

## 5.3 Decomposition — Breaking Problems into Subproblems

Hard problems are compositions of easy ones. Techniques:

- **Reduce to a known problem.** "Can I transform this into sorting / shortest path / two-sum?" Reduction is the most powerful move in algorithm design.
- **Solve a simpler version first.** Drop a constraint (assume sorted, assume no duplicates, assume k=1), solve that, then add complexity back.
- **Identify the recursive structure.** "What's the answer for size n in terms of size n−1?" → leads to recursion/DP.
- **Split into independent phases.** E.g., "preprocess (sort/build index), then answer queries."
- **Handle the general case via the base case.** Define base cases crisply; the recursion usually writes itself.

**Worked decomposition — "Trapping Rain Water":**
1. Insight: water above index `i` = `min(maxLeft[i], maxRight[i]) − height[i]`.
2. Subproblem A: for each `i`, the tallest bar to its left → prefix max.
3. Subproblem B: tallest bar to its right → suffix max.
4. Combine: sum the per-index water. O(n) time/space.
5. Optimize: two pointers replace the two arrays → O(1) space.

This is the pattern: **define a clean sub-quantity, compute it efficiently, combine.**

---

## 5.4 Brute Force → Optimized: The Evolution

Optimal solutions are *evolved*, not summoned. The typical ladder:

### Example 1: Two-Sum
- **Brute force:** check every pair → O(n²).
- **Bottleneck:** for each `x`, we *re-search* for `target − x`.
- **Optimize:** store seen values in a **hash set** → O(1) lookup → **O(n)**.
- **Lesson:** "repeated search" → hash map.

### Example 2: Fibonacci
- **Brute force:** naive recursion → O(2ⁿ) (recomputes subproblems).
- **Bottleneck:** overlapping subproblems.
- **Optimize:** **memoize** → O(n); then **tabulate** → O(n)/O(1) space.
- **Lesson:** "recomputed subproblems" → DP.

### Example 3: Maximum Subarray
- **Brute force:** all subarrays → O(n²) or O(n³).
- **Bottleneck:** recomputing sums; not reusing the best-ending-here.
- **Optimize:** **Kadane's DP** — `best_here = max(x, best_here + x)` → **O(n)**.
- **Lesson:** "best subarray ending at i" → running DP.

### Example 4: Sliding-window maximum
- **Brute force:** for each window, scan for max → O(nk).
- **Bottleneck:** rescanning overlapping windows.
- **Optimize:** **monotonic deque** keeps candidates in O(1) amortized → **O(n)**.
- **Lesson:** "max/min in a moving window" → monotonic deque.

> 💡 **General optimization strategies:**
> 1. **Eliminate repeated work** → caching/memoization/DP.
> 2. **Avoid re-scanning** → sliding window, two pointers, prefix sums.
> 3. **Replace linear search with O(1)/O(log n)** → hash map, sorting + binary search.
> 4. **Precompute** what you'll query repeatedly → prefix sums, sparse tables, segment trees.
> 5. **Exploit structure** → sorted order, monotonicity, bounded value range (counting sort).
> 6. **Trade space for time** (and sometimes the reverse).

---

## 5.5 Optimization Strategies Cheatsheet

| Symptom | Likely fix |
|---|---|
| Nested loops recomputing sums | Prefix sums / sliding window |
| Repeated lookups "have I seen X?" | Hash set/map |
| Repeated "search for value in sorted data" | Binary search |
| Same recursive calls repeated | Memoization → DP |
| Repeated range queries with updates | Segment tree / BIT |
| "Find min/max repeatedly" | Heap |
| "Connected / merge groups repeatedly" | Union-Find |
| Sorting unlocks two pointers/greedy | Sort first (O(n log n) is cheap) |
| Huge recursion depth | Convert to iteration + explicit stack |
| Exponential search space, small n | Backtracking with pruning / bitmask DP |

---

## 5.6 Mental Habits of Strong Problem Solvers

- **Always start with brute force** — it anchors correctness and reveals the bottleneck.
- **Think in invariants** — what stays true each iteration? (Great for binary search, two pointers, loops.)
- **Draw it.** Sketch the array, tree, or recursion tree. Visualization exposes structure.
- **Name the bottleneck before optimizing.** Don't optimize blindly.
- **Test on edge cases mentally** before declaring victory.
- **Know your constraints** — they tell you the target complexity.
- **Prefer the simplest solution that meets constraints** — clever ≠ better.

---

*Next →* [Chapter 6: Interview Preparation](06_Interview_Preparation.md)
