<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>L E E T C O D E &nbsp; P A T T E R N S</samp>
</p>

<h1 align="center">📘 LeetCode Patterns — Coding Interview Playbook</h1>

<p align="center"><em>"Don't memorize solutions. Learn patterns."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Pattern | Jump |
|:-:|---------|:----:|
| 1 | Two Pointers | [§1](#-1-two-pointers) |
| 2 | Sliding Window | [§2](#-2-sliding-window) |
| 3 | Binary Search | [§3](#-3-binary-search) |
| 4 | Stack-Based | [§4](#-4-stack-based) |
| 5 | Linked List | [§5](#-5-linked-list) |
| 6 | BFS & DFS (Graphs/Trees) | [§6](#-6-bfs--dfs-graphstrees) |
| 7 | Dynamic Programming | [§7](#-7-dynamic-programming) |
| 8 | Bit Manipulation | [§8](#-8-bit-manipulation) |
| 9 | Heap / Priority Queue | [§9](#-9-heap--priority-queue) |
| 10 | Intervals | [§10](#-10-intervals) |
| 11 | Matrix Traversal | [§11](#-11-matrix-traversal) |
| 12 | Top-K / Sorting | [§12](#-12-top-k--sorting) |
| 13 | Trie | [§13](#-13-trie) |
| 14 | Practice Problem List | [§14](#-14-practice-problem-list) |

---

## 🔹 1. Two Pointers

### When to Use

- Array is **sorted** (or can be sorted)
- Need to find pairs/triplets that satisfy a condition
- Need to reverse, partition, or compare from both ends

### Pattern: Opposite-Direction Pointers

```cpp
// Two Sum (sorted array) — find pair summing to target
vector<int> twoSum(vector<int>& nums, int target) {
    int left = 0, right = nums.size() - 1;

    while (left < right) {
        int sum = nums[left] + nums[right];
        if (sum == target)       return {left, right};
        else if (sum < target)   left++;      // Need bigger sum
        else                     right--;     // Need smaller sum
    }
    return {};
}
// Time: O(n), Space: O(1)
```

### Pattern: Same-Direction Pointers (Fast/Slow)

```cpp
// Remove duplicates from sorted array — IN PLACE
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;

    for (int fast = 1; fast < nums.size(); fast++) {
        if (nums[fast] != nums[slow]) {
            slow++;
            nums[slow] = nums[fast];
        }
    }
    return slow + 1;    // Length of unique portion
}
// [1,1,2,3,3] → [1,2,3,_,_] returns 3
```

### Classic Problems

```cpp
// 3Sum — find all triplets summing to 0
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;

    for (int i = 0; i < (int)nums.size() - 2; i++) {
        if (i > 0 && nums[i] == nums[i-1]) continue;    // Skip duplicates

        int left = i + 1, right = nums.size() - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                while (left < right && nums[left] == nums[left+1]) left++;
                while (left < right && nums[right] == nums[right-1]) right--;
                left++; right--;
            }
            else if (sum < 0) left++;
            else right--;
        }
    }
    return result;
}
// Time: O(n²)
```

```cpp
// Container With Most Water
int maxArea(vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int maxWater = 0;

    while (left < right) {
        int water = min(height[left], height[right]) * (right - left);
        maxWater = max(maxWater, water);

        if (height[left] < height[right]) left++;
        else right--;
    }
    return maxWater;
}
```

---

## 🔹 2. Sliding Window

### When to Use

- Contiguous **subarray** or **substring** problems
- "Find min/max/count of subarray of size K" or "smallest subarray with condition"

### Pattern: Fixed-Size Window

```cpp
// Maximum sum of subarray of size K
int maxSumK(vector<int>& nums, int k) {
    int windowSum = 0;
    for (int i = 0; i < k; i++) windowSum += nums[i];   // First window

    int maxSum = windowSum;
    for (int i = k; i < nums.size(); i++) {
        windowSum += nums[i] - nums[i - k];    // Slide: add right, remove left
        maxSum = max(maxSum, windowSum);
    }
    return maxSum;
}
// Time: O(n)
```

### Pattern: Variable-Size Window

```cpp
// Smallest subarray with sum >= target
int minSubArrayLen(int target, vector<int>& nums) {
    int left = 0, sum = 0;
    int minLen = INT_MAX;

    for (int right = 0; right < nums.size(); right++) {
        sum += nums[right];                         // Expand window

        while (sum >= target) {                     // Shrink while valid
            minLen = min(minLen, right - left + 1);
            sum -= nums[left];
            left++;
        }
    }
    return minLen == INT_MAX ? 0 : minLen;
}
// Time: O(n) — each element added and removed at most once
```

### Classic: Longest Substring Without Repeating Characters

```cpp
int lengthOfLongestSubstring(string s) {
    unordered_set<char> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < s.size(); right++) {
        while (window.count(s[right])) {       // Duplicate found
            window.erase(s[left]);             // Shrink from left
            left++;
        }
        window.insert(s[right]);
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}
// "abcabcbb" → 3 ("abc")
```

### Classic: Minimum Window Substring

```cpp
string minWindow(string s, string t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;

    int left = 0, matched = 0;
    int minLen = INT_MAX, minStart = 0;

    for (int right = 0; right < s.size(); right++) {
        char c = s[right];
        window[c]++;
        if (need.count(c) && window[c] == need[c]) matched++;

        while (matched == need.size()) {
            if (right - left + 1 < minLen) {
                minLen = right - left + 1;
                minStart = left;
            }
            char d = s[left];
            if (need.count(d) && window[d] == need[d]) matched--;
            window[d]--;
            left++;
        }
    }
    return minLen == INT_MAX ? "" : s.substr(minStart, minLen);
}
// s = "ADOBECODEBANC", t = "ABC" → "BANC"
```

---

## 🔹 3. Binary Search

### When to Use

- Sorted array / search space
- "Find minimum that satisfies condition" → binary search on answer

### Pattern: Standard Binary Search

```cpp
int binarySearch(vector<int>& nums, int target) {
    int lo = 0, hi = nums.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] == target) return mid;
        if (nums[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}
```

### Pattern: Lower Bound (First occurrence ≥ target)

```cpp
int lowerBound(vector<int>& nums, int target) {
    int lo = 0, hi = nums.size();    // Note: hi = size (not size-1)
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] < target) lo = mid + 1;
        else hi = mid;               // nums[mid] >= target
    }
    return lo;    // First index where nums[i] >= target
}
```

### Pattern: Binary Search on Answer

```cpp
// Koko eating bananas — minimum speed to finish in H hours
int minEatingSpeed(vector<int>& piles, int h) {
    int lo = 1, hi = *max_element(piles.begin(), piles.end());

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;

        // Can Koko finish at speed 'mid' within h hours?
        long hours = 0;
        for (int p : piles) hours += (p + mid - 1) / mid;  // ceil division

        if (hours <= h) hi = mid;      // Feasible → try smaller
        else lo = mid + 1;             // Too slow → need faster
    }
    return lo;
}
```

### Classic: Search in Rotated Sorted Array

```cpp
int search(vector<int>& nums, int target) {
    int lo = 0, hi = nums.size() - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] == target) return mid;

        if (nums[lo] <= nums[mid]) {           // Left half is sorted
            if (nums[lo] <= target && target < nums[mid])
                hi = mid - 1;
            else
                lo = mid + 1;
        } else {                                // Right half is sorted
            if (nums[mid] < target && target <= nums[hi])
                lo = mid + 1;
            else
                hi = mid - 1;
        }
    }
    return -1;
}
```

---

## 🔹 4. Stack-Based

### When to Use

- **Matching** (parentheses, tags, brackets)
- **Monotonic stack** (next greater/smaller element)
- **Evaluation** (expressions, calculators)

### Pattern: Valid Parentheses

```cpp
bool isValid(string s) {
    stack<char> st;
    unordered_map<char,char> match = {{')', '('}, {']', '['}, {'}', '{'}};

    for (char c : s) {
        if (match.count(c)) {
            if (st.empty() || st.top() != match[c]) return false;
            st.pop();
        } else {
            st.push(c);
        }
    }
    return st.empty();
}
```

### Pattern: Monotonic Stack (Next Greater Element)

```cpp
// For each element, find the next greater element to its right
vector<int> nextGreater(vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> st;    // Stores INDICES

    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[i] > nums[st.top()]) {
            result[st.top()] = nums[i];
            st.pop();
        }
        st.push(i);
    }
    return result;
}
// [2, 1, 2, 4, 3] → [4, 2, 4, -1, -1]
// Time: O(n) — each element pushed and popped at most once
```

### Classic: Largest Rectangle in Histogram

```cpp
int largestRectangleArea(vector<int>& heights) {
    stack<int> st;
    int maxArea = 0;
    heights.push_back(0);    // Sentinel to flush stack

    for (int i = 0; i < heights.size(); i++) {
        while (!st.empty() && heights[i] < heights[st.top()]) {
            int h = heights[st.top()]; st.pop();
            int w = st.empty() ? i : (i - st.top() - 1);
            maxArea = max(maxArea, h * w);
        }
        st.push(i);
    }
    return maxArea;
}
```

### Classic: Min Stack (O(1) getMin)

```cpp
class MinStack {
    stack<pair<int,int>> st;    // {value, current_min}
public:
    void push(int val) {
        int curMin = st.empty() ? val : min(val, st.top().second);
        st.push({val, curMin});
    }
    void pop()     { st.pop(); }
    int top()      { return st.top().first; }
    int getMin()   { return st.top().second; }
};
```

---

## 🔹 5. Linked List

### Pattern: Fast/Slow Pointers

```cpp
// Find middle of linked list
ListNode* middleNode(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;    // When fast reaches end, slow is at middle
}

// Detect cycle (Floyd's algorithm)
bool hasCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}
```

### Pattern: Reverse a Linked List

```cpp
ListNode* reverseList(ListNode* head) {
    ListNode *prev = nullptr, *curr = head;
    while (curr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}
```

### Classic: Merge Two Sorted Lists

```cpp
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (l1 && l2) {
        if (l1->val <= l2->val) { tail->next = l1; l1 = l1->next; }
        else                    { tail->next = l2; l2 = l2->next; }
        tail = tail->next;
    }
    tail->next = l1 ? l1 : l2;
    return dummy.next;
}
```

### Classic: Reverse Nodes in K-Group

```cpp
ListNode* reverseKGroup(ListNode* head, int k) {
    // Count if we have k nodes
    ListNode* check = head;
    for (int i = 0; i < k; i++) {
        if (!check) return head;    // Less than k nodes — don't reverse
        check = check->next;
    }

    // Reverse k nodes
    ListNode *prev = nullptr, *curr = head;
    for (int i = 0; i < k; i++) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    // head is now the tail of the reversed portion
    head->next = reverseKGroup(curr, k);    // Recurse on remaining
    return prev;
}
```

---

## 🔹 6. BFS & DFS (Graphs/Trees)

### Pattern: BFS (Level-Order / Shortest Path)

```cpp
// Binary tree level-order traversal
vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};
    vector<vector<int>> result;
    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int size = q.size();
        vector<int> level;

        for (int i = 0; i < size; i++) {
            TreeNode* node = q.front(); q.pop();
            level.push_back(node->val);
            if (node->left)  q.push(node->left);
            if (node->right) q.push(node->right);
        }
        result.push_back(level);
    }
    return result;
}
```

### Pattern: DFS (Recursive / Stack)

```cpp
// Maximum depth of binary tree
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(maxDepth(root->left), maxDepth(root->right));
}

// Path sum — does any root-to-leaf path sum to target?
bool hasPathSum(TreeNode* root, int target) {
    if (!root) return false;
    if (!root->left && !root->right) return target == root->val;
    return hasPathSum(root->left, target - root->val)
        || hasPathSum(root->right, target - root->val);
}
```

### Pattern: Graph BFS (Shortest Path in Unweighted Graph)

```cpp
// Number of islands (grid)
int numIslands(vector<vector<char>>& grid) {
    int count = 0;
    int m = grid.size(), n = grid[0].size();
    int dx[] = {0,0,1,-1}, dy[] = {1,-1,0,0};

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') {
                count++;
                // BFS to mark entire island
                queue<pair<int,int>> q;
                q.push({i, j});
                grid[i][j] = '0';

                while (!q.empty()) {
                    auto [x, y] = q.front(); q.pop();
                    for (int d = 0; d < 4; d++) {
                        int nx = x + dx[d], ny = y + dy[d];
                        if (nx >= 0 && nx < m && ny >= 0 && ny < n
                            && grid[nx][ny] == '1') {
                            grid[nx][ny] = '0';
                            q.push({nx, ny});
                        }
                    }
                }
            }
        }
    }
    return count;
}
```

### Pattern: Topological Sort (DAG)

```cpp
// Course Schedule — can you finish all courses?
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    vector<vector<int>> adj(numCourses);
    vector<int> indegree(numCourses, 0);

    for (auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]);
        indegree[p[0]]++;
    }

    queue<int> q;
    for (int i = 0; i < numCourses; i++)
        if (indegree[i] == 0) q.push(i);

    int completed = 0;
    while (!q.empty()) {
        int course = q.front(); q.pop();
        completed++;
        for (int next : adj[course]) {
            if (--indegree[next] == 0) q.push(next);
        }
    }
    return completed == numCourses;
}
```

---

## 🔹 7. Dynamic Programming

### The Framework

```
1. Define STATE: dp[i] = what does this represent?
2. Find TRANSITION: dp[i] depends on which previous states?
3. Set BASE CASE: dp[0] = ?
4. Determine ORDER: Fill left→right? Bottom→up?
5. Return ANSWER: dp[n]? dp[n-1]? max(dp)?
```

### Pattern: 1D DP

```cpp
// Climbing Stairs — 1 or 2 steps at a time
int climbStairs(int n) {
    if (n <= 2) return n;
    int prev2 = 1, prev1 = 2;
    for (int i = 3; i <= n; i++) {
        int curr = prev1 + prev2;    // dp[i] = dp[i-1] + dp[i-2]
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

// House Robber — max money, can't rob adjacent houses
int rob(vector<int>& nums) {
    int prev2 = 0, prev1 = 0;
    for (int num : nums) {
        int curr = max(prev1, prev2 + num);   // Skip or rob
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

### Pattern: 2D DP

```cpp
// Longest Common Subsequence
int longestCommonSubsequence(string s1, string s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;        // Match
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]); // Skip one
        }
    }
    return dp[m][n];
}
// "abcde", "ace" → 3 ("ace")
```

### Pattern: 0/1 Knapsack

```cpp
// Given items with weight[] and value[], max value within capacity W
int knapsack(vector<int>& weight, vector<int>& value, int W) {
    int n = weight.size();
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            dp[i][w] = dp[i-1][w];                           // Don't take item i
            if (weight[i-1] <= w)
                dp[i][w] = max(dp[i][w],
                    dp[i-1][w - weight[i-1]] + value[i-1]);  // Take item i
        }
    }
    return dp[n][W];
}
```

### Classic: Coin Change

```cpp
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, amount + 1);    // "infinity"
    dp[0] = 0;

    for (int i = 1; i <= amount; i++) {
        for (int coin : coins) {
            if (coin <= i)
                dp[i] = min(dp[i], dp[i - coin] + 1);
        }
    }
    return dp[amount] > amount ? -1 : dp[amount];
}
// coins = [1,5,11], amount = 15 → 3 (5+5+5)
```

---

## 🔹 8. Bit Manipulation

### Essential Tricks

```cpp
// Check if n is power of 2
bool isPowerOf2(int n) { return n > 0 && (n & (n - 1)) == 0; }

// Count set bits (Kernighan's algorithm)
int countBits(int n) {
    int count = 0;
    while (n) {
        n &= (n - 1);    // Clears lowest set bit
        count++;
    }
    return count;
}

// Get ith bit
bool getBit(int n, int i) { return (n >> i) & 1; }

// Set ith bit
int setBit(int n, int i) { return n | (1 << i); }

// Clear ith bit
int clearBit(int n, int i) { return n & ~(1 << i); }

// Toggle ith bit
int toggleBit(int n, int i) { return n ^ (1 << i); }

// Swap without temp
void swap(int& a, int& b) { a ^= b; b ^= a; a ^= b; }
```

### Classic: Single Number (every element appears twice except one)

```cpp
int singleNumber(vector<int>& nums) {
    int result = 0;
    for (int n : nums) result ^= n;    // XOR cancels duplicates
    return result;
}
// [4, 1, 2, 1, 2] → 4
```

### Classic: Reverse Bits

```cpp
uint32_t reverseBits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}
```

---

## 🔹 9. Heap / Priority Queue

### When to Use

- **Top K** elements
- **Kth largest/smallest**
- **Merge K sorted** lists/arrays
- **Median** of a data stream

### Pattern: Kth Largest Element

```cpp
int findKthLargest(vector<int>& nums, int k) {
    // Min-heap of size K — top is the Kth largest
    priority_queue<int, vector<int>, greater<int>> minHeap;

    for (int n : nums) {
        minHeap.push(n);
        if (minHeap.size() > k) minHeap.pop();
    }
    return minHeap.top();
}
// Time: O(n log k)
```

### Pattern: Merge K Sorted Lists

```cpp
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);

    for (auto* l : lists)
        if (l) pq.push(l);

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (!pq.empty()) {
        ListNode* node = pq.top(); pq.pop();
        tail->next = node;
        tail = tail->next;
        if (node->next) pq.push(node->next);
    }
    return dummy.next;
}
// Time: O(N log K) where N = total nodes, K = number of lists
```

### Pattern: Median from Data Stream (Two Heaps)

```cpp
class MedianFinder {
    priority_queue<int> maxHeap;                              // Left half (larger values on top)
    priority_queue<int, vector<int>, greater<int>> minHeap;   // Right half (smaller values on top)

public:
    void addNum(int num) {
        maxHeap.push(num);
        minHeap.push(maxHeap.top());
        maxHeap.pop();

        if (minHeap.size() > maxHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size())
            return maxHeap.top();
        return (maxHeap.top() + minHeap.top()) / 2.0;
    }
};
```

---

## 🔹 10. Intervals

### Pattern: Merge Overlapping Intervals

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> merged;

    for (auto& iv : intervals) {
        if (merged.empty() || merged.back()[1] < iv[0])
            merged.push_back(iv);                    // No overlap
        else
            merged.back()[1] = max(merged.back()[1], iv[1]);  // Merge
    }
    return merged;
}
// [[1,3],[2,6],[8,10],[15,18]] → [[1,6],[8,10],[15,18]]
```

### Pattern: Insert Interval

```cpp
vector<vector<int>> insert(vector<vector<int>>& intervals, vector<int>& newIv) {
    vector<vector<int>> result;
    int i = 0, n = intervals.size();

    // Add all before newInterval
    while (i < n && intervals[i][1] < newIv[0])
        result.push_back(intervals[i++]);

    // Merge overlapping
    while (i < n && intervals[i][0] <= newIv[1]) {
        newIv[0] = min(newIv[0], intervals[i][0]);
        newIv[1] = max(newIv[1], intervals[i][1]);
        i++;
    }
    result.push_back(newIv);

    // Add all after
    while (i < n)
        result.push_back(intervals[i++]);

    return result;
}
```

---

## 🔹 11. Matrix Traversal

### Pattern: Spiral Order

```cpp
vector<int> spiralOrder(vector<vector<int>>& matrix) {
    vector<int> result;
    int top = 0, bottom = matrix.size() - 1;
    int left = 0, right = matrix[0].size() - 1;

    while (top <= bottom && left <= right) {
        for (int j = left; j <= right; j++) result.push_back(matrix[top][j]);
        top++;
        for (int i = top; i <= bottom; i++) result.push_back(matrix[i][right]);
        right--;
        if (top <= bottom)
            for (int j = right; j >= left; j--) result.push_back(matrix[bottom][j]);
        bottom--;
        if (left <= right)
            for (int i = bottom; i >= top; i--) result.push_back(matrix[i][left]);
        left++;
    }
    return result;
}
```

### Pattern: Rotate Matrix 90° Clockwise

```cpp
void rotate(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // Step 1: Transpose
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            swap(matrix[i][j], matrix[j][i]);
    // Step 2: Reverse each row
    for (auto& row : matrix)
        reverse(row.begin(), row.end());
}
```

---

## 🔹 12. Top-K / Sorting

### Pattern: Bucket Sort (Top K Frequent)

```cpp
vector<int> topKFrequent(vector<int>& nums, int k) {
    unordered_map<int, int> freq;
    for (int n : nums) freq[n]++;

    // Bucket: index = frequency, value = list of numbers with that frequency
    vector<vector<int>> buckets(nums.size() + 1);
    for (auto& [num, f] : freq) buckets[f].push_back(num);

    vector<int> result;
    for (int i = buckets.size() - 1; i >= 0 && result.size() < k; i--)
        for (int n : buckets[i])
            if (result.size() < k) result.push_back(n);

    return result;
}
// Time: O(n)
```

---

## 🔹 13. Trie

### Pattern: Prefix Tree

```cpp
class Trie {
    struct Node {
        Node* children[26] = {};
        bool isEnd = false;
    };
    Node* root;

public:
    Trie() : root(new Node()) {}

    void insert(const string& word) {
        Node* node = root;
        for (char c : word) {
            int i = c - 'a';
            if (!node->children[i]) node->children[i] = new Node();
            node = node->children[i];
        }
        node->isEnd = true;
    }

    bool search(const string& word) {
        Node* node = find(word);
        return node && node->isEnd;
    }

    bool startsWith(const string& prefix) {
        return find(prefix) != nullptr;
    }

private:
    Node* find(const string& s) {
        Node* node = root;
        for (char c : s) {
            int i = c - 'a';
            if (!node->children[i]) return nullptr;
            node = node->children[i];
        }
        return node;
    }
};
```

---

## 🔹 14. Practice Problem List

### Must-Do Problems by Pattern

| Pattern | Easy | Medium | Hard |
|---------|------|--------|------|
| **Two Pointers** | Two Sum II, Move Zeroes | 3Sum, Container With Most Water | Trapping Rain Water |
| **Sliding Window** | Max Avg Subarray | Longest Substring No Repeat, Min Size Subarray | Min Window Substring |
| **Binary Search** | Search Insert Position | Search Rotated Array, Find Peak | Median of Two Sorted Arrays |
| **Stack** | Valid Parentheses, Min Stack | Daily Temperatures, Eval RPN | Largest Rectangle Histogram |
| **Linked List** | Reverse, Merge Two, Detect Cycle | Remove Nth Node, Reorder List | Reverse K-Group |
| **Tree/BFS/DFS** | Max Depth, Same Tree | Level Order, Validate BST | Serialize/Deserialize |
| **Graph** | Flood Fill | Num Islands, Course Schedule | Word Ladder |
| **DP** | Climbing Stairs, House Robber | Coin Change, LIS, LCS | Edit Distance |
| **Bit Manipulation** | Single Number, Power of 2 | Counting Bits | — |
| **Heap** | Kth Largest in Stream | Top K Frequent, K Closest | Merge K Sorted Lists, Median Stream |
| **Intervals** | — | Merge Intervals, Insert Interval | — |
| **Trie** | — | Implement Trie | Word Search II |

### Suggested Order (4-Week Plan)

```
Week 1: Two Pointers + Sliding Window + Binary Search (15 problems)
Week 2: Stack + Linked List + Tree/BFS/DFS (15 problems)
Week 3: DP (15 problems — start easy, build up)
Week 4: Graph + Heap + Bit + Intervals + Trie (15 problems)
```

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 LeetCode Patterns — From Brute Force to Optimal</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
