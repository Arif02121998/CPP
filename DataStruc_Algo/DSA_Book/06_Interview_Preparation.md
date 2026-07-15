# Chapter 6 — Interview Preparation

[← Back to Table of Contents](README.md)

Problems are grouped by **pattern** (the way interviewers actually think). Each has a concept, an idiomatic C++ solution, complexity, and the **traps** that catch people.

---

## 6.0 Interview Strategy (read this first)

1. **Clarify** — restate the problem, confirm input ranges, ask about edge cases and duplicates.
2. **Examples** — work one small example out loud.
3. **Brute force out loud** — state it, give its complexity. Shows correctness instinct.
4. **Optimize** — name the pattern, explain the idea *before* coding.
5. **Code cleanly** — meaningful names, handle edges, talk as you type.
6. **Test** — dry-run your code on the example + an edge case.
7. **Analyze** — state time/space complexity unprompted.

> 💡 Communication is graded as heavily as correctness. A clear O(n²) explained well often beats a silent buggy O(n).

---

## 6.1 Arrays & Hashing

### Q1. Two Sum *(Easy)*
Return indices of two numbers that add to `target`.
```cpp
#include <vector>
#include <unordered_map>
std::vector<int> twoSum(const std::vector<int>& nums, int target) {
    std::unordered_map<int,int> seen;          // value → index
    for (int i = 0; i < (int)nums.size(); ++i) {
        int need = target - nums[i];
        if (seen.count(need)) return {seen[need], i};
        seen[nums[i]] = i;
    }
    return {};
}
```
**O(n)/O(n).** Trap: check for the complement **before** inserting the current element (else you may match an element with itself).

### Q2. Contains Duplicate *(Easy)*
```cpp
bool hasDuplicate(const std::vector<int>& a) {
    std::unordered_set<int> s;
    for (int x : a) if (!s.insert(x).second) return true;
    return false;
}
```
**O(n)/O(n).** `insert().second` is false if already present — neat idiom.

### Q3. Group Anagrams *(Medium)*
Key insight: anagrams share a **sorted-character signature**.
```cpp
#include <map>
std::vector<std::vector<std::string>> groupAnagrams(std::vector<std::string>& strs) {
    std::unordered_map<std::string, std::vector<std::string>> groups;
    for (auto& s : strs) {
        std::string key = s;
        std::sort(key.begin(), key.end());     // signature
        groups[key].push_back(s);
    }
    std::vector<std::vector<std::string>> res;
    for (auto& [k, v] : groups) res.push_back(std::move(v));
    return res;
}
```
**O(n·k log k).** Trap: don't compare every pair (O(n²)).

### Q4. Product of Array Except Self *(Medium)*
No division; prefix × suffix products.
```cpp
std::vector<int> productExceptSelf(const std::vector<int>& a) {
    int n = a.size();
    std::vector<int> res(n, 1);
    int pre = 1;
    for (int i = 0; i < n; ++i) { res[i] = pre; pre *= a[i]; }   // prefix
    int suf = 1;
    for (int i = n - 1; i >= 0; --i) { res[i] *= suf; suf *= a[i]; } // suffix
    return res;
}
```
**O(n)/O(1) extra.** Trap: handle zeros without division.

---

## 6.2 Two Pointers

### Q5. Valid Palindrome *(Easy)*
```cpp
bool isPalindrome(const std::string& s) {
    int l = 0, r = s.size() - 1;
    while (l < r) {
        while (l < r && !isalnum(s[l])) ++l;   // skip non-alphanumerics
        while (l < r && !isalnum(s[r])) --r;
        if (tolower(s[l++]) != tolower(s[r--])) return false;
    }
    return true;
}
```
**O(n)/O(1).** Trap: case-insensitivity and skipping punctuation.

### Q6. 3Sum *(Medium)*
Sort, fix one element, two-pointer the rest; skip duplicates.
```cpp
std::vector<std::vector<int>> threeSum(std::vector<int>& a) {
    std::sort(a.begin(), a.end());
    std::vector<std::vector<int>> res;
    int n = a.size();
    for (int i = 0; i < n - 2; ++i) {
        if (i > 0 && a[i] == a[i-1]) continue;          // skip dup pivot
        int l = i + 1, r = n - 1;
        while (l < r) {
            int s = a[i] + a[l] + a[r];
            if (s < 0) ++l;
            else if (s > 0) --r;
            else {
                res.push_back({a[i], a[l], a[r]});
                while (l < r && a[l] == a[l+1]) ++l;     // skip dups
                while (l < r && a[r] == a[r-1]) --r;
                ++l; --r;
            }
        }
    }
    return res;
}
```
**O(n²)/O(1).** Trap: **duplicate triplets** — skip equal values at all three positions.

