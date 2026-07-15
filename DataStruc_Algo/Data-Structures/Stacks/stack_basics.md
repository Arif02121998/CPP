# File: Stacks/stack_basics.md

[← Repository Index](../README.md) · **Stacks:** stack_basics · [stack_implementation](stack_implementation.md) · [monotonic_stack](monotonic_stack.md) · [applications](applications.md)

---

## Conceptual Understanding

**Definition.** A stack is a **LIFO** (Last-In, First-Out) collection where insertion and removal happen at a single end — the **top**. The most recently added element is the first removed.

**Why it exists.** A huge class of problems is inherently "handle the most recent thing first": undo, function calls, bracket matching, backtracking. Restricting all access to one end makes every operation **O(1)** and the logic trivially correct.

**Problem it solves.** Reversal, nested-structure validation, expression evaluation, depth-first exploration, and the function-call mechanism itself.

**Real-world analogy.** A stack of plates or a pile of books: you add to the top and take from the top. To reach a lower plate you must remove the ones above it.

---

## Internal Working

A stack is a **discipline**, not a specific layout — it can sit on an array or a linked list. The defining rule: insertion (`push`) and deletion (`pop`) touch only the top.

```
push(10), push(20), push(30):

  top → | 30 |
        | 20 |
        | 10 |
        +----+

pop() → returns 30, top now points to 20
```

**Array-backed** keeps a `top` index; `push` writes `data[++top]`, `pop` returns `data[top--]`. Cache-friendly, amortized growth.
**Linked-list-backed** pushes/pops at the head; always O(1), no resizing, but pointer overhead.

(Implementation details: [stack_implementation.md](stack_implementation.md).)

---

## Operations (all O(1))

| Operation | Meaning |
|---|---|
| `push(x)` | add `x` on top |
| `pop()` | remove the top |
| `top()` / `peek()` | read the top without removing |
| `empty()` | is it empty? |
| `size()` | number of elements |

Step-by-step `push/pop` simply move the top reference; no other element is touched, hence O(1).

---

## Complexity

| Operation | Time | Space |
|---|---|---|
| push / pop / top / empty / size | O(1) | — |
| Search arbitrary element | O(n) | — |
| Overall | | O(n) |

(Array-backed `push` is amortized O(1) due to doubling; linked-backed is true O(1).)

---

## C++ Implementation

```cpp
#include <stack>

std::stack<int> s;
s.push(10);
s.push(20);
int t = s.top();   // 20  (read)
s.pop();           // removes 20 (returns void!)
bool e = s.empty();
size_t n = s.size();
```

**STL equivalent.** `std::stack<T, Container>` is an *adapter* over an underlying container (default `std::deque`; can use `std::vector` or `std::list`).
```cpp
std::stack<int, std::vector<int>> sv;   // vector-backed (cache-friendly)
```

**When to use STL vs custom.** Use `std::stack` virtually always. Build a custom stack only to learn or to add behavior (e.g., a min-stack tracking the minimum in O(1) — see [stack_implementation.md](stack_implementation.md)).

---

## Tradeoffs
- ✅ All operations O(1); trivial to reason about; array-backed is cache-friendly.
- ❌ Only the top is accessible — no random access or search without popping.
- **Simplicity vs flexibility:** maximally simple, but deliberately restrictive.

---

## Use Cases
- The **function call stack** (the runtime manages one for you — recursion uses it).
- Undo/redo, browser back button.
- Bracket/parenthesis matching, syntax parsing, compilers.
- Expression conversion/evaluation (infix↔postfix).
- Iterative DFS, backtracking state.
- Monotonic-stack problems (next greater element).

---

## Pitfalls
- ⚠️ **`top()`/`pop()` on an empty stack is undefined behavior** with `std::stack` — always guard with `empty()`.
- ⚠️ `std::stack::pop()` returns **void**; read `top()` first, then `pop()`.
- ⚠️ **Stack overflow** from unbounded recursion — the call stack is finite (~1–8 MB).
- ⚠️ Using a stack where order should be FIFO (use a [queue](../Queues/queue.md) instead).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Matching / nested / balanced" (brackets, tags) → **stack**.
- "Undo / most recent first" → **stack**.
- "Evaluate / parse expression" → **stack**.
- "Next greater/smaller element," "histogram," "span" → [monotonic stack](monotonic_stack.md).
- "Convert recursion to iteration" → explicit **stack**.

---

## Example Problems

### Easy — Valid Parentheses
**Optimized:** push openers, match each closer against the top → O(n)/O(n).
```cpp
#include <stack>
#include <unordered_map>
bool isValid(const std::string& s) {
    std::stack<char> st;
    std::unordered_map<char,char> match{{')','('},{']','['},{'}','{'}};
    for (char c : s) {
        if (c=='('||c=='['||c=='{') st.push(c);
        else { if (st.empty() || st.top()!=match[c]) return false; st.pop(); }
    }
    return st.empty();
}
```
**Explanation.** A closer is valid only if it matches the most recent opener (the top); leftover openers mean it's unbalanced.

### Medium — Evaluate Reverse Polish Notation
**Optimized:** push operands; on an operator, pop two, compute, push back → O(n).
```cpp
int evalRPN(const std::vector<std::string>& tokens) {
    std::stack<long long> st;
    for (const auto& t : tokens) {
        if (t=="+"||t=="-"||t=="*"||t=="/") {
            long long b = st.top(); st.pop();
            long long a = st.top(); st.pop();
            st.push(t=="+"?a+b : t=="-"?a-b : t=="*"?a*b : a/b);
        } else st.push(std::stoll(t));
    }
    return (int)st.top();
}
```
**Explanation.** Postfix evaluation is the canonical stack use — operands wait on the stack until their operator arrives.

### Hard — Min Stack (O(1) getMin)
Track the running minimum alongside each push.
```cpp
class MinStack {
    std::stack<int> s, mins;
public:
    void push(int x){ s.push(x); mins.push(mins.empty()? x : std::min(x, mins.top())); }
    void pop(){ s.pop(); mins.pop(); }
    int top(){ return s.top(); }
    int getMin(){ return mins.top(); }      // O(1)
};
```
**Explanation.** A parallel stack stores the minimum *as of* each element, so the current minimum is always its top.

---

*Next →* [stack_implementation.md](stack_implementation.md)
