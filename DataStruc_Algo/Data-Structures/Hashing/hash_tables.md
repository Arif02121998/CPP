# File: Hashing/hash_tables.md

[← Repository Index](../README.md) · **Hashing:** hash_tables · [collision_handling](collision_handling.md) · [unordered_maps](unordered_maps.md) · [advanced_hashing](advanced_hashing.md)

---

## Conceptual Understanding

**Definition.** A **hash table** stores key→value pairs and provides **average O(1)** insertion, lookup, and deletion. A **hash function** maps each key to an index (bucket) in an underlying array.

**Why it exists.** When you need fast membership/lookup by key and don't care about order, hashing beats trees (O(log n)) and arrays (O(n) search). It trades ordering and worst-case guarantees for average constant-time access.

**Problem it solves.** Fast dictionaries, sets, frequency counts, de-duplication, caching, and "have I seen this before?" in one pass.

**Real-world analogy.** A coat check: your ticket number (hash) tells the attendant exactly which hook holds your coat — no scanning the whole rack.

---

## Internal Working

```
key ──hash()──► raw integer ──% capacity──► bucket index ──► slot
```

1. **Hash function** turns a key into an integer (should distribute uniformly).
2. **Compression** maps it into `[0, capacity)` (usually `hash % capacity`, capacity often prime or a power of two).
3. **Collision resolution** handles two keys landing in the same bucket (chaining or open addressing — see [collision_handling.md](collision_handling.md)).

```
capacity 8, keys hashing to indices:
"cat"→3  "dog"→5  "bird"→3(collision)

bucket: 0  1  2   3          4  5      6  7
              ["cat","bird"]    ["dog"]
```

### Load factor & rehashing
**Load factor** α = n / capacity. As α grows, collisions rise and operations slow. When α exceeds a threshold (e.g., 0.75–1.0), the table **rehashes**: allocate a bigger array and re-insert everything — amortized O(1) but an occasional O(n) spike.

💡 A **good hash function** + a **low load factor** are what keep operations O(1) on average.

---

## Operations

| Operation | Average | Worst case |
|---|---|---|
| insert | O(1) | O(n) (all collide / rehash) |
| lookup | O(1) | O(n) |
| delete | O(1) | O(n) |
| iterate all | O(n + capacity) | — |

⚠️ Worst case is O(n) when many keys collide (bad hash or adversarial input) — trees give O(log n) *guaranteed*.

---

## C++ Implementation

### Using the STL
```cpp
#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string,int> freq;
freq["apple"]++;                       // insert/update
if (freq.count("apple")) { /* exists */ }
auto it = freq.find("apple");
if (it != freq.end()) it->second += 5;
freq.erase("apple");

std::unordered_set<int> seen;
seen.insert(42);
bool has = seen.count(42);             // membership in O(1) average
```

### Minimal chaining hash map (for understanding)
```cpp
#include <vector>
#include <list>
#include <utility>

template <typename K, typename V>
class HashMap {
    std::vector<std::list<std::pair<K,V>>> buckets;
    size_t count = 0;
    size_t idx(const K& k) const { return std::hash<K>{}(k) % buckets.size(); }
    void rehashIfNeeded() {
        if (count < buckets.size()) return;                 // load factor < 1
        std::vector<std::list<std::pair<K,V>>> old(buckets.size()*2);
        buckets.swap(old);
        count = 0;
        for (auto& chain : old) for (auto& kv : chain) put(kv.first, kv.second);
    }
public:
    HashMap(size_t cap=8): buckets(cap) {}
    void put(const K& k, const V& v) {
        for (auto& kv : buckets[idx(k)]) if (kv.first==k) { kv.second=v; return; }
        buckets[idx(k)].push_back({k,v}); ++count; rehashIfNeeded();
    }
    V* get(const K& k) {
        for (auto& kv : buckets[idx(k)]) if (kv.first==k) return &kv.second;
        return nullptr;
    }
    bool erase(const K& k) {
        auto& chain = buckets[idx(k)];
        for (auto it=chain.begin(); it!=chain.end(); ++it)
            if (it->first==k) { chain.erase(it); --count; return true; }
        return false;
    }
};
```

