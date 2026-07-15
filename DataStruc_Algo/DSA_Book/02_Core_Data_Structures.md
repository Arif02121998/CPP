# Chapter 2 — Core Data Structures

[← Back to Table of Contents](README.md)

Each structure follows the same loop: **Concept → Internal Working → C++ Implementation → Tradeoffs → Use Cases → Pitfalls.**

---

## 2.1 Arrays

### A. Conceptual Layer

**Definition.** An array is a **contiguous block of memory** holding elements of the same type, indexed by integer positions `0 … n-1`.

**Why it exists.** It is the most fundamental structure because it maps directly onto how memory works: a base address plus an offset. It gives the single most powerful primitive in computing — **O(1) random access**.

**Problems it solves.** Anytime you need fast indexed access, fixed-size collections, or a cache-friendly sequence (matrices, lookup tables, buffers).

**Real-world analogy.** A street of identical houses with consecutive numbers. To visit house #47 you don't walk past 1–46; you go straight there because address = start + 47 × house-width.

**Operations & complexity:**

| Operation | Complexity | Why |
|---|---|---|
| Access by index | O(1) | address arithmetic |
| Search (unsorted) | O(n) | must scan |
| Search (sorted) | O(log n) | binary search |
| Insert/delete at end | O(1)* / amortized | dynamic array |
| Insert/delete at middle | O(n) | shift elements |
| Update | O(1) | direct write |

### B. Internal Working

Memory layout for `int a[5]` (4-byte ints) at base address `1000`:

```
index:    0     1     2     3     4
addr:   1000  1004  1008  1012  1016
        +-----+-----+-----+-----+-----+
value:  | 10  | 20  | 30  | 40  | 50  |
        +-----+-----+-----+-----+-----+
```

`address(a[i]) = base + i × sizeof(element)`. This is why access is O(1) and why indices start at 0 (offset of the first element is 0).

**Why arrays are cache-friendly.** CPUs load memory in **cache lines** (~64 bytes). Reading `a[i]` pulls neighbors `a[i+1], a[i+2]…` into cache for free. Sequential array traversal is dramatically faster than chasing pointers — this is the #1 reason arrays often beat "asymptotically equal" linked structures in practice.

**Insertion in the middle** requires shifting:
```
Insert 25 at index 2 in [10,20,30,40,50]:
Step 1: shift 30,40,50 right → [10,20,_,30,40,50]
Step 2: place 25            → [10,20,25,30,40,50]
Cost: O(n) shifts.
```

### C. C++ Implementation

```cpp
#include <array>
#include <vector>
#include <iostream>

// Static array — size fixed at compile time, lives on stack
int raw[5] = {10, 20, 30, 40, 50};

// std::array — zero-overhead wrapper with a safe interface
std::array<int, 5> sa = {10, 20, 30, 40, 50};

// std::vector — dynamic array (resizes automatically)
std::vector<int> v = {10, 20, 30};
v.push_back(40);          // amortized O(1)
v[1] = 99;                // O(1) update
int x = v.at(2);          // bounds-checked access (throws on OOB)

// Insert at middle — O(n)
v.insert(v.begin() + 1, 15);   // [10,15,99,30,40]
v.erase(v.begin() + 1);        // remove — O(n)
```

**STL equivalents.**
- `std::array<T,N>` — fixed size, stack-allocated, no overhead. Use when size is known at compile time.
- `std::vector<T>` — the workhorse dynamic array. **Use this 95% of the time.**
- Raw arrays / C-arrays — avoid in modern C++ except for interop.

**When to use STL vs custom.** Almost always use `std::vector`. Only write a custom array if you're learning, need a specialized memory layout (e.g., SIMD alignment), or are in an embedded/no-STL environment.

### D. Tradeoffs
- ✅ O(1) access, cache-friendly, minimal memory overhead, simple.
- ❌ Costly insert/delete in the middle (O(n) shifting); fixed-size (static) or amortized resize (dynamic).

