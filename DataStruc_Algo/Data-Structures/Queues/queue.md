# File: Queues/queue.md

[← Repository Index](../README.md) · **Queues:** queue · [deque](deque.md) · [priority_queue](priority_queue.md) · [monotonic_queue](monotonic_queue.md)

---

## Conceptual Understanding

**Definition.** A queue is a **FIFO** (First-In, First-Out) collection: elements are inserted at the **back** (enqueue) and removed from the **front** (dequeue). The first element added is the first removed.

**Why it exists.** Many systems must process items **in arrival order** — fairness, scheduling, breadth-first exploration. A queue enforces that order with O(1) operations at both ends.

**Problem it solves.** Order-preserving buffering between producers and consumers, level-by-level traversal (BFS), scheduling, and rate-limiting.

**Real-world analogy.** A line at a ticket counter: people are served in the order they arrived; new arrivals join the back.

---

## Internal Working

A naive array queue that shifts every element on dequeue is O(n). The standard fixes:

**Circular buffer (ring).** Fixed array with `front` and `rear` indices that wrap with modulo. O(1) enqueue/dequeue, no shifting.
```
capacity 5:  [ _ , B , C , D , _ ]
              0   1   2   3   4
front=1 (B), rear=4 (next free). enqueue wraps rear → 0 when it passes the end.
```

**Linked list.** Keep `head` (front) and `tail` (back) pointers; enqueue at tail, dequeue at head — both O(1).

**Two stacks.** An `in` stack and an `out` stack; amortized O(1) (see [../Stacks/stack_implementation.md](../Stacks/stack_implementation.md)).

`std::queue` adapts a `std::deque` by default — O(1) at both ends, dynamically sized.

---

## Operations (all O(1))

| Operation | Meaning |
|---|---|
| `push(x)` / enqueue | add `x` at the back |
| `pop()` / dequeue | remove from the front |
| `front()` | read the front |
| `back()` | read the back |
| `empty()`, `size()` | status |

---

## Complexity

| Operation | Time | Space |
|---|---|---|
| enqueue / dequeue / front / back | O(1) | — |
| search | O(n) | — |
| Overall | | O(n) |

---

## C++ Implementation

```cpp
#include <queue>

std::queue<int> q;
q.push(10);          // enqueue
q.push(20);
int f = q.front();   // 10  (read front)
int b = q.back();    // 20  (read back)
q.pop();             // dequeue 10 (returns void)
bool e = q.empty();
size_t n = q.size();
```

**Custom circular-buffer queue:**
```cpp
#include <vector>
#include <stdexcept>
template <typename T>
class CircularQueue {
    std::vector<T> buf;
    int head = 0, count = 0;
public:
    explicit CircularQueue(int cap) : buf(cap) {}
    bool full()  const { return count == (int)buf.size(); }
    bool empty() const { return count == 0; }
    void push(const T& x) {
        if (full()) throw std::overflow_error("queue full");
        buf[(head + count) % buf.size()] = x; ++count;
    }
    void pop() {
        if (empty()) throw std::underflow_error("queue empty");
        head = (head + 1) % buf.size(); --count;
    }
    T& front() {
        if (empty()) throw std::underflow_error("queue empty");
        return buf[head];
    }
    int size() const { return count; }
};
```

**STL equivalent.** `std::queue<T, Container>` (default `std::deque`). Use `std::list` as the container if you need stable references; never `std::vector` (no efficient `pop_front`).

**When to use STL vs custom.** Use `std::queue`. Hand-roll a circular buffer only for fixed-capacity, lock-free, or embedded scenarios.

---

## Tradeoffs
- ✅ O(1) order-preserving insertion/removal.
- ❌ No random access or search; only ends are reachable.
- **Circular buffer vs list:** ring is cache-friendly and fixed-capacity; list grows dynamically with pointer overhead.

---

## Use Cases
- **BFS** on graphs/trees (see [../Graphs/bfs.md](../Graphs/bfs.md)).
- CPU/task scheduling, print queues, request buffering.
- Producer–consumer pipelines, message queues.
- Sliding-window mechanics (with a [deque](deque.md)).

---

## Pitfalls
- ⚠️ `front()`/`pop()` on an empty queue is UB with `std::queue` — guard with `empty()`.
- ⚠️ `pop()` returns void; read `front()` first.
- ⚠️ Circular-buffer **off-by-one**: distinguishing full vs empty (track a `count`, or leave one slot unused).
- ⚠️ Using a `std::vector` and `erase(begin())` to dequeue → O(n) per pop.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Process in arrival order / level by level" → **queue**.
- "Shortest path in an unweighted graph" → **BFS queue**.
- "Sliding-window min/max" → [monotonic deque](monotonic_queue.md).
- "Schedule / round-robin / cooldown" → **queue** (sometimes with a timer/heap).

---

## Example Problems

### Easy — Implement Queue using Stacks
See [../Stacks/stack_implementation.md](../Stacks/stack_implementation.md) — amortized O(1) with two stacks.

### Medium — Number of Islands (BFS flood fill)
Use a queue to expand each island level by level.
```cpp
#include <queue>
#include <vector>
int numIslands(std::vector<std::vector<char>>& g) {
    if (g.empty()) return 0;
    int rows = g.size(), cols = g[0].size(), count = 0;
    int dr[] = {1,-1,0,0}, dc[] = {0,0,1,-1};
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (g[r][c] == '1') {
                ++count;
                std::queue<std::pair<int,int>> q; q.push({r,c}); g[r][c]='0';
                while (!q.empty()) {
                    auto [cr, cc] = q.front(); q.pop();
                    for (int k = 0; k < 4; ++k) {
                        int nr = cr+dr[k], nc = cc+dc[k];
                        if (nr>=0 && nr<rows && nc>=0 && nc<cols && g[nr][nc]=='1') {
                            g[nr][nc]='0'; q.push({nr,nc});
                        }
                    }
                }
            }
    return count;
}
```
**Explanation.** Each `'1'` starts a BFS that floods its whole island; the queue guarantees we expand outward cell by cell.

### Hard — Sliding Window Maximum
Solved optimally with a **monotonic deque** — see [monotonic_queue.md](monotonic_queue.md). O(n).

---

*Next →* [deque.md](deque.md)
