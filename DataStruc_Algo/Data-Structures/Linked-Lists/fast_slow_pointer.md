# File: Linked-Lists/fast_slow_pointer.md

[← Repository Index](../README.md) · **Linked-Lists:** [singly](singly_linked_list.md) · [doubly](doubly_linked_list.md) · [circular](circular_linked_list.md) · fast_slow_pointer · [advanced_problems](advanced_problems.md)

---

## Conceptual Understanding

**Definition.** The fast & slow pointer technique (a.k.a. **Floyd's tortoise and hare**) runs two pointers through a list at **different speeds** — slow advances one node, fast advances two. Their relative motion reveals structural facts: cycles, midpoints, and k-from-end positions.

**Why it exists.** Detecting a cycle or finding the middle naively needs O(n) extra memory (a hash set of visited nodes) or two passes. Two pointers achieve it in **one pass, O(1) space**.

**Problem it solves.** Cycle detection, finding a cycle's entry, the list middle, the k-th node from the end, and "linked-list-as-function" problems (find duplicate number).

**Real-world analogy.** Two runners on a circular track at different speeds — the faster one *must* eventually lap and meet the slower one if the track loops; on a straight track, the fast runner simply reaches the end first.

---

## Internal Working

**Cycle detection.** If there's a loop, the hare (×2) gains one node per step on the tortoise (×1); the gap shrinks by 1 each step until they coincide *inside* the loop. If there's no loop, the hare hits `null`.

```
slow: 1 → 2 → 3 → 4 ↘
                     5 → 6
                     ↑       (cycle 4→5→6→4)
fast laps and meets slow inside the cycle.
```

**Finding the cycle start (the math).** Let the distance head→entry be `a`, entry→meeting be `b`, and the rest of the loop `c` (loop length `b+c`). At meeting: slow went `a+b`, fast went `a+b+c+b = a+2b+c`, and fast = 2×slow ⇒ `a = c`. So resetting one pointer to head and advancing both by 1 makes them meet **at the entry**.

**Finding the middle.** When fast reaches the end, slow is exactly halfway (fast covered 2× the distance).

---

## Operations / Templates

```cpp
// 1) Detect a cycle
bool hasCycle(Node* head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next; fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

// 2) Find cycle start (or nullptr)
Node* cycleStart(Node* head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next; fast = fast->next->next;
        if (slow == fast) {
            Node* p = head;
            while (p != slow) { p = p->next; slow = slow->next; }
            return p;                      // entry node
        }
    }
    return nullptr;
}

// 3) Middle of the list (second middle on even length)
Node* middle(Node* head) {
    Node *slow = head, *fast = head;
    while (fast && fast->next) { slow = slow->next; fast = fast->next->next; }
    return slow;
}
```

---

## Complexity

| Task | Time | Space |
|---|---|---|
| Cycle detection | O(n) | **O(1)** |
| Find cycle start | O(n) | O(1) |
| Find middle | O(n) | O(1) |
| Nth from end | O(n) | O(1) |

The O(1) space is the decisive advantage over a hash-set approach.

---

## C++ Implementation Notes & STL

There is no STL helper — this is a pointer technique. The same idea applies beyond linked lists: any "next = f(current)" sequence that eventually repeats (e.g., `nums[nums[i]]`) can be treated as a virtual linked list and run through Floyd's algorithm.

**When to use vs a hash set.** A `std::unordered_set<Node*>` of visited nodes also detects cycles in O(n) but uses O(n) memory and is slower in practice. Use fast/slow when O(1) space matters or is required.

---

## Tradeoffs
- ✅ O(1) space; single pass; elegant.
- ❌ The cycle-start math is non-obvious (easy to misremember); only works for "one successor per node" structures.

---

## Use Cases
- Detecting infinite loops / corrupted lists.
- Finding the middle for merge-sorting a list or palindrome checks.
- "Find the duplicate number" (array as implicit linked list).
- Happy number detection (digit-square sequence cycles).

---

## Pitfalls
- ⚠️ **Null checks:** test `fast && fast->next` *before* `fast->next->next`.
- ⚠️ **Even vs odd length:** decide which "middle" you need (the template returns the *second* middle on even length).
- ⚠️ For cycle start, reset to **head**, then advance **both by one** (not by two).
- ⚠️ Starting both at `head` vs `head->next` changes meeting behavior — be consistent.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Linked list" + "cycle / loop / middle / Nth from end" → **fast & slow**.
- "Find the duplicate number, O(1) space, don't modify array" → **Floyd on indices**.
- "Sequence eventually repeats" (happy number) → **Floyd**.

**Common problem types:** linked list cycle I/II, middle of list, palindrome list, reorder list, find duplicate number, happy number.

---

## Example Problems

### Easy — Middle of the Linked List. See `middle()` above. O(n)/O(1).

### Medium — Linked List Cycle II (return the entry node). See `cycleStart()`. O(n)/O(1).
**Explanation.** After the meeting, `a = c` guarantees the head-walker and the meeting-walker collide at the cycle entry.

### Hard — Find the Duplicate Number (Floyd on an array)
`nums` has n+1 values in `[1, n]`; exactly one duplicate. Treat `i → nums[i]` as a linked list; the duplicate is the cycle's entry.
**Brute force:** hash set → O(n) time, O(n) space.
**Optimized:** Floyd → O(n)/O(1), array unmodified.
```cpp
int findDuplicate(const std::vector<int>& nums) {
    int slow = nums[0], fast = nums[0];
    do { slow = nums[slow]; fast = nums[nums[fast]]; } while (slow != fast);
    slow = nums[0];                              // reset to "head"
    while (slow != fast) { slow = nums[slow]; fast = nums[fast]; }
    return slow;                                 // cycle entry = duplicate
}
```
**Explanation.** Because values index into the array, following `nums[i]` forms a chain that must loop at the repeated value — the cycle's entry point is the duplicate.

---

*Next →* [advanced_problems.md](advanced_problems.md)
