# File: Hashing/advanced_hashing.md

[← Repository Index](../README.md) · **Hashing:** [hash_tables](hash_tables.md) · [collision_handling](collision_handling.md) · [unordered_maps](unordered_maps.md) · advanced_hashing

---

## Conceptual Understanding

**Definition.** This file covers **advanced hashing techniques**: rolling hashes (Rabin-Karp), polynomial string hashing, perfect & universal hashing, consistent hashing, Bloom filters, and hash-map-backed designs (LRU cache, O(1) random set).

**Why it matters.** Beyond basic dictionaries, hashing powers substring search, distributed systems, probabilistic membership, and elegant O(1) designs. These appear in system design and hard interview problems.

**Real-world analogy.** A fingerprint: a compact signature that identifies a large object (a string, a file, a server assignment) and lets you compare/route quickly.

---

## 1. Rolling Hash & Rabin-Karp

**Idea.** Hash a string as a polynomial in a base `b` mod a large prime `p`:
$$H(s) = (s_0 b^{k-1} + s_1 b^{k-2} + \dots + s_{k-1}) \bmod p$$
A **rolling** hash updates in O(1) when the window slides: drop the leading char, multiply by `b`, add the new char. This makes substring search **O(n + m)** average.

```cpp
#include <string>
#include <vector>
// Rabin-Karp: find pattern in text (average O(n+m))
int rabinKarp(const std::string& text, const std::string& pat) {
    const long long B = 131, MOD = 1000000007LL;
    int n = text.size(), m = pat.size();
    if (m > n) return -1;
    long long hp = 0, ht = 0, power = 1;
    for (int i = 0; i < m; ++i) {
        hp = (hp * B + pat[i]) % MOD;
        ht = (ht * B + text[i]) % MOD;
        if (i) power = (power * B) % MOD;
    }
    for (int i = 0; i + m <= n; ++i) {
        if (hp == ht && text.compare(i, m, pat) == 0) return i;   // verify to avoid false positive
        if (i + m < n) {
            ht = (ht - text[i] * power % MOD + MOD) % MOD;          // remove leading char
            ht = (ht * B + text[i + m]) % MOD;                      // add trailing char
        }
    }
    return -1;
}
```
⚠️ Always **verify** on a hash match (hashes can collide). Use two moduli/bases ("double hashing") to make collisions astronomically unlikely.
**Use:** substring search, detecting duplicate substrings, longest duplicate/common substring (with binary search on length).

---

## 2. Universal & Perfect Hashing

- **Universal hashing:** pick a hash function **at random** from a family so no fixed input can force worst-case collisions — defends against **hash-flooding DoS**. E.g., `h(x) = ((a·x + b) mod p) mod m` with random a, b.
- **Perfect hashing:** for a **static** key set, build a collision-free structure with O(1) **worst-case** lookup (two-level FKS scheme). Used for fixed keyword sets (compilers, `gperf`).

---

## 3. Consistent Hashing (distributed systems)

Maps both keys and servers onto a **hash ring**; a key belongs to the next server clockwise. Adding/removing a server only remaps `K/N` keys instead of nearly all — essential for **distributed caches, sharded databases, load balancers** (Cassandra, DynamoDB, CDNs). **Virtual nodes** smooth out load imbalance.

```
ring:  ...—[serverA]—key1—[serverB]—key2—[serverC]—...
adding serverD only steals the arc just before it.
```

---

## 4. Bloom Filter (probabilistic membership)

A bit array + `k` hash functions. `add` sets `k` bits; `contains` checks them.
- **No false negatives**, but **possible false positives**.
- Extremely space-efficient for "definitely not present / maybe present" checks.
**Use:** cache/DB pre-filters (skip disk if "definitely absent"), spell-checkers, URL/duplicate filters, network routing.
```
add("cat"): set bits h1,h2,h3
contains("dog"): if any of its bits is 0 → definitely absent; else → maybe present
```

---

## 5. Hash-Map-Backed Designs

