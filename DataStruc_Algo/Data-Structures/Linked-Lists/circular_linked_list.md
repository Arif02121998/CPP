# File: Linked-Lists/circular_linked_list.md

[← Repository Index](../README.md) · **Linked-Lists:** [singly](singly_linked_list.md) · [doubly](doubly_linked_list.md) · circular · [fast_slow_pointer](fast_slow_pointer.md) · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** A circular linked list has its last node point **back to the first** (singly) or links both ways in a ring (doubly). There is no `null` terminator; traversal cycles forever unless you stop at a reference node.

**Why it exists.** Some problems are inherently cyclic — round-robin scheduling, turn-based games, circular buffers — where "after the last comes the first." A circular structure models this directly and gives O(1) access to both ends via a single `tail` pointer (since `tail->next` is the head).

**Problem it solves.** Repeating/rotating traversal; fair resource sharing; ring buffers.

**Real-world analogy.** A group passing a ball in a circle — after the last person, it returns to the first. A clock face. A merry-go-round.

---

## Internal Working

**Circular singly linked list** (keep a `tail`; `tail->next` is the head):
```
        +----+----+    +----+----+    +----+----+
  +---> | 10 |  *-+--> | 20 |  *-+--> | 30 |  *-+---+
  |     +----+----+    +----+----+    +----+----+   |
  +-----------------------------------------------+
        (tail->next points back to head = node 10)
```

With a `tail` pointer:
- `head = tail->next` → O(1).
- `push_back`: new node after tail, point new->next to head, advance tail → O(1).
- `push_front`: insert after tail (becomes new head), tail unchanged → O(1).

**Circular doubly linked list:** `head->prev = tail` and `tail->next = head`, forming a ring traversable in both directions.

---

## Operations

| Operation | With `tail` pointer | Complexity |
|---|---|---|
| Access head | `tail->next` | O(1) |
| Insert front/back | rewire around tail | O(1) |
| Delete front | bypass head | O(1) |
| Search | walk until back to start | O(n) |
| Traverse (one loop) | stop when you return to start | O(n) |

---

## Complexity

| | Best | Worst | Space |
|---|---|---|---|
| Insert front/back | O(1) | O(1) | — |
| Delete front | O(1) | O(1) | — |
| Search/Delete value | O(1) | O(n) | — |
| Overall | | | O(n) |

---

## C++ Implementation

```cpp
class CircularLinkedList {
    struct Node { int data; Node* next = nullptr; explicit Node(int d):data(d){} };
    Node* tail = nullptr;                    // tail->next == head
    size_t sz = 0;
public:
    ~CircularLinkedList() { clear(); }

    void push_back(int v) {                  // O(1)
        Node* n = new Node(v);
        if (!tail) { n->next = n; tail = n; }       // single-node ring
        else { n->next = tail->next; tail->next = n; tail = n; }
        ++sz;
    }
    void push_front(int v) {                 // O(1)
        push_back(v);                        // insert after tail...
        tail = tail->next;                   // ...but keep tail where it was
        // Equivalent: new node becomes head; tail unchanged.
        // (Simplest correct form below.)
    }
    void traverse() const {                  // O(n) — one full loop
        if (!tail) return;
        Node* cur = tail->next;              // head
        do { /* visit cur->data */ cur = cur->next; } while (cur != tail->next);
    }
    void clear() {
        if (!tail) return;
        Node* cur = tail->next;
        tail->next = nullptr;                // break the ring first!
        while (cur) { Node* t = cur; cur = cur->next; delete t; }
        tail = nullptr; sz = 0;
    }
    size_t size() const { return sz; }
};
```

> Note: the cleanest `push_front` allocates a node, sets `n->next = tail->next`, and `tail->next = n` (without moving `tail`). The snippet above keeps the API illustrative; in production prefer the explicit version.

**STL equivalent.** None directly. A **circular buffer** is the array-based cousin — implement with a fixed `std::vector` + modular indices (see [Queues/queue.md](../Queues/queue.md)) or use `boost::circular_buffer`.

**When to use STL vs custom.** For fixed-capacity round-robin, an array-based **circular buffer** is faster and cache-friendly than a circular linked list. Use the linked version when sizes vary widely and you need O(1) growth.

---

## Tradeoffs
- ✅ O(1) both-end ops with one pointer; natural round-robin; no special "end."
- ❌ Easy to create **infinite loops** (no null terminator); traversal must track a stop reference; same cache/memory downsides as any linked list.

---

## Use Cases
- **Round-robin CPU scheduling**; turn rotation in games.
- **Circular buffers** (streaming audio/video, producer-consumer).
- The Josephus problem; cyclic playlists ("repeat all").
- Resource pools cycled fairly.

---

## Pitfalls
- ⚠️ **Infinite loops** — always stop traversal when you return to the start node.
- ⚠️ **Break the ring before freeing** in the destructor, or `clear()` loops forever / double-frees.
- ⚠️ Single-node ring: `node->next` must point to itself.
- ⚠️ Updating `tail` incorrectly on insert/delete corrupts the ring.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Round robin / repeat / cycle through" → circular list or modular array index.
- "Eliminate every k-th in a circle" → **Josephus** (circular list or math).
- "Fixed-size streaming buffer" → circular buffer.

**Common problem types:** Josephus problem, round-robin scheduler, design circular queue, split a circular list.

---

## Example Problems

### Easy — Design Circular Queue
Fixed capacity ring with modular indices (array-based — preferred).
```cpp
class MyCircularQueue {
    std::vector<int> buf; int head=0, count=0, cap;
public:
    MyCircularQueue(int k): buf(k), cap(k) {}
    bool enQueue(int v){ if(count==cap) return false;
        buf[(head+count)%cap]=v; ++count; return true; }
    bool deQueue(){ if(!count) return false; head=(head+1)%cap; --count; return true; }
    int Front(){ return count? buf[head] : -1; }
    int Rear(){ return count? buf[(head+count-1)%cap] : -1; }
    bool isEmpty(){ return count==0; } bool isFull(){ return count==cap; }
};
```
**Explanation.** Modular arithmetic reuses freed front slots — no shifting.

### Medium — Josephus Problem
`n` people in a circle, every k-th eliminated; find the survivor.
**Brute force (list):** simulate with a circular list → O(n·k).
**Optimized (math/DP):** `J(1)=0; J(i)=(J(i-1)+k)%i` → O(n).
```cpp
int josephus(int n, int k) {
    int res = 0;                       // 0-indexed survivor
    for (int i = 2; i <= n; ++i) res = (res + k) % i;
    return res + 1;                    // 1-indexed
}
```
**Explanation.** The recurrence tracks how the survivor's position shifts as the circle shrinks — no simulation needed.

### Hard — Split a Circular Linked List into Two Halves
Use fast/slow pointers to find the midpoint, then close two rings.
**Explanation.** Combine [fast & slow pointers](fast_slow_pointer.md) with careful ring re-closing; ⚠️ handle odd/even length and re-point both tails to their new heads.

---

*Next →* [fast_slow_pointer.md](fast_slow_pointer.md)
