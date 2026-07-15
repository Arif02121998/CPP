# File: Linked-Lists/advanced_problems.md

[← Repository Index](../README.md) · **Linked-Lists:** [singly](singly_linked_list.md) · [doubly](doubly_linked_list.md) · [circular](circular_linked_list.md) · [fast_slow_pointer](fast_slow_pointer.md) · advanced_problems

---

## Conceptual Understanding

**Definition.** This file gathers the **high-frequency hard linked-list problems** and the reusable techniques behind them: the dummy-node idiom, in-place reversal (whole/partial/in-groups), merge-based sorting, and pointer bookkeeping for reordering.

**Why a dedicated file.** Linked-list "hard" problems are rarely about algorithms — they're about **flawless pointer surgery** under O(1) space. A few core idioms cover almost all of them.

**Real-world analogy.** Rewiring a circuit board: the logic is simple, but one crossed wire breaks everything — discipline and a clear before/after picture are everything.

---

## Core Idioms

### Idiom 1 — Dummy (sentinel) node
A throwaway node before the head removes special-casing when the head itself may change (insert/delete at front, merge).
```cpp
Node dummy(0); dummy.next = head;
// ... operate using &dummy ...
return dummy.next;     // the (possibly new) head
```

### Idiom 2 — In-place reversal (3 pointers)
```cpp
Node* reverse(Node* head) {
    Node* prev = nullptr;
    while (head) { Node* nxt = head->next; head->next = prev; prev = head; head = nxt; }
    return prev;
}
```
💡 Always save `next` **before** rewiring, or you lose the rest of the list.

### Idiom 3 — Find middle (fast/slow)
See [fast_slow_pointer.md](fast_slow_pointer.md). Used to split lists for merge sort and palindrome checks.

---

## Problem 1 — Reverse Nodes in k-Group (Hard)

Reverse every consecutive group of k nodes; leftover tail (< k) stays as-is.
**Brute force:** copy values to array, reverse per block, rebuild → O(n) time, O(n) space.
**Optimized:** reverse each group in place → O(n)/O(1).
```cpp
Node* reverseKGroup(Node* head, int k) {
    Node* node = head;
    for (int i = 0; i < k; ++i) { if (!node) return head; node = node->next; }
    Node* prev = reverseKGroup(node, k);          // reverse the remainder first
    for (int i = 0; i < k; ++i) {                 // reverse this group
        Node* nxt = head->next;
        head->next = prev; prev = head; head = nxt;
    }
    return prev;                                   // new head of this group
}
```
**Explanation.** Check k nodes exist; recursively handle the rest; then splice this group reversed onto it.

---

## Problem 2 — Reorder List (Medium/Hard)

Transform `L0→L1→…→Ln` into `L0→Ln→L1→Ln-1→…`.
**Optimized:** find middle → reverse second half → merge alternately → O(n)/O(1).
```cpp
void reorderList(Node* head) {
    if (!head || !head->next) return;
    // 1) middle
    Node *slow = head, *fast = head;
    while (fast->next && fast->next->next) { slow = slow->next; fast = fast->next->next; }
    // 2) reverse second half
    Node *second = slow->next, *prev = nullptr; slow->next = nullptr;
    while (second) { Node* nxt = second->next; second->next = prev; prev = second; second = nxt; }
    // 3) merge two halves
    Node *first = head; second = prev;
    while (second) {
        Node *f = first->next, *s = second->next;
        first->next = second; second->next = f;
        first = f; second = s;
    }
}
```
**Explanation.** Three classic idioms composed: split, reverse, weave.

---

## Problem 3 — Merge k Sorted Lists (Hard)

**Brute force:** collect all values, sort, rebuild → O(N log N).
**Optimized (min-heap):** always emit the smallest current head → O(N log k).
```cpp
#include <queue>
Node* mergeKLists(std::vector<Node*>& lists) {
    auto cmp = [](Node* a, Node* b){ return a->data > b->data; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);
    for (Node* l : lists) if (l) pq.push(l);
    Node dummy(0); Node* tail = &dummy;
    while (!pq.empty()) {
        Node* n = pq.top(); pq.pop();
        tail->next = n; tail = n;
        if (n->next) pq.push(n->next);
    }
    return dummy.next;
}
```
**Explanation.** The heap holds k candidates; each pop/push is O(log k); every node is processed once.
**Alternative:** divide & conquer pairwise merge — also O(N log k), O(1) extra.

---

## Problem 4 — Copy List with Random Pointer (Medium/Hard)

Each node has `next` and a `random` pointer; deep-copy the list.
**Brute force:** hashmap old→new in two passes → O(n)/O(n).
**Optimized (interleaving):** weave copies between originals → O(n)/O(1) extra.
```cpp
// 1) insert copy after each node:  A -> A' -> B -> B' ...
// 2) wire randoms:  A'->random = A->random->next
// 3) unweave to separate the two lists
```
**Explanation.** Interleaving lets each copy find its random target via the original's `random->next`, removing the hash map.

---

## Problem 5 — Palindrome Linked List (Medium)

**Optimized:** find middle → reverse second half → compare → O(n)/O(1).
```cpp
bool isPalindrome(Node* head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    Node* prev = nullptr;                          // reverse second half
    while (slow) { Node* nxt = slow->next; slow->next = prev; prev = slow; slow = nxt; }
    Node *a = head, *b = prev;
    while (b) { if (a->data != b->data) return false; a = a->next; b = b->next; }
    return true;
}
```
**Explanation.** Compare the first half against the reversed second half. ⚠️ Optionally restore the list afterward.

---

## Complexity Summary

| Problem | Time | Space |
|---|---|---|
| Reverse k-group | O(n) | O(1) iterative (O(n/k) recursion) |
| Reorder list | O(n) | O(1) |
| Merge k lists | O(N log k) | O(k) |
| Copy random list | O(n) | O(1) extra (interleave) |
| Palindrome list | O(n) | O(1) |

---

## Tradeoffs
- ✅ O(1)-space pointer techniques avoid copying to arrays.
- ❌ High bug risk — one mis-set pointer corrupts the list. Draw before/after diagrams; test on length 0/1/2.

---

## Use Cases
- Streaming/merge pipelines (merge k lists ≈ external k-way merge).
- Deep-copy of graph-like node structures (random pointer).
- In-place transformations under tight memory.

---

## Pitfalls
- ⚠️ Losing the remainder of the list (save `next` first).
- ⚠️ Not terminating the new tail with `nullptr` → accidental cycle.
- ⚠️ Off-by-one in group/half boundaries.
- ⚠️ Forgetting the dummy node when the head can change.
- ⚠️ Recursion depth on very long lists (k-group recursion is O(n/k) deep).

---

## Problem Patterns

🎯 **Recognition hints:**
- "Reverse / in groups / reorder" → **in-place reversal + dummy node**.
- "Merge sorted lists" → **heap or pairwise D&C**.
- "Deep copy with extra pointers" → **interleaving trick**.
- "Palindrome / compare halves" → **middle + reverse half**.

---

## Example Recap
- **Easy:** reverse a list, merge two sorted lists ([singly_linked_list.md](singly_linked_list.md)).
- **Medium:** reorder list, palindrome list, copy random list.
- **Hard:** reverse in k-groups, merge k sorted lists.

---

*Next →* [Stacks/stack_basics.md](../Stacks/stack_basics.md)
