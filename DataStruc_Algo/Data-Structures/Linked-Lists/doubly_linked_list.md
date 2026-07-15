# File: Linked-Lists/doubly_linked_list.md

[← Repository Index](../README.md) · **Linked-Lists:** [singly](singly_linked_list.md) · doubly · [circular](circular_linked_list.md) · [fast_slow_pointer](fast_slow_pointer.md) · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** A doubly linked list (DLL) gives each node **two** pointers: `next` and `prev`. This makes traversal bidirectional and deletion O(1) *given the node* (you can reach its predecessor directly).

**Why it exists.** In a singly linked list, deleting a node requires its predecessor — found in O(n). The `prev` pointer removes that scan, enabling **O(1) removal of any node you hold**. This is exactly what an LRU cache needs.

**Problem it solves.** Bidirectional iteration, O(1) deletion/insertion at a known node, and O(1) operations at *both* ends.

**Real-world analogy.** A train where each car is coupled to both the car ahead and behind — you can walk either direction and detach any car by reconnecting its two neighbors.

---

## Internal Working

```
 null <-prev            prev<-           prev<-          ->next null
        +------+------+   +------+------+   +------+------+
 head-> | 10 | next-+-->  | 20 | next-+--> | 30 |  null  | <-tail
        +------+------+   +------+------+   +------+------+
```

Each node stores `prev`, `data`, `next`. A `head` and `tail` pointer give O(1) access to both ends.

**Deletion of node `x` (O(1)):**
```
x->prev->next = x->next;     // bridge predecessor → successor
x->next->prev = x->prev;     // and back
delete x;                    // (guard for head/tail = null neighbors)
```

No traversal needed — both neighbors are directly reachable. That O(1) unlink is the DLL's signature advantage.

---

## Operations

| Operation | Complexity |
|---|---|
| push_front / push_back | O(1) |
| pop_front / pop_back | O(1) |
| Insert before/after given node | O(1) |
| Delete given node | O(1) |
| Search / access index | O(n) |
| Reverse | O(n) |

---

## Complexity

| | Best | Worst | Space |
|---|---|---|---|
| Insert/Delete at known node | O(1) | O(1) | — |
| Both-end ops | O(1) | O(1) | — |
| Search/Access | O(1) | O(n) | — |
| Overall | | | O(n) + **2 ptrs/node** |

---

## C++ Implementation

```cpp
class DoublyLinkedList {
    struct Node {
        int data;
        Node *prev = nullptr, *next = nullptr;
        explicit Node(int d) : data(d) {}
    };
    Node *head = nullptr, *tail = nullptr;
    size_t sz = 0;
public:
    ~DoublyLinkedList() { while (head) pop_front(); }

    void push_back(int v) {                     // O(1)
        Node* n = new Node(v);
        n->prev = tail;
        if (tail) tail->next = n; else head = n;
        tail = n; ++sz;
    }
    void push_front(int v) {                    // O(1)
        Node* n = new Node(v);
        n->next = head;
        if (head) head->prev = n; else tail = n;
        head = n; ++sz;
    }
    void pop_front() {                          // O(1)
        if (!head) return;
        Node* t = head; head = head->next;
        if (head) head->prev = nullptr; else tail = nullptr;
        delete t; --sz;
    }
    void pop_back() {                           // O(1)
        if (!tail) return;
        Node* t = tail; tail = tail->prev;
        if (tail) tail->next = nullptr; else head = nullptr;
        delete t; --sz;
    }
    size_t size() const { return sz; }
};
```

**STL equivalent.** `std::list<T>` is a doubly linked list: `push_front/back`, `pop_front/back`, O(1) `insert`/`erase` given an iterator, and `splice` (O(1) move of sublists between lists).

**When to use STL vs custom.** Use `std::list` for O(1) splicing/erasure at iterators. Build a custom DLL when you need an **intrusive** list (nodes embedded in your own objects, e.g., an LRU cache combining a DLL with a hash map).

---

## Tradeoffs
- ✅ O(1) deletion/insertion anywhere you hold a node; bidirectional traversal; O(1) both ends.
- ❌ **Two pointers per node** (more memory than SLL); more pointers to maintain correctly; still poor cache locality.
- **Simplicity vs flexibility:** more flexible than SLL but more bookkeeping and bug surface.

---

## Use Cases
- **LRU cache** (DLL + hash map → O(1) get/put).
- Browser history (back/forward), undo/redo with two directions.
- Music players (prev/next track).
- Text editors' gap/piece structures; deque implementations.

---

## Pitfalls
- ⚠️ **Forgetting to update `prev`** when inserting/deleting → corrupted backward links.
- ⚠️ **Head/tail null neighbors** — guard `x->prev`/`x->next` before dereferencing.
- ⚠️ Updating only one direction's pointer.
- ⚠️ Double-free when both head and tail point to a single remaining node.

---

## Problem Patterns

🎯 **Recognition hints:**
- "O(1) get and put cache" → **DLL + hashmap (LRU)**.
- "Move/remove arbitrary element in O(1)" → DLL.
- "Bidirectional traversal" → DLL.

**Common problem types:** LRU/LFU cache, design browser history, flatten a multilevel DLL.

---

## Example Problems

### Easy — Design a Deque using a DLL
`push_front/back`, `pop_front/back` are all O(1) (code above). **Explanation.** A DLL is a natural deque; STL `std::deque` is faster but the DLL shows the principle.

### Medium — Flatten a Multilevel Doubly Linked List
Each node may have a `child` DLL; flatten depth-first into a single level.
**Optimized:** iterative with a stack, splicing children inline → O(n).
```cpp
// Pseudocode core: when node->child exists, insert the entire child list
// between node and node->next, fixing prev/next at both seams, then continue.
```
**Explanation.** Treat it like DFS; carefully repair `prev`/`next` at the splice points.

### Hard — LRU Cache (O(1) get & put)
Combine a DLL (recency order) with a hash map (key → node).
**Optimized:** move-to-front on access; evict from the tail when full → O(1).
```cpp
#include <unordered_map>
class LRUCache {
    struct Node { int key, val; Node *prev=nullptr, *next=nullptr; };
    int cap;
    Node *head, *tail;                          // sentinels
    std::unordered_map<int, Node*> map;
    void remove(Node* n){ n->prev->next = n->next; n->next->prev = n->prev; }
    void addFront(Node* n){ n->next = head->next; n->prev = head;
                            head->next->prev = n; head->next = n; }
public:
    LRUCache(int c) : cap(c) {
        head = new Node(); tail = new Node();
        head->next = tail; tail->prev = head;   // empty list with sentinels
    }
    int get(int key) {
        if (!map.count(key)) return -1;
        Node* n = map[key]; remove(n); addFront(n);   // mark most-recent
        return n->val;
    }
    void put(int key, int val) {
        if (map.count(key)) { Node* n = map[key]; n->val = val; remove(n); addFront(n); return; }
        if ((int)map.size() == cap) {                  // evict LRU (before tail)
            Node* lru = tail->prev; remove(lru); map.erase(lru->key); delete lru;
        }
        Node* n = new Node(); n->key = key; n->val = val;
        addFront(n); map[key] = n;
    }
};
```
**Explanation.** Sentinel head/tail nodes eliminate null checks. The hash map gives O(1) lookup; the DLL gives O(1) recency reordering and eviction.

---

*Next →* [circular_linked_list.md](circular_linked_list.md)
