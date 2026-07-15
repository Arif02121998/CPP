# File: Arrays/sliding_window_patterns.md

[← Repository Index](../README.md) · **Arrays:** [basics](basics.md) · [operations](operations.md) · [prefix_sum](prefix_sum.md) · sliding_window_patterns · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** The **sliding window** maintains a contiguous range `[l, r]` over an array/string and updates an aggregate (sum, count, max, distinct count…) **incrementally** as the window moves, instead of recomputing it from scratch.

**Why it exists.** Many "best contiguous segment" problems are O(n²) by brute force (try every start, scan forward). Because adjacent windows overlap heavily, we can reuse work: add the entering element, remove the leaving one. Each element enters and leaves once → **O(n)**.

**Problem it solves.** Longest/shortest/max-sum contiguous subarray or substring satisfying a constraint.

**Real-world analogy.** A train window scrolling past scenery: as the train moves one car forward, one new view appears and one old view disappears — you don't re-scan the entire landscape.

---

## Internal Working

Two flavors:

**Fixed window (size k):**
```
a = [2, 1, 5, 1, 3, 2],  k = 3
[2 1 5]        sum=8
  [1 5 1]      sum = 8 + 1 - 2 = 7
    [5 1 3]    sum = 7 + 3 - 1 = 9   ← max
      [1 3 2]  sum = 9 + 2 - 5 = 6
```

**Dynamic window (grow/shrink to satisfy a condition):**
- Expand `r` to include more.
- While the window **violates** the constraint, shrink from `l`.
- Record the best valid window.

💡 The invariant — "the window `[l, r]` is always (about to be) valid" — is what makes the single pass correct.

---

## Operations (the templates)

### Fixed-size window
```cpp
int sum = 0;
for (int i = 0; i < k; ++i) sum += a[i];     // first window
int best = sum;
for (int r = k; r < n; ++r) {
    sum += a[r] - a[r - k];                  // slide: +enter, -leave
    best = std::max(best, sum);
}
```

### Dynamic window (longest valid)
```cpp
int l = 0, best = 0;
for (int r = 0; r < n; ++r) {
    add(a[r]);                               // expand
    while (invalid())                        // shrink until valid
        remove(a[l++]);
    best = std::max(best, r - l + 1);
}
```

### Dynamic window (shortest valid)
```cpp
int l = 0, best = INT_MAX;
for (int r = 0; r < n; ++r) {
    add(a[r]);
    while (valid()) {                        // shrink while still valid
        best = std::min(best, r - l + 1);
        remove(a[l++]);
    }
}
```

---

## Complexity

| | Time | Space |
|---|---|---|
| Fixed window | O(n) | O(1) |
| Dynamic window | O(n) | O(1)–O(k)/O(charset) |

Each index is added once and removed at most once → amortized O(n), even with the inner `while` loop.

---

## C++ Implementation

```cpp
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <climits>

// Fixed: maximum sum of any k consecutive elements
int maxSumK(const std::vector<int>& a, int k) {
    int sum = 0;
    for (int i = 0; i < k; ++i) sum += a[i];
    int best = sum;
    for (int r = k; r < (int)a.size(); ++r) {
        sum += a[r] - a[r - k];
        best = std::max(best, sum);
    }
    return best;
}

// Dynamic: longest substring without repeating characters
int longestUnique(const std::string& s) {
    std::unordered_map<char,int> last;       // char → last index
    int l = 0, best = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        if (last.count(s[r]) && last[s[r]] >= l)
            l = last[s[r]] + 1;              // jump l past the previous copy
        last[s[r]] = r;
        best = std::max(best, r - l + 1);
    }
    return best;
}

// Dynamic: smallest subarray with sum >= target (positive numbers)
int minSubArrayLen(int target, const std::vector<int>& a) {
    int l = 0, sum = 0, best = INT_MAX;
    for (int r = 0; r < (int)a.size(); ++r) {
        sum += a[r];
        while (sum >= target) {
            best = std::min(best, r - l + 1);
            sum -= a[l++];
        }
    }
    return best == INT_MAX ? 0 : best;
}
```

