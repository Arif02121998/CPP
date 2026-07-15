# File: Hashing/unordered_maps.md

[← Repository Index](../README.md) · **Hashing:** [hash_tables](hash_tables.md) · [collision_handling](collision_handling.md) · unordered_maps · [advanced_hashing](advanced_hashing.md)

---

## Conceptual Understanding

**Definition.** This file is the **practical C++ guide** to the standard hash-based containers: `std::unordered_map`, `std::unordered_set`, and their `multi` variants — how they work, how to use them well, and how to hash custom types.

**Why it matters.** These are the workhorse containers for O(1)-average lookups in real C++ code and interviews. Knowing their guarantees, iterator invalidation rules, and how to supply custom hashers separates correct, fast code from subtle bugs.

**Real-world analogy.** A well-labeled filing cabinet: instant retrieval by label, but the folders aren't in any sorted order.

---

## The STL Hash Containers

| Container | Stores | Duplicates | Ordered? |
|---|---|---|---|
| `std::unordered_set<K>` | keys | no | no |
| `std::unordered_map<K,V>` | key→value | no (unique keys) | no |
| `std::unordered_multiset<K>` | keys | yes | no |
| `std::unordered_multimap<K,V>` | key→value | yes | no |

All use **separate chaining** internally with a load-factor-driven rehash. Contrast with the **ordered** `std::map/std::set` (red-black trees, O(log n), sorted) — see [../Trees/balanced_trees.md](../Trees/balanced_trees.md).

---

## Internal Working (libstdc++/libc++)

- Buckets hold singly-linked chains of nodes.
- **`load_factor() = size / bucket_count`**; when it exceeds **`max_load_factor()`** (default 1.0), the table rehashes to more buckets and redistributes nodes.
- **`reserve(n)`** / **`rehash(b)`** pre-size the table to avoid repeated rehashing.

```
unordered_map<string,int>:
  hash("go") % bucket_count → bucket 5 → node("go",1)
```

---

## Essential Operations & Idioms

```cpp
#include <unordered_map>
#include <unordered_set>
#include <string>

std::unordered_map<std::string,int> m;

// insert / update
m["a"] = 1;                       // creates or overwrites
m.emplace("b", 2);                // constructs in place, no overwrite if exists
++m["c"];                         // default-constructs 0 then increments (handy for counting)

// lookup — prefer find/count over operator[] for reads
auto it = m.find("a");
if (it != m.end()) { /* it->second */ }
bool has = m.count("a");          // 0 or 1

// C++17 structured bindings + insert result
if (auto [pos, inserted] = m.emplace("d", 4); inserted) { /* newly added */ }

// iterate (order is unspecified)
for (const auto& [k, v] : m) { /* ... */ }

// erase
m.erase("a");

// performance: pre-size to avoid rehashes
m.reserve(10000);
```

⚠️ **`operator[]` inserts a default value if the key is absent** — using it inside a read (e.g., `if (m[k] == ...)`) silently creates entries. Use `find`/`count` for pure lookups.

---

## Hashing Custom Types

To key by a user type, provide a hash (and equality). Combine field hashes carefully (don't just XOR raw hashes — that collides on symmetric values).
```cpp
#include <unordered_set>
struct Point { int x, y; bool operator==(const Point& o) const { return x==o.x && y==o.y; } };

struct PointHash {
    size_t operator()(const Point& p) const {
        size_t h1 = std::hash<int>{}(p.x);
        size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1<<6) + (h1>>2));  // boost-style mix
    }
};
std::unordered_set<Point, PointHash> pts;
```
💡 For `pair`/`tuple` keys there's no default hash — supply one, or encode into a single integer (e.g., `x * BIG + y`) when ranges are bounded.

---

## Iterator & Reference Invalidation

| Action | Iterators | References/pointers to elements |
|---|---|---|
| insert causing **rehash** | invalidated | **valid** (nodes are stable) |
| insert without rehash | valid | valid |
| erase(element) | only that element's | only that element's |

💡 Unlike `vector`, element **references stay valid** across rehash (node-based), but **iterators do not**. Don't cache iterators across inserts.

---

## Complexity

| Operation | Average | Worst |
|---|---|---|
| find / insert / erase | O(1) | O(n) (degenerate hashing) |
| iterate | O(n + bucket_count) | — |

---

## unordered_map vs map

| | `unordered_map` | `map` |
|---|---|---|
| Backing | hash table | red-black tree |
| Complexity | O(1) average | O(log n) guaranteed |
| Ordering | none | sorted by key |
| Range/predecessor queries | no | yes (`lower_bound`) |
| Iteration order | unspecified | ascending |
| Custom key needs | hash + `==` | `<` (comparator) |

💡 Choose `unordered_map` for raw lookup speed; `map` when you need order, range queries, or worst-case guarantees.

---

## Tradeoffs
- ✅ O(1)-average operations; the default choice for dictionaries/sets in C++.
- ❌ No ordering; O(n) worst case; higher memory than a sorted vector.
- ❌ Custom types need a correct hash; poor hashing kills performance.
- ❌ Iterator invalidation on rehash surprises beginners.

---

## Use Cases
- Frequency maps, seen-sets, memoization, adjacency maps for graphs.
- De-duplication, grouping, indexing by id/string.
- Caches (with additional structure for eviction — [advanced_hashing.md](advanced_hashing.md)).

---

## Pitfalls
- ⚠️ `operator[]` accidentally inserting during reads.
- ⚠️ Forgetting a hash/`==` for custom keys (won't compile) or writing a weak hash (slow).
- ⚠️ Caching iterators across insertions (invalidated on rehash).
- ⚠️ Relying on iteration order.
- ⚠️ Not `reserve()`-ing when the size is known → repeated rehashes.
- ⚠️ Hash-flooding DoS with untrusted string keys.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Count / group / dedupe / seen-before" → **unordered_map/set**.
- "Need sorted order or range queries too" → use **map/set** instead.
- "Key is a pair/tuple/struct" → provide a **custom hash**.
- "Known large size up front" → **reserve** to avoid rehashing.

---

## Example Problems

### Easy — First Unique Character in a String
Count with an `unordered_map` (or `array<int,26>`), then find the first count-1 character. O(n).

### Medium — Subarray Sum Equals K
Hash map of prefix-sum frequencies; for each prefix, add how many earlier prefixes equal `sum − k`. O(n).
```cpp
#include <unordered_map>
int subarraySum(std::vector<int>& nums, int k) {
    std::unordered_map<int,int> cnt{{0,1}};
    int sum = 0, res = 0;
    for (int x : nums) { sum += x; res += cnt[sum - k]; ++cnt[sum]; }
    return res;
}
```
**Explanation.** `sum − k` seen before means a subarray ending here sums to k; the map gives those counts in O(1). O(n).

### Hard — Insert Delete GetRandom O(1)
`unordered_map<val,index>` + `vector<val>`; delete swaps with the last element to keep the array dense. All O(1) — full code in [advanced_hashing.md](advanced_hashing.md).

---

*Next →* [advanced_hashing.md](advanced_hashing.md)
