# File: Tries/trie.md

[← Repository Index](../README.md) · **Tries:** trie · [applications](applications.md)

---

## Conceptual Understanding

**Definition.** A **trie** (prefix tree, pronounced "try") is a tree where each **edge** represents a character and each **path from the root** spells a prefix. Words share their common prefixes, so lookups depend on the **word length**, not the number of stored words.

**Why it exists.** For string collections, a hash set gives O(L) lookup but can't answer **prefix** questions ("all words starting with 'pre'"). A trie answers prefix/autocomplete queries naturally and stores shared prefixes once.

**Problem it solves.** Prefix search, autocomplete, dictionary membership, longest-prefix matching, and word-based board/puzzle search.

**Real-world analogy.** A dictionary organized by spelling: to find "cat" you follow c → a → t; "car" shares the c → a path and branches at the third letter.

---

## Internal Working

Each node has up to |Σ| children (26 for lowercase English) and an `isEnd` flag marking a complete word.

```
Insert "cat", "car", "dog":

        (root)
        /     \
       c       d
       |       |
       a       o
      / \      |
     t   r     g*
     *   *
(* = isEnd, a complete word ends here)
```

- **Insert(word):** walk/create a node per character; mark the last `isEnd`.
- **Search(word):** walk the characters; found iff the path exists **and** the final node is `isEnd`.
- **StartsWith(prefix):** walk the characters; true iff the path exists (ignore `isEnd`).

💡 The distinction between `search` (needs `isEnd`) and `startsWith` (path exists) is the whole point of a trie.

---

## Operations

| Operation | Time | Notes |
|---|---|---|
| insert | O(L) | L = word length |
| search | O(L) | independent of #words |
| startsWith | O(P) | P = prefix length |
| delete | O(L) | prune empty nodes |
| Space | O(total chars × Σ) | array children can be memory-heavy |

---

## C++ Implementation

### Array children (fast, fixed alphabet)
```cpp
struct TrieNode {
    TrieNode* child[26] = {nullptr};
    bool isEnd = false;
};

class Trie {
    TrieNode* root = new TrieNode();
public:
    void insert(const std::string& word) {
        TrieNode* cur = root;
        for (char c : word) {
            int i = c - 'a';
            if (!cur->child[i]) cur->child[i] = new TrieNode();
            cur = cur->child[i];
        }
        cur->isEnd = true;
    }
    bool search(const std::string& word) {
        TrieNode* n = walk(word);
        return n && n->isEnd;                 // must be a complete word
    }
    bool startsWith(const std::string& prefix) {
        return walk(prefix) != nullptr;       // path existence only
    }
private:
    TrieNode* walk(const std::string& s) {
        TrieNode* cur = root;
        for (char c : s) {
            int i = c - 'a';
            if (!cur->child[i]) return nullptr;
            cur = cur->child[i];
        }
        return cur;
    }
};
```

### Hash-map children (large/unknown alphabet, sparse)
```cpp
#include <unordered_map>
struct Node { std::unordered_map<char, Node*> child; bool isEnd = false; };
```
Use this when the alphabet is large (Unicode) or nodes are sparse — saves memory at the cost of a small constant.

**STL note.** No standard trie. Build it manually, or approximate prefix queries with a **sorted `std::vector`/`std::set` + `lower_bound`** (O(log n + matches) but without shared-prefix storage).

**When to use a trie vs a hash set.** Trie for **prefix** operations, ordered traversal, or many shared prefixes; hash set for pure membership (less memory, simpler).

---

## Tradeoffs
- ✅ O(L) operations independent of the number of words; natural prefix queries; sorted traversal (DFS yields lexicographic order).
- ✅ Shares common prefixes → can save space for prefix-heavy sets.
- ❌ **Memory-heavy** with 26-pointer array nodes (most pointers null); pointer chasing hurts cache locality.
- ❌ More code than a hash set; deletion needs pruning.
- **vs hash set:** hash set is smaller/simpler for membership but can't do prefixes; trie excels at prefixes/ordering.

---

## Use Cases
- Autocomplete / typeahead / search suggestions.
- Spell checkers and dictionaries.
- IP routing (longest-prefix match — often a bitwise/radix trie).
- Word games and board search (Boggle, Word Search II).
- T9 predictive text; prefix-based analytics.

---

## Space Optimizations
- **Compressed trie / radix tree (Patricia):** merge single-child chains into one edge with a substring → fewer nodes, less memory.
- **Ternary search trie:** three-way branching → memory between a trie and a BST.
- **DAWG (directed acyclic word graph):** also merge common **suffixes** for maximal sharing.

---

## Pitfalls
- ⚠️ Confusing `search` (requires `isEnd`) with `startsWith` (path only).
- ⚠️ Memory blowup from array nodes on large alphabets — use hash-map children or compression.
- ⚠️ Memory leaks — free nodes on destruction; deletion must prune now-empty branches carefully.
- ⚠️ Off-by-one in `c - 'a'` for non-lowercase input (validate/normalize the alphabet).
- ⚠️ Deleting a word that's a **prefix** of another must only clear `isEnd`, not remove shared nodes.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Autocomplete / words with prefix / startsWith" → **trie**.
- "Dictionary of words + repeated prefix lookups" → **trie**.
- "Word search on a board / match many words at once" → **trie + DFS**.
- "Longest prefix / longest word built from others" → **trie**.
- "Maximum XOR of two numbers" → **binary (bitwise) trie** ([applications.md](applications.md)).

---

## Example Problems

### Easy — Implement Trie (Prefix Tree)
`insert`, `search`, `startsWith` — exactly the class above. O(L) each.

### Medium — Design Add and Search Words (wildcard '.')
Support `.` matching any character via DFS branching at `.`.
```cpp
bool searchNode(const std::string& w, int i, TrieNode* node) {
    if (!node) return false;
    if (i == (int)w.size()) return node->isEnd;
    char c = w[i];
    if (c == '.') {
        for (int k = 0; k < 26; ++k)
            if (searchNode(w, i+1, node->child[k])) return true;   // try all branches
        return false;
    }
    return searchNode(w, i+1, node->child[c-'a']);
}
```
**Explanation.** A concrete letter follows one child; `.` recurses into all children. O(26^(#dots) · L) worst case.

### Hard — Word Search II
Put the dictionary in a trie, then DFS the board once, pruning whenever the current path isn't a trie prefix — far faster than searching each word separately. See [applications.md](applications.md).

---

*Next →* [applications.md](applications.md)