**STL equivalent.** No single STL call, but `std::deque` powers the **sliding-window maximum** variant (see [monotonic_queue.md](../Queues/monotonic_queue.md)).

**When to use a window vs prefix sum:** windows shine when the window grows/shrinks monotonically and (often) values are non-negative. For arbitrary range sums or "sum == K" with negatives, use [prefix sums](prefix_sum.md).

---

## Tradeoffs

- ✅ O(n) time, usually O(1) space; elegant.
- ❌ Requires a **monotonic** relationship (growing the window doesn't un-satisfy then re-satisfy unpredictably). Negative numbers can break "shrink while valid" logic — use prefix sums + deque instead.

---

## Use Cases
- Network throughput over a moving time window; rate limiting.
- Longest/shortest substring problems; anagram/permutation search.
- Max average, bounded-distinct-elements, "at most K replacements".

---

## Pitfalls
- ⚠️ Moving `l` **backward** — it must only advance (guard with `>= l`).
- ⚠️ Forgetting to update the aggregate when shrinking.
- ⚠️ Applying a window to a problem with negatives where shrinking logic fails — switch to prefix sums.
- ⚠️ Mixing fixed vs dynamic templates.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Contiguous subarray/substring" + "longest/shortest/max" → **sliding window**.
- "Window of size k" → fixed window.
- "with at most K distinct / K replacements / sum ≥ target" → dynamic window.
- "max/min in each window" → window + **monotonic deque**.

**Common problem types:** max-sum size k, longest unique substring, min-window-substring, permutation-in-string, max consecutive ones III.

---

## Example Problems

### Easy — Maximum Average Subarray I
Max average of any length-k window = max sum / k.
```cpp
double findMaxAverage(const std::vector<int>& a, int k) {
    return maxSumK(a, k) / (double)k;     // reuse fixed-window sum
}
```

### Medium — Longest Repeating Character Replacement
Longest substring obtainable by replacing ≤ k characters so all are equal.
**Brute force:** every substring, check feasibility → O(n²).
**Optimized:** window valid while `(windowLen − maxFreqChar) ≤ k` → O(n).
```cpp
int characterReplacement(const std::string& s, int k) {
    std::vector<int> cnt(26, 0);
    int l = 0, maxFreq = 0, best = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        maxFreq = std::max(maxFreq, ++cnt[s[r]-'A']);
        while ((r - l + 1) - maxFreq > k) --cnt[s[l++]-'A'];  // too many replacements
        best = std::max(best, r - l + 1);
    }
    return best;
}
```
**Explanation.** A window is valid if the characters we'd replace (`len − mostFrequent`) don't exceed k.

### Hard — Minimum Window Substring
Smallest window of `s` containing all characters of `t`.
**Brute force:** every substring, check coverage → O(n²·|t|).
**Optimized:** expand to cover, shrink to minimize → O(n).
```cpp
std::string minWindow(const std::string& s, const std::string& t) {
    std::unordered_map<char,int> need;
    for (char c : t) ++need[c];
    int required = need.size(), formed = 0, l = 0, bestLen = INT_MAX, bestL = 0;
    std::unordered_map<char,int> win;
    for (int r = 0; r < (int)s.size(); ++r) {
        if (++win[s[r]] == need[s[r]]) ++formed;
        while (formed == required) {
            if (r - l + 1 < bestLen) { bestLen = r - l + 1; bestL = l; }
            if (win[s[l]]-- == need[s[l]]) --formed;
            ++l;
        }
    }
    return bestLen == INT_MAX ? "" : s.substr(bestL, bestLen);
}
```
**Explanation.** Track how many *distinct required characters* are fully satisfied (`formed`); when all are, shrink to record the minimum.

---

*Next →* [advanced_problems.md](advanced_problems.md)
