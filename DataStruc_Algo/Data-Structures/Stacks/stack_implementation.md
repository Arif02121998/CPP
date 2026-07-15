# File: Stacks/stack_implementation.md

[← Repository Index](../README.md) · **Stacks:** [stack_basics](stack_basics.md) · stack_implementation · [monotonic_stack](monotonic_stack.md) · [applications](applications.md)

---

## Conceptual Understanding

**Definition.** This file covers **how to build a stack** from scratch on top of two different backing stores — a dynamic array and a linked list — plus useful augmented stacks (min-stack, two-stacks-in-one-array).

**Why it matters.** Implementing a stack teaches the array-vs-pointer tradeoff in miniature, and augmented stacks (like O(1) `getMin`) are frequent interview asks. Understanding the backing store explains the difference between *true* O(1) and *amortized* O(1).

**Real-world analogy.** Two ways to maintain a pile: stack physical items in a fixed-size bin that you replace with a bigger bin when full (array), or clip each new item onto the previous one (linked list).

---

## Internal Working

### Array-backed
- Keep `data` and a `top` index (−1 when empty).
- `push`: `data[++top] = x` (grow capacity when full — doubling → amortized O(1)).
- `pop`: return `data[top--]`.

```
data: [10][20][30][ ][ ]   top = 2
push(40): data[3]=40, top=3
pop():    returns data[3]=40, top=2
```

### Linked-list-backed
- Push/pop at the **head**; each node points to the one below it.
- Always true O(1); no resizing, but a pointer per element and cache misses.

```
top → [30] → [20] → [10] → null
```

---

## Operations — Step by Step

**push(x):**
- Array: if `top+1 == capacity`, reallocate to `2×`; then `data[++top] = x`.
- List: `n = new Node(x); n->next = head; head = n;`.

**pop():**
- Array: read `data[top]`, then `--top` (optionally shrink).
- List: `t = head; head = head->next; delete t;`.

**top():** return `data[top]` / `head->data`.

---

## Complexity

| Backing | push | pop | top | Memory overhead |
|---|---|---|---|---|
| Array (doubling) | O(1) amortized | O(1) | O(1) | low; up to ~2× capacity |
| Linked list | O(1) worst | O(1) | O(1) | pointer per node |

---

## C++ Implementation

### Array-backed (using std::vector for growth)
```cpp
#include <vector>
#include <stdexcept>

template <typename T>
class ArrayStack {
    std::vector<T> data;
public:
    void push(const T& x) { data.push_back(x); }      // amortized O(1)
    void pop() {
        if (data.empty()) throw std::out_of_range("pop on empty");
        data.pop_back();
    }
    T& top() {
        if (data.empty()) throw std::out_of_range("top on empty");
        return data.back();
    }
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};
```

### Linked-list-backed
```cpp
template <typename T>
class ListStack {
    struct Node { T val; Node* next; };
    Node* head = nullptr;
    size_t sz = 0;
public:
    ~ListStack() { while (head) pop(); }
    void push(const T& x) { head = new Node{ x, head }; ++sz; }   // O(1)
    void pop() {
        if (!head) throw std::out_of_range("pop on empty");
        Node* t = head; head = head->next; delete t; --sz;
    }
    T& top() {
        if (!head) throw std::out_of_range("top on empty");
        return head->val;
    }
    bool empty() const { return head == nullptr; }
    size_t size() const { return sz; }
};
```

**STL equivalent.** `std::stack<T>` wraps any of these. Customize the backing container:
```cpp
#include <stack>
std::stack<int, std::vector<int>> s;   // vector-backed adapter
```

**When to use STL vs custom.** Use `std::stack`. Hand-roll only for augmented behavior (min-stack), embedded constraints, or to demonstrate the mechanics.

---

## Augmented Stack — Min Stack (O(1) min)
```cpp
#include <stack>
#include <algorithm>
class MinStack {
    std::stack<int> s, mins;          // mins.top() = min of current stack
public:
    void push(int x){ s.push(x); mins.push(mins.empty()? x : std::min(x, mins.top())); }
    void pop(){ s.pop(); mins.pop(); }
    int top(){ return s.top(); }
    int getMin(){ return mins.top(); }
};
```
💡 The auxiliary stack stores the minimum *as of each push*, so `getMin` is O(1) without scanning.

## Augmented — Two Stacks in One Array
Grow one stack from the left, the other from the right; they meet in the middle. Maximizes space use for two stacks of complementary sizes.

---

## Tradeoffs
- **Array vs list:** array wins on cache locality and memory density; list wins on worst-case O(1) (no resize spikes) and never needs contiguous memory.
- **Performance vs memory:** array doubling can hold ~2× memory transiently; `shrink_to_fit` reclaims it.

---

## Use Cases
- Any LIFO need; the augmented min-stack is common in interviews and in monotonic-stack-style problems.

---

## Pitfalls
- ⚠️ Guard `pop`/`top` against empty (UB in `std::stack`).
- ⚠️ Memory leak in the list version if the destructor doesn't free nodes.
- ⚠️ Forgetting to keep the auxiliary `mins` stack in sync (push *and* pop together).
- ⚠️ Integer overflow in min/sum-tracking stacks.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Support getMin/getMax in O(1)" → **augmented (parallel) stack**.
- "Implement queue using stacks" / "stack using queues" → adapter composition (see [applications.md](applications.md)).

---

## Example Problems

### Easy — Implement Stack using a single vector. See `ArrayStack` above.

### Medium — Min Stack. See augmented implementation (O(1) all operations).

### Hard — Implement Queue using Two Stacks (amortized O(1))
```cpp
#include <stack>
class MyQueue {
    std::stack<int> in, out;
    void shift(){ if (out.empty()) while (!in.empty()){ out.push(in.top()); in.pop(); } }
public:
    void push(int x){ in.push(x); }
    int pop(){ shift(); int v = out.top(); out.pop(); return v; }
    int peek(){ shift(); return out.top(); }
    bool empty(){ return in.empty() && out.empty(); }
};
```
**Explanation.** Reversing through a second stack restores FIFO order; each element is moved at most once → **amortized O(1)** per operation.

---

*Next →* [monotonic_stack.md](monotonic_stack.md)