### E. Use Cases
- Matrices/grids, image pixels, audio buffers.
- Lookup tables, dynamic programming tables.
- Backing store for stacks, heaps, hash tables.
- Any "list of things" where you mostly index and append.

### F. Pitfalls
- ⚠️ **Off-by-one / out-of-bounds.** `a[n]` is invalid for size `n`. Out-of-bounds is *undefined behavior* in C++ (no automatic error). Use `.at()` when you want bounds checking.
- ⚠️ **Invalidated iterators/pointers.** `push_back` may reallocate and invalidate all existing iterators/pointers/references. Never hold a pointer across a possible resize.
- ⚠️ **Forgetting `reserve`** when the final size is known → unnecessary reallocations.
- ⚠️ Confusing `size()` (elements) with `capacity()` (allocated slots).

---

## 2.2 Linked Lists

### A. Conceptual Layer

**Definition.** A linked list is a sequence of **nodes**, where each node stores a value and a **pointer** to the next node (and, for doubly linked, the previous one). Nodes are scattered in memory, connected by references.

**Why it exists.** Arrays pay O(n) to insert/delete in the middle because of shifting. Linked lists make insertion/deletion **O(1)** *if you already hold the node* — just rewire pointers, no shifting, no contiguous block needed.

**Problems it solves.** Frequent insertions/deletions at arbitrary positions; building blocks for stacks, queues, adjacency lists, LRU caches, and allocators where contiguous memory is impractical.

**Real-world analogy.** A treasure hunt: each clue (node) tells you where the next clue is. You can't jump to clue #5 — you must follow from the start. But inserting a new clue is trivial: rewrite two clues to point to it.

**Variants:**
- **Singly linked** — each node points to `next`. One-directional.
- **Doubly linked** — each node points to `next` and `prev`. Bidirectional, easier deletion.
- **Circular** — last node points back to the first (singly or doubly). Useful for round-robin.

**Operations & complexity:**

| Operation | Singly | Doubly | Array |
|---|---|---|---|
| Access by index | O(n) | O(n) | O(1) |
| Insert/delete at head | O(1) | O(1) | O(n) |
| Insert/delete at tail | O(n)† | O(1)‡ | O(1) amortized |
| Insert/delete given node | O(1)§ | O(1) | O(n) |
| Search | O(n) | O(n) | O(n) |

† O(1) with a tail pointer for insert; deleting tail is O(n) in singly (need predecessor).
‡ O(1) with tail pointer. § Singly needs the *previous* node to delete; doubly has `prev`.

### B. Internal Working

Singly linked list `10 → 20 → 30`:
```
 head
   |
   v
+----+----+    +----+----+    +----+------+
| 10 |  *-+--> | 20 |  *-+--> | 30 | null |
+----+----+    +----+----+    +----+------+
```

**Insertion at head** — O(1), no shifting:
```
newNode->next = head;   // point new node at old first
head = newNode;         // head now points to new node
```

**Deletion** rewires the predecessor's pointer past the target; the node is then freed. No element movement — only pointer updates.

**Why O(n) access.** There is no address arithmetic — nodes are scattered. To reach index `k` you must follow `k` pointers (pointer chasing), each potentially a cache miss. This is why linked lists are **slow in practice** despite good asymptotics.

### C. C++ Implementation

