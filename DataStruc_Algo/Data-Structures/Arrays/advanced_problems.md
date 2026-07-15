# File: Arrays/advanced_problems.md

[← Repository Index](../README.md) · **Arrays:** [basics](basics.md) · [operations](operations.md) · [prefix_sum](prefix_sum.md) · [sliding_window_patterns](sliding_window_patterns.md) · advanced_problems

---

## Conceptual Understanding

**Definition.** This file collects the **canonical advanced array techniques** that recur in interviews and contests: Kadane's algorithm, Dutch national flag partitioning, two-pointer water trapping, cyclic sort, and merge-based counting.

**Why a dedicated file.** These problems each crystallize a *reusable trick* that transfers to dozens of variants. Knowing them turns "hard" array problems into pattern recall.

**Real-world analogy.** A toolbox of specialized jigs: once you know which jig fits a cut, the work is fast and precise.

---

## Technique 1 — Kadane's Algorithm (Maximum Subarray)

**Idea.** A running DP: the best subarray ending at `i` is either `a[i]` alone or `a[i]` extended onto the best subarray ending at `i-1`.

$$best\_here_i = \max(a[i],\; best\_here_{i-1} + a[i])$$

```cpp
#include <vector>
#include <climits>
int maxSubArray(const std::vector<int>& a) {
    int bestHere = a[0], best = a[0];
    for (size_t i = 1; i < a.size(); ++i) {
        bestHere = std::max(a[i], bestHere + a[i]);   // extend or restart
        best = std::max(best, bestHere);
    }
    return best;
}
```
**Complexity:** O(n)/O(1). ⚠️ Initialize with `a[0]`, not 0, to handle all-negative arrays. **Variants:** max product subarray (track min too, for negatives), max circular subarray, max sum with at most one deletion.

---

## Technique 2 — Dutch National Flag (3-way Partition)

**Idea.** Sort an array of three values (0/1/2) in **one pass, O(1) space** using three pointers `low, mid, high`. Generalizes to quicksort's 3-way partition (handles duplicate pivots).

```cpp
void sortColors(std::vector<int>& a) {
    int low = 0, mid = 0, high = a.size() - 1;
    while (mid <= high) {
        if (a[mid] == 0)      std::swap(a[low++], a[mid++]);
        else if (a[mid] == 1) ++mid;
        else                  std::swap(a[mid], a[high--]);  // don't ++mid here
    }
}
```
**Dry run** `[2,0,2,1,1,0]` → `[0,0,1,1,2,2]`. **Complexity:** O(n)/O(1).
⚠️ After swapping with `high`, **don't advance `mid`** — the swapped-in value is unexamined.

---

## Technique 3 — Trapping Rain Water (Two Pointers)

**Idea.** Water above index `i` = `min(maxLeft, maxRight) − height[i]`. Two pointers track running maxima from both ends, always moving the shorter side.

**Brute force:** for each i compute maxLeft/maxRight by scanning → O(n²).
**Optimized:** two pointers → O(n)/O(1).
```cpp
int trap(const std::vector<int>& h) {
    int l = 0, r = h.size() - 1, leftMax = 0, rightMax = 0, water = 0;
    while (l < r) {
        if (h[l] < h[r]) {
            leftMax = std::max(leftMax, h[l]);
            water += leftMax - h[l];          // left side is the bottleneck
            ++l;
        } else {
            rightMax = std::max(rightMax, h[r]);
            water += rightMax - h[r];
            --r;
        }
    }
    return water;
}
```
**Why it works.** The shorter wall *bounds* the water at that index, so we can safely commit it. **Complexity:** O(n)/O(1).

---

## Technique 4 — Cyclic Sort (numbers in 1..n)

**Idea.** When values are a permutation/near-permutation of `1..n`, each value's home is `value − 1`. Place each value home by swapping; mismatches reveal missing/duplicate numbers — O(n)/O(1).

```cpp
std::vector<int> findDisappeared(std::vector<int>& a) {
    int n = a.size(), i = 0;
    while (i < n) {
        int correct = a[i] - 1;
        if (a[i] != a[correct]) std::swap(a[i], a[correct]);
        else ++i;
    }
    std::vector<int> missing;
    for (int j = 0; j < n; ++j) if (a[j] != j + 1) missing.push_back(j + 1);
    return missing;
}
```
**Variants:** missing number, find the duplicate, first missing positive, set mismatch.
⚠️ Only swap when `a[i] != a[correct]` (else infinite loop); don't `++i` right after a swap.

---

## Technique 5 — Count Inversions (Merge Sort)

**Idea.** An *inversion* is a pair `i < j` with `a[i] > a[j]`. Count them during a modified merge sort: when taking from the right half, all remaining left-half elements form inversions.

```cpp
long long mergeCount(std::vector<int>& a, int l, int m, int r) {
    std::vector<int> tmp; long long inv = 0;
    int i = l, j = m + 1;
    while (i <= m && j <= r) {
        if (a[i] <= a[j]) tmp.push_back(a[i++]);
        else { inv += (m - i + 1); tmp.push_back(a[j++]); }  // a[i..m] all > a[j]
    }
    while (i <= m) tmp.push_back(a[i++]);
    while (j <= r) tmp.push_back(a[j++]);
    for (int k = 0; k < (int)tmp.size(); ++k) a[l + k] = tmp[k];
    return inv;
}
long long countInversions(std::vector<int>& a, int l, int r) {
    if (l >= r) return 0;
    int m = l + (r - l) / 2;
    return countInversions(a, l, m) + countInversions(a, m+1, r)
         + mergeCount(a, l, m, r);
}
```
**Complexity:** O(n log n). **Use case:** "how far from sorted," ranking, Kendall-tau distance.

---

## Complexity Summary

| Technique | Time | Space |
|---|---|---|
| Kadane | O(n) | O(1) |
| Dutch flag | O(n) | O(1) |
| Trapping rain water | O(n) | O(1) |
| Cyclic sort | O(n) | O(1) |
| Count inversions | O(n log n) | O(n) |

---

## Tradeoffs
- These O(n)/O(1) tricks beat generic O(n²)/O(n log n) approaches but are **specialized** — each relies on a structural property (running optimum, value range, monotonic walls). Recognize the property first.

---

## Use Cases
- Stock/profit analysis (Kadane), histogram/terrain water modeling, three-way classification, data-integrity checks (missing/dup), sortedness metrics.

---

## Pitfalls
- ⚠️ Kadane: wrong initialization on all-negative input.
- ⚠️ Dutch flag: advancing `mid` after a `high` swap.
- ⚠️ Cyclic sort: infinite loops without the equality guard.
- ⚠️ Inversions: integer overflow — use `long long`.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Maximum/largest sum contiguous" → **Kadane**.
- "Sort 0/1/2" / "partition around a pivot with duplicates" → **Dutch flag**.
- "Water/area between bars," "container" → **two pointers**.
- "Array of 1..n, find missing/duplicate, O(1) space" → **cyclic sort**.
- "Count pairs out of order / smaller to the right" → **merge-sort counting / BIT**.

---

## Example Problems

### Easy — Maximum Subarray (Kadane). See Technique 1.

### Medium — Sort Colors (Dutch flag). See Technique 2.

### Hard — Trapping Rain Water (two pointers). See Technique 3.
For "**Count of Smaller Numbers After Self**" (Hard), use the merge-sort counting idea (Technique 5) or a [Fenwick tree](../Advanced/fenwick_tree.md) over compressed values — both O(n log n).

---

*Next →* [Linked-Lists/singly_linked_list.md](../Linked-Lists/singly_linked_list.md)