**STL note.** `std::unordered_map`/`unordered_set` (chaining) for hash tables; `std::map`/`std::set` (balanced BSTs) when you need **ordering**. Details & performance in [unordered_maps.md](unordered_maps.md).

**When to use hashing vs a tree.** Hash table for pure fast key lookup with no ordering needs; balanced tree when you need sorted iteration, range queries, or worst-case O(log n).

---

## Tradeoffs
- ✅ Average O(1) insert/lookup/delete; simple and ubiquitous.
- ❌ No ordering; O(n) worst case; iteration order is unspecified.
- ❌ Extra memory (buckets, load-factor slack, pointers for chaining).
- ❌ Needs a good hash function; poor hashing or adversarial keys degrade to O(n).
- **vs balanced BST:** hashing wins average speed; BST gives order + worst-case guarantees.

---

## Use Cases
- Dictionaries/maps, sets, symbol tables in compilers/interpreters.
- Frequency counting, de-duplication, group-by.
- Caching/memoization (see [advanced_hashing.md](advanced_hashing.md) for LRU).
- Two-sum-style "seen before" one-pass lookups.
- Database indexing (hash indexes), routing tables.

---

## Pitfalls
- ⚠️ Assuming O(1) **worst case** — it's O(1) *average*; collisions/rehash cause spikes.
- ⚠️ Using a mutable object as a key and then mutating it (breaks its hash/bucket).
- ⚠️ Relying on iteration order (there is none).
- ⚠️ Poor custom hash → clustering → O(n) behavior (and DoS via hash flooding).
- ⚠️ Storing pointers/iterators across a rehash (`unordered_map` invalidates iterators on rehash; references to values stay valid).
- ⚠️ Floating-point keys (precision) — avoid.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Have I seen this / does it exist / count occurrences" → **hash set/map**.
- "Find a pair/complement (two-sum)" → **hash map of seen values**.
- "Group anagrams / by key" → **hash map keyed by a canonical form**.
- "Deduplicate / distinct count" → **hash set**.
- "O(1) cache / memoize" → **hash map**.

---

## Example Problems

### Easy — Two Sum
```cpp
#include <unordered_map>
#include <vector>
std::vector<int> twoSum(std::vector<int>& nums, int target) {
    std::unordered_map<int,int> seen;                 // value → index
    for (int i = 0; i < (int)nums.size(); ++i) {
        auto it = seen.find(target - nums[i]);
        if (it != seen.end()) return {it->second, i};
        seen[nums[i]] = i;
    }
    return {};
}
```
**Explanation.** Store each value as you go; the complement lookup is O(1). One pass, O(n).

### Medium — Group Anagrams
Key each word by its sorted letters (or a 26-count signature); words with the same key are anagrams. O(N·K).
```cpp
// key = sorted(word); groups[key].push_back(word);
```
**Explanation.** A canonical form collapses anagrams to the same bucket.

### Hard — Longest Consecutive Sequence
Put all numbers in a hash set; for each number that starts a run (`n-1` absent), count upward. O(n).
```cpp
#include <unordered_set>
int longestConsecutive(std::vector<int>& nums) {
    std::unordered_set<int> s(nums.begin(), nums.end());
    int best = 0;
    for (int n : s) {
        if (s.count(n - 1)) continue;                 // not a sequence start
        int cur = n, len = 1;
        while (s.count(cur + 1)) { ++cur; ++len; }
        best = std::max(best, len);
    }
    return best;
}
```
**Explanation.** Only sequence starts trigger a walk, so each element is visited O(1) times → O(n) overall despite the inner loop.

---

*Next →* [collision_handling.md](collision_handling.md)