```cpp
#include <memory>

// Singly linked list
struct Node {
    int data;
    Node* next;
    Node(int d) : data(d), next(nullptr) {}
};

class SinglyLinkedList {
    Node* head = nullptr;
    Node* tail = nullptr;     // keep tail for O(1) append
    size_t sz = 0;
public:
    ~SinglyLinkedList() { clear(); }

    void push_front(int v) {           // O(1)
        Node* n = new Node(v);
        n->next = head;
        head = n;
        if (!tail) tail = n;
        ++sz;
    }

    void push_back(int v) {            // O(1) with tail pointer
        Node* n = new Node(v);
        if (tail) tail->next = n;
        else head = n;
        tail = n;
        ++sz;
    }

    bool remove(int v) {               // O(n): delete first match
        Node* prev = nullptr;
        Node* cur = head;
        while (cur && cur->data != v) { prev = cur; cur = cur->next; }
        if (!cur) return false;        // not found
        if (prev) prev->next = cur->next;
        else head = cur->next;
        if (cur == tail) tail = prev;
        delete cur;
        --sz;
        return true;
    }

    // Reverse in O(n) time, O(1) space — classic interview question
    void reverse() {
        Node* prev = nullptr;
        Node* cur = head;
        tail = head;
        while (cur) {
            Node* nxt = cur->next;
            cur->next = prev;          // flip the pointer
            prev = cur;
            cur = nxt;
        }
        head = prev;
    }

    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
        tail = nullptr; sz = 0;
    }
    size_t size() const { return sz; }
};
```

**STL equivalents.**
- `std::list<T>` — doubly linked list. O(1) insert/erase anywhere *given an iterator*; no random access.
- `std::forward_list<T>` — singly linked, minimal memory overhead.

```cpp
#include <list>
std::list<int> dl = {10, 20, 30};
auto it = std::next(dl.begin());  // points to 20
dl.insert(it, 15);                // O(1) given the iterator → 10,15,20,30
dl.erase(it);                     // O(1) → 10,15,30
```

**When to use STL vs custom.** Use `std::list` only when you genuinely need O(1) splicing/erasure at known positions and never index. In most cases `std::vector` outperforms `std::list` even for "insert in middle" workloads because of cache locality — **benchmark before choosing a linked list.**

### D. Tradeoffs
- ✅ O(1) insert/delete given the node; grows without reallocation; no contiguous memory needed.
- ❌ O(n) access; poor cache locality (pointer chasing); extra memory per node (pointers); harder to get right (memory management).

### E. Use Cases
- Implementing stacks/queues/deques.
- Adjacency lists for graphs.
- LRU cache (doubly linked list + hash map).
- Undo/redo histories; music playlists (circular for repeat).
- Memory allocators / free lists.

