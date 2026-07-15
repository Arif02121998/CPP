# File: Queues/monotonic_queue.md

[← Repository Index](../README.md) · **Queues:** [queue](queue.md) · [deque](deque.md) · [priority_queue](priority_queue.md) · monotonic_queue

---

## Conceptual Understanding

**Definition.** A **monotonic queue** is a deque whose elements are kept in sorted (increasing or decreasing) order by discarding elements that can never again be the answer. It supports querying the **min or max of a sliding window in amortized O(1)**.

**Why it exists.** "Maximum/minimum of every window of size k" is naively O(nk). A heap brings it to O(n log k) but can't cheaply evict elements that fall out of the window. A monotonic deque achieves **O(n)** by maintaining only useful candidates.

**Problem it solves.** Sliding-window maximum/minimum, and any DP where a transition takes the best value over a moving range (sliding-window optimization of DP).

**Real-world analogy.** A leaderboard where, when a new high score arrives, everyone it beats is permanently irrelevant and removed — so the leader is always at the front.

---

## Internal Working

Store **indices** in a deque. For a **maximum** query keep values **decreasing** front→back:

1. **Pop back** while the back's value ≤ incoming value (they can never be the max while the newcomer is present).
2. **Push** the new index at the back.
3. **Pop front** if it's outside the current window (`front <= i - k`).
4. The **front** is the window's maximum.

```
nums = [1,3,-1,-3,5,3,6,7], k = 3   (window max)

i=0 v=1   deque idx=[0]            (vals:1)
i=1 v=3   pop 0(1<=3) push1 [1]    (3)
i=2 v=-1  push2 [1,2]              (3,-1)   window full → max=nums[1]=3
i=3 v=-3  push3 [1,2,3] front1 valid → max=3
i=4 v=5   pop3,2,1 (all<=5) push4 [4] → max=5
i=5 v=3   push5 [4,5] → max=5
i=6 v=6   pop5,4 push6 [6] → max=6
i=7 v=7   pop6 push7 [7] → max=7
result = [3,3,5,5,6,7]
```

💡 Each index is pushed once and popped once → **O(n)** total despite the inner while-loops.

---

## Operations

| Step | Action | Cost (amortized) |
|---|---|---|
| insert new element | pop dominated backs, push | O(1) |
| evict out-of-window | pop front if stale | O(1) |
| query window extreme | read front | O(1) |

---

## C++ Implementation — Sliding Window Maximum
```cpp
#include <vector>
#include <deque>

std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
    std::deque<int> dq;                         // stores indices, values decreasing
    std::vector<int> res;
    for (int i = 0; i < (int)nums.size(); ++i) {
        // 1) drop smaller values from the back — they can't be max anymore
        while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
        dq.push_back(i);
        // 2) drop the front if it left the window
        if (dq.front() <= i - k) dq.pop_front();
        // 3) record once the first window is complete
        if (i >= k - 1) res.push_back(nums[dq.front()]);
    }
    return res;
}
```
For a **minimum** window, flip the comparison to `nums[dq.back()] >= nums[i]` (keep values increasing).

**STL note.** Built on `std::deque<int>`. There's no dedicated container — it's a technique layered on a deque.

---

## Complexity

| | Time | Space |
|---|---|---|
| All window extremes | O(n) | O(k) |
| Per element amortized | O(1) | — |

Compare: brute force O(nk); heap O(n log k); monotonic deque **O(n)**.

---

## Tradeoffs
- ✅ Optimal O(n) for sliding-window min/max.
- ✅ Constant memory relative to window size O(k).
- ❌ Only gives the extreme, not arbitrary order statistics (use a multiset/heap for the k-th).
- ❌ Trickier to get right than a heap; comparison direction and eviction are bug-prone.

**vs `std::multiset`:** a multiset window also yields min/max (and median) in O(n log k) with easy eviction; the monotonic deque is faster (O(n)) but min/max only.

---

## Use Cases
- Sliding-window maximum/minimum.
- **DP optimization:** `dp[i] = best(dp[j]) for j in [i-k, i-1]) + cost` — the deque maintains the best `dp[j]` over the window (e.g., Jump Game VI, Constrained Subsequence Sum).
- Longest subarray where max−min ≤ limit (two monotonic deques: one max, one min).
- Streaming analytics: rolling max/min over the last k samples.

---

## Pitfalls
- ⚠️ Storing values instead of **indices** → can't evict by window position.
- ⚠️ Wrong window-eviction check (`<= i - k` vs `< i - k + 1`) — off-by-one on window size.
- ⚠️ Using `<` vs `<=` when popping the back affects handling of equal values (still correct for max, but be deliberate).
- ⚠️ Recording the result before the first full window (`i >= k-1`).
- ⚠️ Confusing min vs max — the comparison direction must match the query.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Maximum/minimum of every window of size k" → **monotonic deque**.
- "Longest subarray with max−min ≤ limit" → **two monotonic deques**.
- "DP transition = best over a sliding range" → **monotonic-deque DP optimization**.
- "Shortest subarray with sum ≥ K (with negatives)" → **prefix sums + monotonic deque**.

---

## Example Problems

### Easy/Medium — Sliding Window Maximum
See implementation above. O(n).

### Medium — Constrained Subsequence Sum
`dp[i] = nums[i] + max(0, max(dp[i-k..i-1]))`; maintain a decreasing deque of `dp` values over the last k indices to get that max in O(1).

### Hard — Shortest Subarray with Sum at Least K (handles negatives)
```cpp
#include <vector>
#include <deque>
int shortestSubarray(const std::vector<int>& nums, int K) {
    int n = nums.size();
    std::vector<long long> pre(n + 1, 0);
    for (int i = 0; i < n; ++i) pre[i+1] = pre[i] + nums[i];
    std::deque<int> dq;                          // increasing prefix sums (indices)
    int best = n + 1;
    for (int i = 0; i <= n; ++i) {
        while (!dq.empty() && pre[i] - pre[dq.front()] >= K) {
            best = std::min(best, i - dq.front());   // valid window, shrink from front
            dq.pop_front();
        }
        while (!dq.empty() && pre[dq.back()] >= pre[i]) dq.pop_back(); // keep increasing
        dq.push_back(i);
    }
    return best <= n ? best : -1;
}
```
**Explanation.** With negatives, a plain sliding window fails. Using prefix sums, we want the closest earlier index with `pre[j] <= pre[i] - K`; a monotonic (increasing) deque of prefix indices finds it in amortized O(1). O(n).

---

*Next →* [../Trees/binary_tree.md](../Trees/binary_tree.md)
