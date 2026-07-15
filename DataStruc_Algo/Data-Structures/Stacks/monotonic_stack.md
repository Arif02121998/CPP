# File: Stacks/monotonic_stack.md

[← Repository Index](../README.md) · **Stacks:** [stack_basics](stack_basics.md) · [stack_implementation](stack_implementation.md) · monotonic_stack · [applications](applications.md)

---

## Conceptual Understanding

**Definition.** A **monotonic stack** maintains its elements in sorted (strictly/weakly increasing or decreasing) order. Before pushing a new element, you **pop everything that violates the order**. The elements you pop are exactly those for which the new element is the answer to a "next greater/smaller" question.

**Why it exists.** Many problems ask, for each element, "what is the nearest element to the left/right that is greater/smaller?" The brute force is O(n²) nested scanning. A monotonic stack answers all of them in a **single O(n) pass** because each element is pushed and popped at most once.

**Problem it solves.** Next/previous greater/smaller element, stock spans, largest rectangle in a histogram, trapping rain water, and many "as soon as a bigger value appears, resolve the pending ones" patterns.

**Real-world analogy.** People queuing by height where anyone shorter than a newcomer steps aside: when a tall person arrives, all the shorter people ahead "find their taller neighbor."

---

## Internal Working

Keep a stack of **indices** (often more useful than values, since you can recover distances).

**Decreasing stack** (top is the smallest) → finds **next greater element**:
- While the new value is **greater** than the value at the top index, the new value is that popped index's "next greater." Pop and record. Then push the new index.

```
arr = [2, 1, 2, 4, 3]   (find next greater to the right)

i=0 push 0            stack(idx)=[0]            (vals: 2)
i=1 1<2, push 1       stack=[0,1]               (2,1)
i=2 2>1 pop1 ans[1]=2; 2==2 push2  stack=[0,2] (2,2)
i=3 4>2 pop2 ans[2]=4; 4>2 pop0 ans[0]=4; push3 stack=[3]  (4)
i=4 3<4 push4         stack=[3,4]               (4,3)
remaining have no next greater → -1
ans = [4, 2, 4, -1, -1]
```

💡 **Direction & comparison rules:**
- **Next greater** → traverse left→right, maintain **decreasing** stack, pop on `>`.
- **Next smaller** → maintain **increasing** stack, pop on `<`.
- **Previous greater/smaller** → traverse right→left (or read what's left on the stack when you push).

---

## Operations
- `while (!st.empty() && violates(st.top(), cur)) pop and resolve;`
- `push(cur index)`.
Each index enters and leaves the stack once → **O(n)** total.

---

## Complexity

| | Time | Space |
|---|---|---|
| Build all answers | O(n) | O(n) stack |
| Per-element amortized | O(1) | — |

Although there's an inner `while`, the total number of pops across the whole run is ≤ n, so it's linear — a classic **amortized** argument.

---

## C++ Implementation — Next Greater Element
```cpp
#include <vector>
#include <stack>

// For each i, the value of the next strictly greater element to the right (-1 if none).
std::vector<int> nextGreater(const std::vector<int>& a) {
    int n = a.size();
    std::vector<int> ans(n, -1);
    std::stack<int> st;                       // stores indices, values decreasing
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && a[i] > a[st.top()]) {
            ans[st.top()] = a[i];             // a[i] resolves the popped index
            st.pop();
        }
        st.push(i);
    }
    return ans;                               // indices left in st have no greater element
}
```

**STL note.** Use `std::stack<int>` (or a `std::vector<int>` as a manual stack for speed). There is no dedicated STL monotonic stack — it's a technique, not a container.

---

## Tradeoffs
- ✅ Turns O(n²) scanning into O(n).
- ✅ One template solves a whole family of "nearest greater/smaller" problems.
- ❌ Requires care with strict vs non-strict comparisons and traversal direction.
- ❌ Stack stores indices; need to map back to values/distances.

---

## Use Cases
- Next/previous greater/smaller element.
- Stock span / daily temperatures (distance to next warmer day).
- Largest rectangle in a histogram; maximal rectangle in a binary matrix.
- Trapping rain water (stack variant).
- Remove K digits / build smallest number (greedy with monotonic stack).

---

## Pitfalls
- ⚠️ **Strict vs non-strict** (`>` vs `>=`) changes whether equal elements are popped — pick based on the problem (duplicates, "next greater or equal").
- ⚠️ Wrong **direction**: next-greater-right needs left→right; previous-greater needs the mirror.
- ⚠️ Storing values when you actually need **indices** (for distances/widths).
- ⚠️ Forgetting to resolve the indices still on the stack at the end (they have no answer → default/-1).

---

## Problem Patterns

🎯 **Recognition hints:**
- "For each element, find the next/previous greater/smaller" → **monotonic stack**.
- "Largest rectangle / maximal area under bars" → **monotonic (increasing) stack of indices**.
- "Span," "days until," "warmer temperature" → **monotonic stack**.
- "Build lexicographically smallest/largest by removing k" → **greedy monotonic stack**.

---

## Example Problems

### Easy — Daily Temperatures
For each day, how many days until a warmer temperature?
```cpp
std::vector<int> dailyTemperatures(const std::vector<int>& t) {
    int n = t.size();
    std::vector<int> ans(n, 0);
    std::stack<int> st;                       // decreasing temps, store indices
    for (int i = 0; i < n; ++i) {
        while (!st.empty() && t[i] > t[st.top()]) {
            ans[st.top()] = i - st.top();     // distance to warmer day
            st.pop();
        }
        st.push(i);
    }
    return ans;
}
```
**Explanation.** Indices wait until a warmer day pops them; the index difference is the wait. O(n).

### Medium — Next Greater Element II (circular array)
Iterate `2n` times using `i % n` so wrap-around neighbors are considered.
```cpp
std::vector<int> nextGreaterCircular(const std::vector<int>& a) {
    int n = a.size();
    std::vector<int> ans(n, -1);
    std::stack<int> st;
    for (int i = 0; i < 2*n; ++i) {
        int v = a[i % n];
        while (!st.empty() && v > a[st.top()]) { ans[st.top()] = v; st.pop(); }
        if (i < n) st.push(i);                // only push real indices
    }
    return ans;
}
```
**Explanation.** Two passes simulate the circle; guard pushes to the first pass.

### Hard — Largest Rectangle in Histogram
**Brute force:** for each bar expand left/right → O(n²).
**Optimized (monotonic increasing stack):** O(n).
```cpp
int largestRectangleArea(std::vector<int> h) {
    h.push_back(0);                           // sentinel flushes the stack
    std::stack<int> st;                       // increasing heights, indices
    int best = 0;
    for (int i = 0; i < (int)h.size(); ++i) {
        while (!st.empty() && h[i] < h[st.top()]) {
            int height = h[st.top()]; st.pop();
            int left = st.empty() ? -1 : st.top();
            int width = i - left - 1;
            best = std::max(best, height * width);
        }
        st.push(i);
    }
    return best;
}
```
**Explanation.** When a shorter bar appears, every taller bar's maximal rectangle ends here; width spans from the previous shorter bar (`left`) to `i`. The sentinel `0` forces all remaining bars to resolve.

---

*Next →* [applications.md](applications.md)
