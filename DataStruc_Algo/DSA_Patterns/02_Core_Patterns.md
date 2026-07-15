# Part II — Core Patterns

[← Back to Table of Contents](README.md)

The everyday patterns. Master these first — they appear in the majority of problems. Each follows the same anatomy: **Intuition → When to use (signals) → Template → Worked example → Complexity → Variations → Pitfalls.**

---

## 2.1 Two Pointers

### Intuition
Use **two indices** that traverse the data in a coordinated way — converging from both ends, or one chasing the other — to replace an O(n²) nested loop with a single O(n) pass. Works because each move provably eliminates possibilities.

### 🎯 When to use (signals)
- Input is **sorted** (or sorting it helps).
- "Find a **pair / triplet** with a given sum/property."
- In-place **partition / dedup / move** operations.
- Palindrome checks; comparing two sequences.
- Constraint hints O(n) or O(n log n) for what looks like a pairs problem.

### Template
```cpp
// Converging pointers on a sorted array
int l = 0, r = n - 1;
while (l < r) {
    int val = a[l] + a[r];
    if (val == target) { /* found */ break; }
    else if (val < target) ++l;   // need bigger → raise the low end
    else --r;                     // need smaller → lower the high end
}
```

### Worked example — 3Sum (find all unique triplets summing to 0)
Fix one element, two-pointer the rest; skip duplicates to keep triplets unique.
```cpp
#include <vector>
#include <algorithm>
std::vector<std::vector<int>> threeSum(std::vector<int>& a) {
    std::sort(a.begin(), a.end());
    std::vector<std::vector<int>> res;
    int n = a.size();
    for (int i = 0; i < n - 2; ++i) {
        if (i > 0 && a[i] == a[i-1]) continue;          // skip duplicate pivot
        int l = i + 1, r = n - 1;
        while (l < r) {
            int s = a[i] + a[l] + a[r];
            if (s < 0) ++l;
            else if (s > 0) --r;
            else {
                res.push_back({a[i], a[l], a[r]});
                while (l < r && a[l] == a[l+1]) ++l;     // skip dup left
                while (l < r && a[r] == a[r-1]) --r;     // skip dup right
                ++l; --r;
            }
        }
    }
    return res;
}
```

### Complexity. Sorting O(n log n) + scan O(n²) for 3Sum; O(n) for pair problems. Space O(1) extra.

### Variations
- **Opposite ends:** two-sum sorted, container-with-most-water, valid palindrome, reverse.
- **Same direction (slow/fast):** remove duplicates in-place, move zeros, partition (Dutch national flag).
- **Two arrays:** merge two sorted arrays, intersection of sorted arrays.

### ⚠️ Pitfalls
- Forgetting to **skip duplicates** → duplicate results.
- Moving the wrong pointer (must respect the sorted-order logic).
- Off-by-one in `l < r` vs `l <= r`.

---

## 2.2 Sliding Window

### Intuition
Maintain a **contiguous window** `[l, r]` over the data and update an aggregate **incrementally** as the window slides, instead of recomputing from scratch. Each element enters and leaves the window once → O(n).

### 🎯 When to use (signals)
- **Contiguous** subarray/substring problem.
- "Longest / shortest / max-sum window satisfying a condition."
- "Window of size k" (fixed) or "smallest/longest under a constraint" (dynamic).
- Brute force is "for each start, scan forward" → O(n²).

### Templates
```cpp
// Fixed-size window of size k
int sum = 0;
for (int i = 0; i < k; ++i) sum += a[i];
int best = sum;
for (int r = k; r < n; ++r) {
    sum += a[r] - a[r - k];          // add entering, remove leaving
    best = std::max(best, sum);
}
```
```cpp
// Dynamic window: grow r, shrink l while constraint violated
int l = 0, best = 0;
for (int r = 0; r < n; ++r) {
    add(a[r]);                       // expand window
    while (constraintViolated())     // shrink until valid
        remove(a[l++]);
    best = std::max(best, r - l + 1);
}
```

### Worked example — Longest substring without repeating characters
```cpp
#include <unordered_map>
int longestUnique(const std::string& s) {
    std::unordered_map<char,int> last;     // char → last seen index
    int l = 0, best = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        if (last.count(s[r]) && last[s[r]] >= l)
            l = last[s[r]] + 1;            // jump l past the previous occurrence
        last[s[r]] = r;
        best = std::max(best, r - l + 1);
    }
    return best;
}
```
**Dry run** `"abcabcbb"`: window grows `a,ab,abc`; at second `a`, `l` jumps to 1; max length stays 3.

