# Part V — Interview Preparation

[← Back to Table of Contents](README.md)

> Putting it together: a repeatable interview loop, pattern-grouped problem sets, how to talk while you solve, the traps that lose points, and a study schedule.

---

## 5.1 The Interview Problem-Solving Loop

Say these steps out loud — communication is scored as heavily as the solution.

1. **Clarify (30–60s).** Restate the problem. Ask: input ranges? duplicates? sorted? negative values? in-place required? expected output for edge cases?
2. **Examples.** Walk one small example by hand. Add one edge case (empty / single / all-equal).
3. **Brute force.** State *any* correct approach and its complexity. Shows correctness instinct and a baseline.
4. **Identify the bottleneck & name the pattern.** "We're re-searching for the complement each time → that's a hashing pattern."
5. **Explain the optimized idea before coding.** Get a nod from the interviewer.
6. **Code cleanly.** Meaningful names, handle edges first, narrate as you type.
7. **Test.** Dry-run on the example and an edge case. Fix bugs calmly.
8. **State complexity.** Time and space, unprompted. Mention possible further optimizations.

> 💡 A clearly-explained O(n²) often outscores a silent, buggy O(n). Think *and talk*.

---

## 5.2 Pattern-Grouped Problem Sets

Work these in order *within* each pattern (easy → hard). Implement, then re-derive days later. (Names map to standard interview problems; solve on any judge.)

### Two Pointers
- Valid Palindrome · Two Sum II (sorted) · Remove Duplicates from Sorted Array
- 3Sum · Container With Most Water · Sort Colors (Dutch flag)
- Trapping Rain Water *(hard)*

### Sliding Window
- Maximum Average Subarray (fixed k) · Best Time to Buy/Sell Stock
- Longest Substring Without Repeating Characters · Longest Repeating Character Replacement
- Permutation in String · Minimum Size Subarray Sum
- Minimum Window Substring *(hard)* · Sliding Window Maximum *(deque)*

### Fast & Slow Pointers
- Middle of Linked List · Linked List Cycle · Happy Number
- Find the Duplicate Number · Palindrome Linked List · Cycle II (find start)

### Prefix Sum / Hashing
- Running Sum · Subarray Sum Equals K · Contiguous Array (equal 0s/1s)
- Two Sum · Group Anagrams · Longest Consecutive Sequence · Top-K Frequent

### Binary Search
- Binary Search · First/Last Position · Search in Rotated Sorted Array
- Find Peak Element · Koko Eating Bananas · Split Array Largest Sum *(answer search)*
- Median of Two Sorted Arrays *(hard)*

### Stack / Monotonic Stack
- Valid Parentheses · Min Stack · Evaluate RPN · Daily Temperatures
- Next Greater Element I/II · Largest Rectangle in Histogram *(hard)*

### Linked List
- Reverse Linked List · Merge Two Sorted Lists · Remove Nth From End
- Reorder List · Reverse Nodes in k-Group *(hard)*

### Intervals
- Merge Intervals · Insert Interval · Non-overlapping Intervals
- Meeting Rooms I/II · Employee Free Time *(hard)*

### Trees (BFS/DFS)
- Max Depth · Same Tree · Invert Tree · Level Order · Right Side View
- Validate BST · LCA · Diameter · Path Sum II · Kth Smallest in BST
- Serialize/Deserialize · Binary Tree Max Path Sum *(hard)*

### Graphs
- Number of Islands · Clone Graph · Rotting Oranges · Course Schedule I/II
- Pacific Atlantic · Redundant Connection *(DSU)* · Word Ladder *(hard)*
- Network Delay Time *(Dijkstra)* · Min Cost to Connect Points *(MST)*

### Heap / Top-K / Two Heaps
- Kth Largest Element · Top-K Frequent · K Closest Points
- Task Scheduler · Merge K Sorted Lists *(hard)* · Find Median from Data Stream *(hard)*

### Backtracking
- Subsets I/II · Permutations I/II · Combination Sum I/II
- Generate Parentheses · Word Search · Palindrome Partitioning
- N-Queens *(hard)* · Sudoku Solver *(hard)*

### Dynamic Programming
- Climbing Stairs · House Robber I/II · Coin Change · Longest Increasing Subsequence
- Unique Paths · Min Path Sum · Word Break · Partition Equal Subset Sum
- Longest Common Subsequence · Edit Distance *(hard)* · Burst Balloons *(hard)*

