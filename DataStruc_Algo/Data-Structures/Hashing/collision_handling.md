# File: Hashing/collision_handling.md

[← Repository Index](../README.md) · **Hashing:** [hash_tables](hash_tables.md) · collision_handling · [unordered_maps](unordered_maps.md) · [advanced_hashing](advanced_hashing.md)

---

## Conceptual Understanding

**Definition.** A **collision** occurs when two distinct keys hash to the same bucket. Because the key space is far larger than the table, collisions are inevitable (pigeonhole principle). **Collision resolution** strategies determine how a hash table keeps operations fast despite them.

**Why it matters.** The resolution strategy — plus the load factor — decides whether operations stay O(1) average or degrade toward O(n). It also affects memory, cache behavior, and deletion complexity.

**Real-world analogy.** Two guests assigned the same hotel room: either put both in a shared suite (chaining) or send the later arrival to the next open room (open addressing).

---

## Strategy 1 — Separate Chaining

Each bucket holds a **container** (linked list, or a tree/vector) of all entries that hash there.
```
bucket 3: → ("cat",1) → ("bird",9)      (both hashed to 3)
```
- **Insert:** append/prepend to the bucket's list — O(1) (plus a duplicate-key scan).
- **Lookup/delete:** scan the bucket — O(chain length), average O(1 + α).

✅ Simple; handles high load factors gracefully; easy deletion.
❌ Pointer overhead and poor cache locality (list nodes scattered).
💡 Java's `HashMap` and C++'s `std::unordered_map` use chaining; Java upgrades long chains to red-black **trees** (O(log n) per bucket) to resist worst-case/adversarial collisions.

---

## Strategy 2 — Open Addressing

All entries live **in the array itself**; on collision, **probe** for the next open slot by a defined sequence.

### Linear probing: `(h + i) mod m`
Check the next slot, then the next, …
- ✅ Excellent cache locality (contiguous).
- ❌ **Primary clustering**: long runs of filled slots form and grow.

### Quadratic probing: `(h + c1·i + c2·i²) mod m`
Jumps grow quadratically, breaking up primary clusters.
- ✅ Reduces clustering.
- ❌ **Secondary clustering**; may not probe all slots unless m and constants are chosen carefully.

### Double hashing: `(h1 + i·h2(key)) mod m`
A second hash sets a key-dependent step size.
- ✅ Best distribution, minimal clustering.
- ❌ Two hash computations; `h2` must never be 0 and should be coprime with m.

```
Linear probing insert, m=8, collide at 3:
try 3 (full) → 4 (full) → 5 (empty) → place here
```

⚠️ **Deletion in open addressing** can't just empty a slot (it would break probe chains). Use a **tombstone** (deleted marker) or backward-shift deletion.
⚠️ Open addressing requires **α < 1** (table can't be full) and degrades sharply as α → 1; rehash around α ≈ 0.7.

---

## Comparison

| Aspect | Separate chaining | Open addressing |
|---|---|---|
| Storage | buckets + node lists | single array |
| Load factor | can exceed 1 | must stay < 1 (rehash ~0.7) |
| Cache locality | poor | excellent |
| Deletion | easy | needs tombstones |
| Clustering | none | linear/quadratic clustering |
| Memory overhead | pointers per node | empty-slot slack |
| Worst case | O(n) (or O(log n) if treeified) | O(n) |

💡 **Rule of thumb:** chaining for simplicity and high/unknown load; open addressing (esp. **Robin Hood** or **linear probing**) for speed and cache efficiency at controlled load factors.

---

## Robin Hood Hashing (open addressing refinement)
On insert, if the incoming key has probed farther than the key already in a slot, **swap** them — equalizing probe distances and dramatically reducing worst-case lookups. Used by many modern high-performance hash maps.

---

## C++ Illustration — Linear Probing with Tombstones
```cpp
#include <vector>
#include <optional>

class LinearProbingSet {
    enum State { EMPTY, FILLED, DELETED };
    std::vector<int> keys;
    std::vector<State> st;
    size_t count = 0;
    size_t h(int k) const { return (std::hash<int>{}(k)) % keys.size(); }
    void rehash() {
        std::vector<int> ok = keys; std::vector<State> os = st;
        keys.assign(os.size()*2, 0); st.assign(os.size()*2, EMPTY); count = 0;
        for (size_t i = 0; i < ok.size(); ++i) if (os[i]==FILLED) insert(ok[i]);
    }
public:
    LinearProbingSet(size_t cap=8): keys(cap,0), st(cap,EMPTY) {}
    void insert(int k) {
        if ((count+1) * 10 >= keys.size() * 7) rehash();       // load factor ~0.7
        size_t i = h(k);
        while (st[i]==FILLED) { if (keys[i]==k) return; i=(i+1)%keys.size(); }
        keys[i]=k; st[i]=FILLED; ++count;
    }
    bool contains(int k) const {
        size_t i = h(k), steps = 0;
        while (st[i]!=EMPTY && steps < keys.size()) {
            if (st[i]==FILLED && keys[i]==k) return true;      // skip DELETED, keep probing
            i=(i+1)%keys.size(); ++steps;
        }
        return false;
    }
    void erase(int k) {
        size_t i = h(k), steps = 0;
        while (st[i]!=EMPTY && steps < keys.size()) {
            if (st[i]==FILLED && keys[i]==k) { st[i]=DELETED; --count; return; }
            i=(i+1)%keys.size(); ++steps;
        }
    }
};
```

---

## Complexity

| | Average | Worst |
|---|---|---|
| insert/lookup/delete (α bounded) | O(1) | O(n) |
| Expected probes (open addr., load α) | ~1/(1−α) | — |

As α → 1, open-addressing probe counts explode — the mathematical reason to rehash early.

---

## Tradeoffs
- **Chaining vs open addressing:** memory/cache vs simplicity/deletion.
- **Linear vs quadratic vs double hashing:** locality vs clustering resistance.
- **Robin Hood / treeified buckets:** extra logic for better worst case.

---

## Use Cases
- `std::unordered_map` (chaining); high-performance maps (open addressing, Robin Hood).
- Memory-constrained/cache-sensitive systems favor open addressing.
- Adversary-resistant tables use randomized hashing + treeified buckets.

---

## Pitfalls
- ⚠️ Naive deletion in open addressing breaking probe chains (use tombstones/backward-shift).
- ⚠️ Letting the load factor approach 1 in open addressing (rehash ~0.7).
- ⚠️ `h2(key)==0` in double hashing → infinite loop.
- ⚠️ Non-prime capacity with quadratic probing may fail to find empty slots.
- ⚠️ Ignoring hash-flooding DoS — use randomized/keyed hashing for untrusted input.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Design a hash map/set from scratch" → pick **chaining** (simple) or **open addressing** (fast).
- "Why is my hash map slow?" → high load factor / bad hash → **clustering**; rehash or improve the hash.
- "Delete then lookup fails" → missing **tombstones** in open addressing.

---

## Example Problems

### Easy/Medium — Design HashSet / Design HashMap
Implement `add/remove/contains` (or `put/get/remove`) using chaining or fixed-bucket arrays — see the implementations here and in [hash_tables.md](hash_tables.md).

### Medium — Insert Delete GetRandom O(1)
Combine a hash map (value→index) with a dynamic array; delete by swapping the target with the last element. O(1) all operations — see [advanced_hashing.md](advanced_hashing.md).

### Hard — Design a hash map resilient to adversarial keys
Use randomized/keyed hashing and treeified buckets so worst-case per bucket is O(log n) instead of O(n).

---

*Next →* [unordered_maps.md](unordered_maps.md)