### F. Pitfalls
- ⚠️ **Dangling pointers / use-after-free** — accessing a node after `delete`.
- ⚠️ **Memory leaks** — forgetting to free nodes (use smart pointers or a proper destructor).
- ⚠️ **Losing the list** — reassigning `head` before saving `head->next`.
- ⚠️ **Null dereference** — always check `cur != nullptr` before `cur->next`.
- ⚠️ **Cycle bugs** — accidental cycles cause infinite loops (use Floyd's cycle detection to find them).

---

## 2.3 Stacks

### A. Conceptual Layer

**Definition.** A stack is a **LIFO** (Last-In, First-Out) collection: the last element pushed is the first popped. Access is restricted to **one end** (the "top").

**Why it exists.** Many problems are naturally last-in-first-out: undo, function calls, matching brackets, backtracking. Restricting access to one end makes all operations O(1) and the logic dead simple.

**Problems it solves.** Reversal, nested-structure matching, expression evaluation, DFS, the function call mechanism itself.

**Real-world analogy.** A stack of plates. You add to the top and remove from the top. You can't pull a plate from the middle without removing those above it.

**Operations (all O(1)):**

| Operation | Meaning |
|---|---|
| `push(x)` | add `x` to top |
| `pop()` | remove top |
| `top()`/`peek()` | view top without removing |
| `empty()` | is it empty? |
| `size()` | element count |

### B. Internal Working

A stack is an **interface/discipline**, not a storage layout — it can be backed by an array or a linked list.

**Array-backed** (most common): keep a `topIndex`. Push writes `data[++top]`; pop returns `data[top--]`. O(1), cache-friendly, amortized growth.

```
push(10), push(20), push(30):
   top → [30]
         [20]
         [10]
pop() → returns 30, top now at 20
```

**Linked-list-backed:** push/pop at the head. O(1) always, no resizing, but pointer overhead.

### C. C++ Implementation

```cpp
#include <vector>
#include <stdexcept>

template <typename T>
class Stack {
    std::vector<T> data;          // array-backed
public:
    void push(const T& x) { data.push_back(x); }   // amortized O(1)
    void pop() {
        if (data.empty()) throw std::out_of_range("pop on empty stack");
        data.pop_back();                            // O(1)
    }
    T& top() {
        if (data.empty()) throw std::out_of_range("top on empty stack");
        return data.back();
    }
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};
```

**STL equivalent.** `std::stack<T>` is an adapter over a container (default `std::deque`):

```cpp
#include <stack>
std::stack<int> s;
s.push(10); s.push(20);
int t = s.top();   // 20
s.pop();           // removes 20
// ⚠️ s.top()/s.pop() on an empty stack is UNDEFINED BEHAVIOR — check empty() first!
```

**When to use STL vs custom.** Use `std::stack`. Write custom only to learn or to add behavior (e.g., a min-stack that tracks the minimum in O(1)).

```cpp
// Classic: Min-Stack — O(1) push, pop, AND getMin
#include <stack>
class MinStack {
    std::stack<int> s, mins;     // mins tracks running minimum
public:
    void push(int x) {
        s.push(x);
        mins.push(mins.empty() ? x : std::min(x, mins.top()));
    }
    void pop()    { s.pop(); mins.pop(); }
    int  top()    { return s.top(); }
    int  getMin() { return mins.top(); }   // O(1)
};
```

### D. Tradeoffs
- ✅ All operations O(1); trivial to reason about; array-backed is cache-friendly.
- ❌ Only top is accessible; no random access or search without popping.

### E. Use Cases
- **Function call stack** (the runtime uses one for you).
- Undo/redo, browser back button.
- Bracket/parenthesis matching, syntax parsing.
- Expression evaluation (infix→postfix, postfix evaluation).
- Iterative DFS, backtracking state.
- Monotonic stack (next-greater-element, histograms).

### F. Pitfalls
- ⚠️ **`pop`/`top` on empty** — UB with `std::stack`. Always guard with `empty()`.
- ⚠️ `std::stack::pop()` returns `void` (doesn't return the value) — read `top()` first, then `pop()`.
- ⚠️ **Stack overflow** from unbounded recursion — the call stack is finite (~1–8 MB).

---

## 2.4 Queues

### A. Conceptual Layer

**Definition.** A queue is a **FIFO** (First-In, First-Out) collection: elements are added at the **rear** and removed from the **front**, in arrival order.

**Why it exists.** Fairness and order. When tasks must be served in the order they arrive (no cutting in line), FIFO is the natural model. It is the backbone of scheduling and BFS.

**Real-world analogy.** A line at a ticket counter. First person in line is served first; new arrivals join the back.

**Operations (all O(1)):** `enqueue/push` (rear), `dequeue/pop` (front), `front`, `back`, `empty`, `size`.

#### Variants

**Deque (double-ended queue).** Insert/remove at **both** ends in O(1). A superset of stack and queue.

**Priority Queue.** Elements are served by **priority**, not arrival order. The highest- (or lowest-) priority element comes out first. Backed by a **heap** (see Chapter 3). `push`/`pop` are O(log n); `top` is O(1).

### B. Internal Working

A **naive array queue** that moves `front` forward wastes space at the front. The fix is a **circular buffer**: indices wrap with modulo arithmetic, reusing freed front slots.

```
Circular buffer, capacity 5:
indices:  0   1   2   3   4
        [ _ , A , B , C , _ ]
                ^front      ^rear
enqueue D → rear wraps to index 4, then 0...
front = (front + 1) % capacity   on dequeue
rear  = (rear  + 1) % capacity   on enqueue
```

A **linked-list queue** keeps `front` and `rear` pointers: dequeue at front, enqueue at rear — both O(1), no fixed capacity.

### C. C++ Implementation

```cpp
// Circular-buffer queue (fixed capacity)
#include <vector>
#include <stdexcept>

template <typename T>
class CircularQueue {
    std::vector<T> buf;
    size_t head = 0, tail = 0, count = 0, cap;
public:
    explicit CircularQueue(size_t capacity) : buf(capacity), cap(capacity) {}

    void enqueue(const T& x) {
        if (count == cap) throw std::overflow_error("queue full");
        buf[tail] = x;
        tail = (tail + 1) % cap;
        ++count;
    }
    void dequeue() {
        if (count == 0) throw std::underflow_error("queue empty");
        head = (head + 1) % cap;
        --count;
    }
    T& front() {
        if (count == 0) throw std::underflow_error("queue empty");
        return buf[head];
    }
    bool empty() const { return count == 0; }
    size_t size() const { return count; }
};
```

**STL equivalents.**
- `std::queue<T>` — FIFO adapter (default over `std::deque`).
- `std::deque<T>` — double-ended queue, also supports random access.
- `std::priority_queue<T>` — max-heap by default.

```cpp
#include <queue>
std::queue<int> q;
q.push(1); q.push(2);
int f = q.front();   // 1
q.pop();             // removes 1

std::deque<int> dq;
dq.push_front(0); dq.push_back(1);   // both O(1)

// Max-heap (largest on top)
std::priority_queue<int> maxpq;
maxpq.push(3); maxpq.push(1); maxpq.push(5);
int big = maxpq.top();               // 5

// Min-heap (smallest on top) — common in Dijkstra
std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;
minpq.push(3); minpq.push(1); minpq.push(5);
int small = minpq.top();             // 1
```

**When to use STL vs custom.** Always use the STL adapters. Write a circular buffer only for fixed-memory/embedded systems or lock-free concurrent designs.

### D. Tradeoffs

| | Queue | Deque | Priority Queue |
|---|---|---|---|
| Order | FIFO | both ends | by priority |
| push/pop | O(1) | O(1) | O(log n) |
| Random access | ❌ | ✅ (deque) | ❌ |
| Backing | deque/list | block-array | heap |

### E. Use Cases
- **Queue:** BFS, task/job scheduling, producer-consumer buffers, print spoolers, request handling.
- **Deque:** sliding-window maximum (monotonic deque), undo with bounded history, work-stealing schedulers.
- **Priority Queue:** Dijkstra/Prim, event simulation, Huffman coding, "top-K" problems, A* search.

### F. Pitfalls
- ⚠️ **Naive array queue wastes space** — use a circular buffer or `std::deque`.
- ⚠️ `front()`/`pop()` on empty `std::queue` is **UB** — check `empty()`.
- ⚠️ **Priority queue is a heap, not sorted** — iterating doesn't give sorted order; only `top()` is the extreme.
- ⚠️ Default `std::priority_queue` is a **max-heap**; for min-heap pass `std::greater`. Forgetting this is a very common Dijkstra bug.

---

## Chapter Summary

| Structure | Access | Insert/Delete | Best for | Backing |
|---|---|---|---|---|
| Array (`vector`) | O(1) | O(n) mid / O(1)* end | indexed data, iteration | contiguous memory |
| Linked List | O(n) | O(1) given node | frequent mid edits, splicing | nodes + pointers |
| Stack | O(1) top | O(1) | LIFO: undo, DFS, parsing | array / list |
| Queue | O(1) ends | O(1) | FIFO: BFS, scheduling | circular buffer / list |
| Priority Queue | O(1) top | O(log n) | priority order, top-K | heap |

---

*Next →* [Chapter 3: Advanced Data Structures](03_Advanced_Data_Structures.md)