### Complexity. O(n) time; O(1) or O(k)/O(charset) space.

### Variations
- Max sum of size k; min-window-substring (Hard); longest substring with ≤ K distinct; permutation-in-string / anagram search; max consecutive ones after K flips.
- Combine with a **monotonic deque** for sliding-window maximum ([§3.15](03_Advanced_Patterns.md#315-monotonic-deque-sliding-window-extremum)).

### ⚠️ Pitfalls
- Moving `l` **backward** — it must only advance (guard with `>= l`).
- Forgetting to update the aggregate when shrinking.
- Confusing fixed vs dynamic window logic.

---

## 2.3 Fast & Slow Pointers (Floyd's Cycle Detection)

### Intuition
Two pointers move at **different speeds** (slow +1, fast +2). On a cycle, the fast pointer laps the slow one and they meet; on a line, fast reaches the end. Also finds the **middle** (when fast hits the end, slow is at the middle).

### 🎯 When to use (signals)
- **Linked list** cycle detection / find cycle start.
- Find the **middle** of a linked list in one pass.
- "Happy number," sequences that eventually cycle.
- Detect a cycle without extra O(n) memory.

### Template
```cpp
ListNode *slow = head, *fast = head;
while (fast && fast->next) {
    slow = slow->next;          // +1
    fast = fast->next->next;    // +2
    if (slow == fast) { /* cycle */ break; }
}
```

### Worked example — find the cycle's start node
After they meet, reset one pointer to head; advance both by 1; they meet at the entrance (a classic math identity).
```cpp
ListNode* detectCycleStart(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next; fast = fast->next->next;
        if (slow == fast) {                 // meeting point
            ListNode* p = head;
            while (p != slow) { p = p->next; slow = slow->next; }
            return p;                        // cycle start
        }
    }
    return nullptr;                          // no cycle
}
```

### Complexity. O(n) time, **O(1) space** (the key advantage over a hash set).

### Variations. Middle of list, palindrome linked list (find middle + reverse half), happy number, find duplicate number (Floyd on index→value mapping).

### ⚠️ Pitfalls
- Null checks: test `fast && fast->next` before `fast->next->next`.
- Off-by-one for "middle" (two middles on even length — decide which).

---

## 2.4 Prefix Sum / Difference Array

### Intuition
Precompute cumulative sums so any **range sum** is answered in O(1) by subtraction: `sum(l..r) = pre[r+1] − pre[l]`. The **difference array** is the inverse: apply many range *updates* in O(1) each, then reconstruct.

### 🎯 When to use (signals)
- Many **range-sum queries** on a static array.
- "Subarray with sum = K" / "count subarrays summing to K."
- Many **range updates** then a final read (difference array).
- 2D versions: submatrix sums.

### Templates
```cpp
// 1) Prefix sums for O(1) range queries
std::vector<long long> pre(n + 1, 0);
for (int i = 0; i < n; ++i) pre[i+1] = pre[i] + a[i];
// sum of a[l..r] = pre[r+1] - pre[l]
```
```cpp
// 2) Prefix-sum + hashmap: count subarrays with sum == k
#include <unordered_map>
int subarraySum(const std::vector<int>& a, int k) {
    std::unordered_map<long long,int> cnt{{0,1}};   // prefix 0 seen once
    long long sum = 0; int res = 0;
    for (int x : a) {
        sum += x;
        res += cnt[sum - k];          // # earlier prefixes that make sum-k
        ++cnt[sum];
    }
    return res;
}
```
```cpp
// 3) Difference array: range update [l,r] += val in O(1)
std::vector<long long> diff(n + 1, 0);
auto update = [&](int l, int r, long long val){ diff[l]+=val; diff[r+1]-=val; };
// reconstruct: a[i] = running prefix of diff
```

### Complexity. Build O(n); each query/update O(1). 2D: build O(nm), query O(1).

### Variations. Count subarrays with sum K, subarray sums divisible by K (prefix mod), equal 0s/1s (map +1/−1), 2D submatrix sum, range increment problems (difference array).

### ⚠️ Pitfalls
- Off-by-one in `pre` indexing (size n+1, `pre[0]=0`).
- Integer overflow → use `long long`.
- For "sum == k" counting, seed the map with `{0:1}` (empty prefix).

---

## 2.5 Binary Search (on array & on the answer)

### Intuition
Repeatedly **halve** a monotonic search space. Classic: search a sorted array. Powerful generalization: **binary search on the answer** — when "is value X feasible?" is monotonic (true for all ≥ some threshold), search that threshold even without an explicit sorted array.

### 🎯 When to use (signals)
- Data is **sorted** or has a **monotonic** property.
- "Find min/max value such that a condition holds."
- "Minimize the maximum" / "maximize the minimum."
- Huge answer range (up to 10¹⁸) with a feasibility check.

### Templates
```cpp
// Lower bound: first index with a[i] >= target
int lo = 0, hi = n;                     // half-open [lo, hi)
while (lo < hi) {
    int mid = lo + (hi - lo) / 2;
    if (a[mid] < target) lo = mid + 1;
    else                 hi = mid;
}
// lo = insertion point
```
```cpp
// Binary search on the answer: smallest x with feasible(x) == true
long long lo = LO, hi = HI;
while (lo < hi) {
    long long mid = lo + (hi - lo) / 2;
    if (feasible(mid)) hi = mid;        // mid works → try smaller
    else               lo = mid + 1;    // mid fails → go higher
}
// lo = minimal feasible answer
```

### Worked example — minimum eating speed (Koko bananas)
Find the smallest speed `k` such that all piles can be eaten within `H` hours. `feasible(k)` = total hours ≤ H, which is monotonic in `k`.
```cpp
#include <vector>
int minEatingSpeed(const std::vector<int>& piles, int H) {
    auto hours = [&](long long k){
        long long h = 0;
        for (int p : piles) h += (p + k - 1) / k;   // ceil division
        return h;
    };
    long long lo = 1, hi = *std::max_element(piles.begin(), piles.end());
    while (lo < hi) {
        long long mid = lo + (hi - lo) / 2;
        if (hours(mid) <= H) hi = mid; else lo = mid + 1;
    }
    return lo;
}
```

### Complexity. O(log(range) × cost-of-feasibility). Array search O(log n).

### Variations. lower/upper bound, first/last occurrence, search in rotated sorted array, peak element, median of two sorted arrays, "split array largest sum," "ship within D days," allocate books/pages.

### ⚠️ Pitfalls
- `mid` overflow → use `lo + (hi-lo)/2`.
- Wrong loop boundary (`<` vs `<=`) or update (`mid` vs `mid±1`) → infinite loop.
- Feasibility predicate not actually monotonic → binary search invalid.

---

## 2.6 Hashing & Frequency Counting

### Intuition
Trade memory for time: a hash map/set turns "have I seen X?" and "how many X?" into **O(1)** operations, collapsing many O(n²) scans to O(n).

### 🎯 When to use (signals)
- "Has this appeared before?", "count occurrences," "deduplicate."
- Find pairs/complements (Two Sum), anagrams, group by key.
- Detect duplicates, first unique, majority element.

### Template
```cpp
#include <unordered_map>
#include <unordered_set>
std::unordered_map<int,int> freq;
for (int x : a) ++freq[x];               // frequency table
std::unordered_set<int> seen;
for (int x : a) {
    if (seen.count(target - x)) { /* complement found */ }
    seen.insert(x);
}
```

### Worked example — group anagrams (signature key)
```cpp
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& v) {
    std::unordered_map<std::string, std::vector<std::string>> groups;
    for (auto& s : v) {
        std::string key = s;
        std::sort(key.begin(), key.end());   // anagrams share sorted signature
        groups[key].push_back(s);
    }
    std::vector<std::vector<std::string>> out;
    for (auto& [k, g] : groups) out.push_back(std::move(g));
    return out;
}
```

### Complexity. O(n) average per pass; O(n) space.

### Variations. Two Sum, longest consecutive sequence (O(n) with a set), subarray sum = K (with prefix sums), top-K frequent (map + heap/bucket), isomorphic strings, ransom note.

### ⚠️ Pitfalls
- `map[key]` **auto-inserts** a default — use `.count()`/`.find()` to test without inserting.
- Custom key types need a `std::hash` specialization + `operator==`.
- Never rely on `unordered_map` iteration order.

---

## 2.7 Stack & Monotonic Stack

### Intuition
A **stack** (LIFO) naturally handles nested/most-recent-first structure. A **monotonic stack** keeps elements in increasing or decreasing order, enabling O(n) answers to "next/previous greater/smaller element."

### 🎯 When to use (signals)
- Matching/nesting: parentheses, expression parsing.
- "Next greater/smaller element," "previous greater," histogram areas.
- "Span," "daily temperatures," "stock span."
- Undo / backtrack to the most recent state.

### Templates
```cpp
// Balanced parentheses
std::stack<char> st;
for (char c : s) {
    if (isOpen(c)) st.push(c);
    else { if (st.empty() || !matches(st.top(), c)) return false; st.pop(); }
}
return st.empty();
```
```cpp
// Monotonic (decreasing) stack: next greater element to the right
std::vector<int> nextGreater(const std::vector<int>& a) {
    int n = a.size();
    std::vector<int> res(n, -1);
    std::stack<int> st;                       // holds indices, values decreasing
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && a[i] > a[st.top()]) {
            res[st.top()] = a[i];             // a[i] is next-greater for st.top()
            st.pop();
        }
        st.push(i);
    }
    return res;
}
```

### Worked example — largest rectangle in histogram
Monotonic increasing stack of bar indices; when a shorter bar appears, pop and compute areas using the popped bar as the limiting height.
```cpp
int largestRectangle(std::vector<int> h) {
    h.push_back(0);                            // sentinel flushes the stack
    std::stack<int> st;
    int best = 0;
    for (int i = 0; i < (int)h.size(); ++i) {
        while (!st.empty() && h[i] < h[st.top()]) {
            int height = h[st.top()]; st.pop();
            int left = st.empty() ? -1 : st.top();
            best = std::max(best, height * (i - left - 1));
        }
        st.push(i);
    }
    return best;
}
```

### Complexity. O(n) — each index is pushed and popped at most once.

### Variations. Valid parentheses, min-stack (O(1) getMin), evaluate RPN, decode string, daily temperatures, trapping rain water (stack variant), next greater in a circular array.

### ⚠️ Pitfalls
- `top()`/`pop()` on an empty stack → undefined behavior; guard with `empty()`.
- Storing **indices** (not values) when you need distances/spans.
- Choosing increasing vs decreasing stack incorrectly for the query.

---

## 2.8 In-place Linked List Reversal

### Intuition
Reverse pointers as you walk the list, using a constant number of pointers (`prev`, `cur`, `next`) — no extra array. A building block for many linked-list problems.

### 🎯 When to use (signals)
- "Reverse a linked list" (whole, or a sub-range, or in groups of k).
- Reorder/rotate a list; palindrome check (reverse the second half).
- Must be **O(1) space** (can't copy to an array).

### Template
```cpp
ListNode* reverse(ListNode* head) {
    ListNode* prev = nullptr;
    while (head) {
        ListNode* next = head->next;   // save
        head->next = prev;             // reverse the link
        prev = head;                   // advance prev
        head = next;                   // advance head
    }
    return prev;                       // new head
}
```

### Worked example — reverse nodes in groups of k
```cpp
ListNode* reverseKGroup(ListNode* head, int k) {
    ListNode* node = head;
    for (int i = 0; i < k; ++i) {              // ensure k nodes remain
        if (!node) return head;                // fewer than k → leave as-is
        node = node->next;
    }
    ListNode* prev = reverseKGroup(node, k);   // reverse the rest first
    for (int i = 0; i < k; ++i) {              // reverse this group
        ListNode* next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;                               // new head of this group
}
```

### Complexity. O(n) time, O(1) space (iterative).

### Variations. Reverse sublist [m,n], rotate list, reorder list, palindrome linked list, swap nodes in pairs.

### ⚠️ Pitfalls
- Losing the rest of the list (save `next` *before* rewiring).
- Off-by-one on group boundaries; reconnecting reversed segments wrong.
- Forgetting to update the new head/tail.

---

## 2.9 Merge Intervals

### Intuition
Sort intervals by start; then sweep once, **merging** any interval that overlaps the current one. Sorting makes overlaps adjacent so a single pass suffices.

### 🎯 When to use (signals)
- Input is a set of **intervals** `[start, end]`.
- "Merge overlapping," "insert interval," "free time," "meeting rooms."
- Scheduling / calendar / range-conflict problems.

### Template
```cpp
#include <vector>
#include <algorithm>
std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& iv) {
    std::sort(iv.begin(), iv.end());           // by start
    std::vector<std::vector<int>> res;
    for (auto& cur : iv) {
        if (res.empty() || res.back()[1] < cur[0])
            res.push_back(cur);                // no overlap → new interval
        else
            res.back()[1] = std::max(res.back()[1], cur[1]);  // merge
    }
    return res;
}
```

### Worked example — meeting rooms II (min rooms needed)
Sweep starts/ends or use a min-heap of end times: a new meeting reuses a room if the earliest end ≤ its start.
```cpp
#include <queue>
int minMeetingRooms(std::vector<std::vector<int>>& iv) {
    std::sort(iv.begin(), iv.end());
    std::priority_queue<int, std::vector<int>, std::greater<>> ends; // min-heap of end times
    for (auto& m : iv) {
        if (!ends.empty() && ends.top() <= m[0]) ends.pop();  // reuse a room
        ends.push(m[1]);
    }
    return ends.size();                        // peak concurrency = rooms
}
```

### Complexity. O(n log n) for the sort, O(n) sweep.

### Variations. Insert interval, interval intersections, non-overlapping intervals (greedy removal), meeting rooms I/II, employee free time, car pooling (difference array on time).

### ⚠️ Pitfalls
- Forgetting to **sort first**.
- Boundary policy: do `[1,2]` and `[2,3]` overlap? Decide `<` vs `<=`.
- Updating `end` with `max` (a contained interval shouldn't shrink the merge).

---

## 2.10 Cyclic Sort

### Intuition
When an array contains numbers in a known range (e.g., `1..n` or `0..n−1`), each value has a **natural home index**. Place each number where it belongs by swapping; then any index whose value is wrong reveals a missing/duplicate number — all in O(n) and O(1) space.

### 🎯 When to use (signals)
- Array of `n` numbers in range `1..n` or `0..n−1`.
- "Find the missing / duplicate / all missing numbers."
- "First missing positive."
- Must be O(n) time and O(1) space (no extra hash set).

### Template
```cpp
// Place each value v at index v-1 (for range 1..n)
int i = 0;
while (i < n) {
    int correct = a[i] - 1;                    // home index of a[i]
    if (a[i] >= 1 && a[i] <= n && a[i] != a[correct])
        std::swap(a[i], a[correct]);           // send a[i] home
    else
        ++i;                                   // already correct / out of range
}
```

### Worked example — find all numbers disappeared in [1..n]
```cpp
#include <vector>
std::vector<int> findDisappeared(std::vector<int>& a) {
    int n = a.size(), i = 0;
    while (i < n) {
        int correct = a[i] - 1;
        if (a[i] != a[correct]) std::swap(a[i], a[correct]);
        else ++i;
    }
    std::vector<int> missing;
    for (int j = 0; j < n; ++j)
        if (a[j] != j + 1) missing.push_back(j + 1);  // wrong home → missing
    return missing;
}
```

### Complexity. O(n) time (each swap places one number permanently), O(1) extra space.

### Variations. Missing number, find the duplicate, find all duplicates, first missing positive, set mismatch.

### ⚠️ Pitfalls
- Infinite loop: only swap when the target index doesn't already hold the right value (`a[i] != a[correct]`).
- Range guards for values outside `1..n` (first-missing-positive).
- Don't increment `i` after a swap — re-examine the swapped-in value.

---

## Part II Summary

| Pattern | Signal | Complexity | One-line use |
|---|---|---|---|
| Two Pointers | sorted, pair/triplet, in-place | O(n) | converge/chase indices |
| Sliding Window | contiguous subarray/substring | O(n) | incremental window |
| Fast & Slow | linked list cycle/middle | O(n), O(1) | different speeds |
| Prefix Sum | range sums, subarray=K | O(1) query | cumulative subtraction |
| Binary Search | sorted/monotonic, min-max | O(log n) | halve the space |
| Hashing | seen?, count, dedup | O(n) | O(1) lookup |
| Monotonic Stack | next greater/smaller | O(n) | ordered stack |
| List Reversal | reverse in O(1) space | O(n) | rewire pointers |
| Merge Intervals | overlapping ranges | O(n log n) | sort + sweep |
| Cyclic Sort | numbers in 1..n | O(n), O(1) | value → home index |

---

*Next →* [Part III: Advanced Patterns](03_Advanced_Patterns.md)