### Q7. Container With Most Water *(Medium)*
```cpp
int maxArea(const std::vector<int>& h) {
    int l = 0, r = h.size() - 1, best = 0;
    while (l < r) {
        best = std::max(best, std::min(h[l], h[r]) * (r - l));
        if (h[l] < h[r]) ++l; else --r;         // move the shorter wall
    }
    return best;
}
```
**O(n)/O(1).** Insight: moving the taller wall can never help.

---

## 6.3 Sliding Window

### Q8. Best Time to Buy/Sell Stock *(Easy)*
```cpp
int maxProfit(const std::vector<int>& p) {
    int minPrice = INT_MAX, best = 0;
    for (int x : p) {
        minPrice = std::min(minPrice, x);
        best = std::max(best, x - minPrice);
    }
    return best;
}
```
**O(n)/O(1).** Track the minimum so far; profit = current − min.

### Q9. Longest Substring Without Repeating Characters *(Medium)*
(See [Ch 4.7](04_Algorithms.md#47-sliding-window) for the implementation.) **O(n).** Trap: only move `start` *forward*, never back — guard with `last[c] >= start`.

### Q10. Minimum Window Substring *(Hard)*
Grow window to cover all needed chars, then shrink to minimize.
```cpp
std::string minWindow(const std::string& s, const std::string& t) {
    std::unordered_map<char,int> need;
    for (char c : t) ++need[c];
    int required = need.size(), formed = 0;
    std::unordered_map<char,int> win;
    int l = 0, bestLen = INT_MAX, bestL = 0;
    for (int r = 0; r < (int)s.size(); ++r) {
        char c = s[r];
        if (++win[c] == need[c]) ++formed;
        while (formed == required) {            // valid → try to shrink
            if (r - l + 1 < bestLen) { bestLen = r - l + 1; bestL = l; }
            char d = s[l++];
            if (win[d]-- == need[d]) --formed;
        }
    }
    return bestLen == INT_MAX ? "" : s.substr(bestL, bestLen);
}
```
**O(n)/O(charset).** Trap: track *how many distinct chars are fully satisfied* (`formed`), not just total counts.

---

## 6.4 Stack

### Q11. Valid Parentheses *(Easy)*
```cpp
bool validParens(const std::string& s) {
    std::stack<char> st;
    std::unordered_map<char,char> match{{')','('},{']','['},{'}','{'}};
    for (char c : s) {
        if (c=='('||c=='['||c=='{') st.push(c);
        else {
            if (st.empty() || st.top() != match[c]) return false;
            st.pop();
        }
    }
    return st.empty();                          // all opened were closed
}
```
**O(n)/O(n).** Trap: empty stack on a closer; leftover opens at the end.

### Q12. Daily Temperatures / Next Greater Element *(Medium)*
Monotonic decreasing stack of indices.
```cpp
std::vector<int> dailyTemperatures(const std::vector<int>& t) {
    int n = t.size();
    std::vector<int> res(n, 0);
    std::stack<int> st;                          // indices, decreasing temps
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && t[i] > t[st.top()]) {
            res[st.top()] = i - st.top();        // days until warmer
            st.pop();
        }
        st.push(i);
    }
    return res;
}
```
**O(n)/O(n).** The monotonic-stack pattern: each index pushed/popped once.

---

## 6.5 Linked List

### Q13. Reverse Linked List *(Easy)*
```cpp
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    while (head) {
        ListNode* nxt = head->next;
        head->next = prev;                       // flip pointer
        prev = head;
        head = nxt;
    }
    return prev;                                 // new head
}
```
**O(n)/O(1).** The most-asked linked-list question. Know it cold.

### Q14. Merge Two Sorted Lists *(Easy)*
```cpp
ListNode* mergeTwoLists(ListNode* a, ListNode* b) {
    ListNode dummy(0); ListNode* tail = &dummy;  // dummy simplifies edges
    while (a && b) {
        if (a->val <= b->val) { tail->next = a; a = a->next; }
        else                  { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = a ? a : b;                       // attach the rest
    return dummy.next;
}
```
**O(n+m)/O(1).** Trap: forgetting the **dummy node** makes head-handling messy.

### Q15. Detect Cycle / Find Cycle Start *(Medium)*
Floyd's algorithm — see [Ch 4.8](04_Algorithms.md#48-two-pointers). To find the *start*: after they meet, reset one pointer to head, advance both by 1; they meet at the cycle entrance.

---

## 6.6 Trees

### Q16. Maximum Depth of Binary Tree *(Easy)*
```cpp
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::max(maxDepth(root->left), maxDepth(root->right));
}
```
**O(n)/O(h).** Foundational recursion.

### Q17. Validate BST *(Medium)*
Pass down valid (min, max) bounds — **not** just compare with the parent.
```cpp
bool validate(TreeNode* n, long lo, long hi) {
    if (!n) return true;
    if (n->val <= lo || n->val >= hi) return false;
    return validate(n->left, lo, n->val) && validate(n->right, n->val, hi);
}
bool isValidBST(TreeNode* root) { return validate(root, LONG_MIN, LONG_MAX); }
```
**O(n).** ⚠️ **Classic trap:** checking only `left < node < right` locally is wrong — a deep descendant can violate the BST property. Use bounds (or in-order traversal must be strictly increasing).

### Q18. Lowest Common Ancestor *(Medium)*
```cpp
TreeNode* lca(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    TreeNode* L = lca(root->left, p, q);
    TreeNode* R = lca(root->right, p, q);
    if (L && R) return root;                     // p,q split here → LCA
    return L ? L : R;
}
```
**O(n).** Elegant post-order: if both sides return non-null, current node is the LCA.

### Q19. Level Order Traversal *(Medium)*
BFS with a queue, processing one level per outer iteration — see [Ch 3.1](03_Advanced_Data_Structures.md#31-trees). Trap: capture `q.size()` *before* the inner loop to delimit a level.

### Q20. Serialize/Deserialize Binary Tree *(Hard)*
Pre-order with null markers. Serialize: `1,2,#,#,3,#,#`. Deserialize: recursively consume tokens. **O(n).** Trap: consistent null markers and delimiter parsing.

---

## 6.7 Graphs

### Q21. Number of Islands *(Medium)*
Flood-fill each unvisited land cell.
```cpp
void sink(std::vector<std::vector<char>>& g, int r, int c) {
    if (r<0||c<0||r>=(int)g.size()||c>=(int)g[0].size()||g[r][c]!='1') return;
    g[r][c] = '0';                               // mark visited
    sink(g, r+1, c); sink(g, r-1, c); sink(g, r, c+1); sink(g, r, c-1);
}
int numIslands(std::vector<std::vector<char>>& g) {
    int count = 0;
    for (int r = 0; r < (int)g.size(); ++r)
        for (int c = 0; c < (int)g[0].size(); ++c)
            if (g[r][c] == '1') { ++count; sink(g, r, c); }
    return count;
}
```
**O(R·C).** Trap: bounds checking; mark cells visited to avoid revisiting.

### Q22. Course Schedule (Cycle Detection in DAG) *(Medium)*
Topological sort via Kahn's algorithm — if you can't order all nodes, there's a cycle → impossible. See [Ch 4.6.3](04_Algorithms.md#463-topological-sort-dag-ordering). **O(V+E).**

### Q23. Clone Graph *(Medium)*
DFS/BFS with a `unordered_map<Node*,Node*>` mapping originals → clones to handle cycles. Trap: create the clone *before* recursing into neighbors to break infinite loops.

### Q24. Word Ladder *(Hard)*
BFS over words where edges connect words differing by one letter → shortest transformation = fewest BFS levels. **O(N·L²).** Trap: build neighbors via wildcard patterns (`h*t`) for efficiency.

---

## 6.8 Dynamic Programming

### Q25. Climbing Stairs *(Easy)*
`dp[i] = dp[i-1] + dp[i-2]` — it's Fibonacci. **O(n)/O(1).**

### Q26. House Robber *(Medium)*
```cpp
int rob(const std::vector<int>& a) {
    int prev = 0, cur = 0;                        // rob[i-2], rob[i-1]
    for (int x : a) { int t = std::max(cur, prev + x); prev = cur; cur = t; }
    return cur;
}
```
**O(n)/O(1).** State: max loot up to house i, with/without robbing it.

### Q27. Coin Change *(Medium)*
Min coins for an amount — see [Ch 4.5](04_Algorithms.md#45-dynamic-programming-dp). **O(amount·coins).** Trap: initialize with an impossible-large value; return −1 if unreachable. Greedy fails here.

### Q28. Longest Increasing Subsequence *(Medium)*
```cpp
int lengthOfLIS(const std::vector<int>& a) {
    std::vector<int> tails;                       // tails[k] = smallest tail of LIS length k+1
    for (int x : a) {
        auto it = std::lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;                             // replace to keep tails minimal
    }
    return tails.size();
}
```
**O(n log n)** (patience sorting). The O(n²) DP is `dp[i]=max(dp[j]+1)` for `j<i, a[j]<a[i]`.

### Q29. Edit Distance *(Hard)*
2D DP: `dp[i][j]` = min ops to convert `a[0..i)`→`b[0..j)`. Transitions: match (diagonal), insert, delete, replace. **O(mn).** A canonical interview DP — know the recurrence.

### Q30. Word Break *(Medium)*
`dp[i]` = can `s[0..i)` be segmented? `dp[i] = OR over j of (dp[j] && s[j..i) in dict)`. **O(n²)** with a hash set of words.

---

## 6.9 Heap / Priority Queue

### Q31. Kth Largest Element *(Medium)*
Min-heap of size k (see [Ch 3.2](03_Advanced_Data_Structures.md#32-heaps)) → **O(n log k)**. Or `std::nth_element` → **O(n)** average. Trap: kth *largest* uses a *min*-heap.

### Q32. Merge K Sorted Lists *(Hard)*
Min-heap of the current heads; pop smallest, push its next. **O(N log k).**
```cpp
struct Cmp { bool operator()(ListNode* a, ListNode* b){ return a->val > b->val; } };
ListNode* mergeKLists(std::vector<ListNode*>& lists) {
    std::priority_queue<ListNode*, std::vector<ListNode*>, Cmp> pq;
    for (auto l : lists) if (l) pq.push(l);
    ListNode dummy(0); ListNode* tail = &dummy;
    while (!pq.empty()) {
        ListNode* n = pq.top(); pq.pop();
        tail->next = n; tail = n;
        if (n->next) pq.push(n->next);
    }
    return dummy.next;
}
```

### Q33. Find Median from Data Stream *(Hard)*
Two heaps: a max-heap for the lower half, a min-heap for the upper half, balanced in size. Median = top(s). `addNum` O(log n), `findMedian` O(1). Trap: keep sizes balanced and route elements correctly.

---

## 6.10 Edge Cases & Common Traps Checklist

Before saying "done", verify against these:

- **Empty input** (empty array/string/list, null root).
- **Single element.**
- **All identical / duplicates.**
- **Already sorted / reverse sorted** (worst case for quicksort, degenerate BST).
- **Negative numbers / zero** (especially with products, division, sums).
- **Integer overflow** — use `long long`; `mid = lo + (hi-lo)/2`.
- **Off-by-one** — inclusive vs exclusive bounds, `<` vs `<=`.
- **Null/dangling pointers** in linked lists and trees.
- **Cycles** in linked lists/graphs → infinite loops.
- **Disconnected graph** — loop over *all* vertices.
- **Min-heap vs max-heap** confusion (Dijkstra, kth largest).
- **Hash map `[]` auto-inserts** — use `.count()`/`.find()` to test.
- **Stack overflow** from deep recursion — consider iterative.
- **Modulo arithmetic** for large counts (`1e9+7`) — apply at every step.

---

## 6.11 Suggested Study Order (Pattern-First)

1. Arrays & Hashing → Two Pointers → Sliding Window
2. Stack → Linked List
3. Binary Search → Trees → Tries
4. Heap / Priority Queue
5. Backtracking
6. Graphs (BFS/DFS) → Advanced Graphs (Dijkstra, Union-Find, Topo)
7. 1-D DP → 2-D DP → Greedy
8. Intervals → Bit Manipulation → Math

> 💡 Do ~5–10 problems per pattern until you recognize it instantly. Depth per pattern beats breadth across random problems.

---

*Next →* [Chapter 7: Comparisons & Design Thinking](07_Comparisons_and_Design.md)