### LRU Cache — O(1) get/put
Hash map (key→list iterator) + doubly linked list (most→least recently used).
```cpp
#include <unordered_map>
#include <list>
class LRUCache {
    int cap;
    std::list<std::pair<int,int>> order;                       // front = most recent
    std::unordered_map<int, std::list<std::pair<int,int>>::iterator> pos;
public:
    LRUCache(int c): cap(c) {}
    int get(int key) {
        auto it = pos.find(key);
        if (it == pos.end()) return -1;
        order.splice(order.begin(), order, it->second);        // move to front, O(1)
        return it->second->second;
    }
    void put(int key, int val) {
        auto it = pos.find(key);
        if (it != pos.end()) { it->second->second = val; order.splice(order.begin(), order, it->second); return; }
        if ((int)order.size() == cap) {                        // evict LRU
            pos.erase(order.back().first);
            order.pop_back();
        }
        order.push_front({key, val});
        pos[key] = order.begin();
    }
};
```
**Explanation.** The list tracks recency; the map gives O(1) node access; `splice` reorders in O(1).

### Insert Delete GetRandom O(1)
```cpp
#include <unordered_map>
#include <vector>
class RandomizedSet {
    std::unordered_map<int,int> idx;      // value → index in vals
    std::vector<int> vals;
public:
    bool insert(int v) {
        if (idx.count(v)) return false;
        idx[v] = vals.size(); vals.push_back(v); return true;
    }
    bool remove(int v) {
        auto it = idx.find(v);
        if (it == idx.end()) return false;
        int i = it->second, last = vals.back();
        vals[i] = last; idx[last] = i;    // move last into the hole
        vals.pop_back(); idx.erase(it);
        return true;
    }
    int getRandom() { return vals[rand() % vals.size()]; }
};
```
**Explanation.** The array enables O(1) random access; swapping the removed element with the last keeps it dense — all operations O(1).

---

## Complexity Summary

| Technique | Time | Space |
|---|---|---|
| Rabin-Karp | O(n+m) avg, O(nm) worst | O(1) |
| Universal hashing | O(1) expected | O(1) |
| Perfect hashing (static) | O(1) worst lookup | O(n) |
| Consistent hashing lookup | O(log N) (ring search) | O(N·vnodes) |
| Bloom filter add/query | O(k) | O(m bits) |
| LRU / RandomizedSet ops | O(1) | O(n) |

---

## Tradeoffs
- ✅ Enable substring search, distributed scaling, space-efficient membership, and O(1) designs.
- ❌ Rolling hash needs verification; Bloom filters allow false positives; consistent hashing adds ring-management complexity.
- **Bloom filter:** space vs false-positive rate (tune bits `m` and hash count `k`).

---

## Use Cases
- Rabin-Karp: plagiarism/duplicate detection, string search.
- Universal/perfect: DoS-resistant and static-keyword tables.
- Consistent hashing: caches, sharded DBs, load balancers, CDNs.
- Bloom filters: DB/cache pre-filters, network, spell-check.
- LRU/RandomizedSet: caches, in-memory stores, sampling.

---

## Pitfalls
- ⚠️ Rolling hash without verification → false matches; single modulus → collision attacks.
- ⚠️ Integer overflow in polynomial hashing (use `long long` and mod carefully; handle negative mod).
- ⚠️ Bloom filter: treating "maybe present" as "present"; can't delete from a plain Bloom filter (use a counting variant).
- ⚠️ Consistent hashing without virtual nodes → uneven load.
- ⚠️ LRU: forgetting to update recency on `get`, or evicting before checking capacity.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Find/compare substrings fast / duplicate substring" → **rolling hash / Rabin-Karp**.
- "O(1) get + put with eviction" → **hash map + linked list (LRU)**.
- "O(1) insert/delete/random" → **hash map + dynamic array**.
- "Massive set, approximate membership, tiny memory" → **Bloom filter**.
- "Distribute keys across servers with minimal reshuffling" → **consistent hashing**.

---

## Example Problems

### Medium — Repeated DNA Sequences
Rolling-hash all length-10 windows; report those seen more than once. O(n).

### Medium — LRU Cache
Exactly the design above — O(1) get/put.

### Hard — Longest Duplicate Substring
Binary search the length L; use Rabin-Karp to test whether any length-L substring repeats (hash set of window hashes). O(n log n) average.
**Explanation.** Duplicate-substring existence is monotonic in L, enabling binary search; rolling hashes make each length test linear.

---

*Next →* [../Tries/trie.md](../Tries/trie.md)
