# File: Linked-Lists/singly_linked_list.md

[← Repository Index](../README.md) · **Linked-Lists:** singly · [doubly](doubly_linked_list.md) · [circular](circular_linked_list.md) · [fast_slow_pointer](fast_slow_pointer.md) · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** A singly linked list is a chain of **nodes**, each holding a value and a single pointer (`next`) to the following node. The list is reached through a `head` pointer; the last node's `next` is `null`.

**Why it exists.** Arrays pay O(n) to insert/delete in the middle (shifting) and need a contiguous block. Linked lists make insertion/deletion **O(1)** *given the node* — just rewire pointers — and grow without reallocation. They trade away O(1) random access for O(1) structural edits.

**Problem it solves.** Frequent insertions/deletions where you already hold a reference to the position; building blocks for stacks, queues, adjacency lists, and allocators.

**Real-world analogy.** A scavenger hunt: each clue points to the next location. You can't teleport to clue #5 — you follow the chain — but inserting a new clue only means rewriting one clue to point at it.

---

## Internal Working

```
 head
   |
   v
+----+----+    +----+----+    +----+------+
| 10 |  *-+--> | 20 |  *-+--> | 30 | null |
+----+----+    +----+----+    +----+------+
```

Nodes live anywhere in memory (the heap), linked only by pointers. There is **no address arithmetic**, so reaching index `k` means following `k` pointers (**pointer chasing**), each a potential cache miss — this is why linked lists are slow in practice despite good asymptotics.

**Insertion at head (O(1)):**
```
new->next = head;   // point new node at the old first
head = new;         // head now starts at new node
```

**Deletion** rewires the predecessor's `next` past the target, then frees the node — no element movement.

---

## Operations

| Operation | How | Complexity |
|---|---|---|
| Access index k | walk k pointers | O(n) |
| Search value | linear scan | O(n) |
| Insert at head | rewire head | O(1) |
| Insert at tail | walk to end (or keep tail ptr) | O(n) / O(1) |
| Delete head | move head | O(1) |
| Delete given node's value | find predecessor, rewire | O(n) |
| Reverse | flip all pointers | O(n) |

---

## Complexity

| | Best | Worst | Space |
|---|---|---|---|
| Access/Search | O(1) (head) | O(n) | — |
| Insert/Delete at head | O(1) | O(1) | — |
| Insert/Delete at tail | O(1)† | O(n) | — |
| Overall | | | O(n) + 1 ptr/node |

† O(1) with a maintained `tail` pointer.

---

## C++ Implementation

```cpp
struct Node {
    int data;
    Node* next = nullptr;
    explicit Node(int d) : data(d) {}
};

class SinglyLinkedList {
    Node* head = nullptr;
    Node* tail = nullptr;            // O(1) append
    size_t sz = 0;
public:
    ~SinglyLinkedList() { clear(); }

    void push_front(int v) {                    // O(1)
        Node* n = new Node(v);
        n->next = head; head = n;
        if (!tail) tail = n;
        ++sz;
    }
    void push_back(int v) {                     // O(1) with tail
        Node* n = new Node(v);
        if (tail) tail->next = n; else head = n;
        tail = n; ++sz;
    }
    bool remove(int v) {                        // O(n)
        Node *prev = nullptr, *cur = head;
        while (cur && cur->data != v) { prev = cur; cur = cur->next; }
        if (!cur) return false;
        if (prev) prev->next = cur->next; else head = cur->next;
        if (cur == tail) tail = prev;
        delete cur; --sz; return true;
    }
    void reverse() {                            // O(n), O(1)
        Node *prev = nullptr, *cur = head;
        tail = head;
        while (cur) { Node* nxt = cur->next; cur->next = prev; prev = cur; cur = nxt; }
        head = prev;
    }
    void clear() {
        while (head) { Node* t = head; head = head->next; delete t; }
        tail = nullptr; sz = 0;
    }
    size_t size() const { return sz; }
};
```

**STL equivalent.** `std::forward_list<T>` — a singly linked list with minimal overhead (`push_front`, `insert_after`, `erase_after`). `std::list<T>` is doubly linked.

**When to use STL vs custom.** Use `std::forward_list`/`std::list` only when you genuinely need O(1) splicing and never index. ⚠️ In most "list-like" workloads, `std::vector` is faster due to cache locality — benchmark before choosing a linked list.

---

## Tradeoffs
- ✅ O(1) head insert/delete; grows without reallocation; no contiguous memory needed.
- ❌ O(n) access; poor cache locality; extra pointer per node; manual memory management.
- **Simplicity vs flexibility:** flexible structural edits, but more bug-prone than arrays.

---

## Use Cases
- Implementing stacks/queues; adjacency lists.
- Free lists / memory pools in allocators.
- When data is huge and you can't allocate one contiguous block.

---

## Pitfalls
- ⚠️ **Null dereference** — check `cur` before `cur->next`.
- ⚠️ **Losing the list** — save `head->next` before reassigning `head`.
- ⚠️ **Memory leaks / use-after-free** — free removed nodes; never touch a freed node.
- ⚠️ **Dangling `tail`** — update it on tail deletion.
- ⚠️ Accidental cycles → infinite loops (detect with [Floyd's algorithm](fast_slow_pointer.md)).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Reverse / reorder / merge lists" → [in-place reversal](advanced_problems.md), dummy node.
- "Find middle / detect cycle / Nth from end" → [fast & slow pointers](fast_slow_pointer.md).
- "O(1) space required" (can't copy to array) → pointer manipulation.

**Common problem types:** reverse list, merge two sorted lists, remove Nth node, detect/locate cycle, palindrome list, reorder list.

---

## Example Problems

### Easy — Reverse a Linked List. See `reverse()` above (O(n)/O(1)). The most-asked linked-list question — know it cold.

### Medium — Merge Two Sorted Lists
**Brute force:** collect to array, sort → O((n+m)log).
**Optimized:** splice with a dummy node → O(n+m)/O(1).
```cpp
Node* mergeTwoLists(Node* a, Node* b) {
    Node dummy(0); Node* tail = &dummy;
    while (a && b) {
        if (a->data <= b->data) { tail->next = a; a = a->next; }
        else                    { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;
}
```
**Explanation.** The dummy node removes head special-casing; attach the remaining tail at the end.

### Hard — Remove Nth Node From End (one pass)
**Optimized:** two pointers `n` apart; when the lead hits the end, the trail is just before the target.
```cpp
Node* removeNthFromEnd(Node* head, int n) {
    Node dummy(0); dummy.next = head;
    Node *lead = &dummy, *trail = &dummy;
    for (int i = 0; i < n; ++i) lead = lead->next;   // gap of n
    while (lead->next) { lead = lead->next; trail = trail->next; }
    Node* del = trail->next;
    trail->next = del->next; delete del;
    return dummy.next;
}
```
**Explanation.** The fixed gap means one pass suffices; the dummy handles removing the head itself.

---

*Next →* [doubly_linked_list.md](doubly_linked_list.md)