### Bit Manipulation
- Single Number I/II/III · Number of 1 Bits · Counting Bits · Missing Number · Sum of Two Integers

---

## 5.3 Speaking Your Reasoning (Scripts)

Phrases that signal structured thinking:

- **Clarifying:** *"Can the array contain negatives? Are there duplicates? Should I modify it in place?"*
- **Pattern naming:** *"This asks for the longest contiguous substring under a constraint — that's a dynamic sliding window."*
- **Tradeoff:** *"I can do O(n²) brute force, but since n is 10⁵ I'll aim for O(n log n) with sorting + two pointers."*
- **Before coding:** *"My plan: sort by finish time, then greedily pick non-overlapping intervals. Let me code that."*
- **While testing:** *"Let me trace `[1,1,2]` — pointer at index 0 … output length 2. Correct."*
- **Complexity:** *"Time O(n) since each element enters and leaves the window once; space O(k) for the map."*

> 💡 Interviewers hire for *how you think*. Narrate the framework from [Part I](01_Foundations_of_Problem_Solving.md).

---

## 5.4 Edge Cases & Traps Checklist

Before declaring done, verify:

- **Empty input** — empty array/string/list, null tree root.
- **Single element** / two elements.
- **All identical / duplicates** (skip-duplicate logic in two-pointer/backtracking).
- **Already sorted / reverse sorted** — degenerate BST, worst-case quicksort.
- **Negatives / zero** — products, division, prefix sums, sliding window assumptions.
- **Integer overflow** — use `long long`; `mid = lo + (hi-lo)/2`.
- **Off-by-one** — inclusive vs exclusive bounds, `<` vs `<=`.
- **Null / dangling pointers** — linked lists, trees (check before deref).
- **Cycles** — linked lists & graphs → infinite loops; mark visited.
- **Disconnected graph** — iterate over all nodes.
- **Min-heap vs max-heap** — Top-K and Dijkstra mix-ups.
- **`map[key]` auto-inserts** — use `.count()`/`.find()` to test.
- **Deep recursion** — convert to iteration to avoid stack overflow.
- **Modulo arithmetic** — apply `% (1e9+7)` at every step for large counts.

---

## 5.5 A Study Schedule (Pattern-First)

A focused ramp. Adjust pace to your level; **depth per pattern beats random breadth**.

| Phase | Focus | Goal |
|---|---|---|
| 1 | Two Pointers, Sliding Window, Hashing, Prefix Sum | recognize array patterns instantly |
| 2 | Binary Search (+ on answer), Stack/Monotonic Stack | master searching & ordered scans |
| 3 | Linked List (reversal, fast/slow), Intervals, Cyclic Sort | finish core patterns |
| 4 | Trees (BFS/DFS) | recursion fluency |
| 5 | Graphs (BFS/DFS, topo, union-find, Dijkstra) | model & traverse |
| 6 | Heaps (Top-K, K-way merge, two heaps) | streaming/priority |
| 7 | Backtracking, Subsets/Permutations | enumeration with pruning |
| 8 | DP (1-D → 2-D → interval/bitmask) | the big one — go slow, many reps |
| 9 | Greedy, Bit Manipulation, Monotonic Deque | round out the toolkit |
| 10 | Mixed review + timed mock interviews | recognition under pressure |

**Per pattern:** read the section → implement the template from memory → solve 5–8 problems → log the pattern name and any trap you hit. Revisit missed problems after a few days.

> 💡 **The finish line:** you read a new problem, and within seconds you can say *"this is pattern X because of signal Y, target complexity Z"* — then the code is the easy part.

---

## 5.6 Final Mindset

- **Brute force first** — it anchors correctness and reveals the bottleneck.
- **Name the pattern before coding** — the framework is your safety net.
- **Constraints choose the complexity; complexity narrows the pattern.**
- **Talk through your reasoning** — it's half the evaluation.
- **Practice deliberately, by pattern, with spaced repetition.**

---

[← Back to Table of Contents](README.md) · [Foundations](01_Foundations_of_Problem_Solving.md) · [Core Patterns](02_Core_Patterns.md) · [Advanced Patterns](03_Advanced_Patterns.md) · [Pattern Recognition](04_Pattern_Recognition.md)

*End of book. Recognize the pattern, trust the template, and explain as you go.*
