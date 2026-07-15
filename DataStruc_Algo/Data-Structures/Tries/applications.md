# File: Tries/applications.md

[← Repository Index](../README.md) · **Tries:** [trie](trie.md) · applications

---

## Conceptual Understanding

**Definition.** This file explores the **major applications** of tries: autocomplete engines, the **bitwise (binary) trie** for XOR problems, **board search** with DFS pruning, longest-prefix matching, and word-building problems.

**Why it matters.** The plain trie is simple; its power shows in these applied forms. Recognizing "this XOR problem is a binary trie" or "search all dictionary words at once with a trie" is what unlocks efficient solutions.

**Real-world analogy.** A prefix tree is the engine behind the search box that completes your query, the router that picks the most specific network route, and the game solver that finds every valid word on a board.

---

## 1. Autocomplete / Search Suggestions

Insert all terms; on a prefix, walk to the prefix node, then **DFS** to collect completions (optionally ranked by frequency). Returning the top-k suggestions is a trie walk + bounded DFS.
```cpp
#include <string>
#include <vector>
void collect(TrieNode* node, std::string& path, std::vector<std::string>& out) {
    if (out.size() >= 10) return;                 // top-k cap
    if (node->isEnd) out.push_back(path);
    for (int i = 0; i < 26; ++i)
        if (node->child[i]) {
            path.push_back('a' + i);
            collect(node->child[i], path, out);
            path.pop_back();                       // backtrack
        }
}
```
💡 To rank suggestions, store a frequency/score per word and use a heap, or a trie node that caches its best completions.

---

## 2. Bitwise (Binary) Trie — XOR Problems

Store integers as fixed-width **bit strings** (e.g., 32 bits) in a trie with two children (0/1). To **maximize XOR** with a query, greedily follow the **opposite** bit at each level.

```cpp
struct BinTrie {
    BinTrie* child[2] = {nullptr, nullptr};
};
void insert(BinTrie* root, int num) {
    BinTrie* cur = root;
    for (int b = 31; b >= 0; --b) {
        int bit = (num >> b) & 1;
        if (!cur->child[bit]) cur->child[bit] = new BinTrie();
        cur = cur->child[bit];
    }
}
int maxXor(BinTrie* root, int num) {
    BinTrie* cur = root; int res = 0;
    for (int b = 31; b >= 0; --b) {
        int bit = (num >> b) & 1;
        if (cur->child[bit ^ 1]) { res |= (1 << b); cur = cur->child[bit ^ 1]; } // take opposite
        else cur = cur->child[bit];
    }
    return res;
}
```
**Explanation.** Choosing the opposite bit whenever possible sets that bit in the XOR result, greedily maximizing it from the most significant bit down. O(32) per query.
**Use:** Maximum XOR of Two Numbers, Maximum XOR With an Element From Array, XOR-range queries.

---

## 3. Board Search with DFS Pruning — Word Search II

Insert all target words into a trie, then DFS the grid **once**. At each cell, descend the trie in lockstep; prune the moment the current path isn't a valid trie prefix. This beats running a separate search per word.
```cpp
// Sketch:
// build trie of words; DFS(r,c,node):
//   idx = board[r][c]-'a'; if (!node->child[idx]) return;   // prune
//   node = node->child[idx];
//   if (node->word != "") { collect node->word; node->word=""; } // avoid dupes
//   mark visited; recurse 4 neighbors; unmark.
```
💡 Storing the full word at its terminal node (instead of an `isEnd` flag) makes collection trivial; clearing it prevents duplicate results.
**Complexity:** O(cells × 4^L) worst case, but pruning makes it fast in practice.

---

## 4. Longest-Prefix Matching (IP routing)

Routers pick the **most specific** matching prefix for a destination IP. A **bitwise/radix trie** over address bits finds the longest matching prefix in O(address length). Also used in `Replace Words` (replace a word by its shortest dictionary root).
```cpp
// Replace Words: for each word, walk the trie until you hit an isEnd → that root replaces the word.
```

---

## 5. Word-Building Problems

- **Longest Word in Dictionary:** a word is buildable only if every prefix is also a word; DFS the trie keeping only paths where each node `isEnd`.
- **Concatenated / compound words:** trie + DP to test if a word splits into dictionary words.

---

## Complexity Summary

| Application | Time | Space |
|---|---|---|
| Autocomplete (prefix + collect k) | O(P + k·L) | O(total chars) |
| Max XOR (binary trie) | O(n·32) build, O(32)/query | O(n·32) |
| Word Search II | O(cells·4^L) worst, pruned in practice | O(total chars) |
| Longest-prefix match | O(address bits) | O(#prefixes·bits) |

---

## Tradeoffs
- ✅ Tries turn "many strings / prefixes / bits" problems into single-pass, prunable searches.
- ✅ Binary tries give elegant O(bits) XOR solutions vs O(n²) brute force.
- ❌ Memory overhead (many nodes); pointer chasing.
- ❌ For pure membership without prefixes, a hash set is lighter.

---

## Use Cases
- Search engines, IDE autocomplete, command-line completion.
- Networking: IP routing tables (longest-prefix match).
- Competitive programming: max-XOR, XOR subarray queries.
- Word games and dictionary/compound-word problems.
- Bioinformatics (k-mer indexing), text indexing.

---

## Pitfalls
- ⚠️ Word Search II: not pruning with the trie (defeats the purpose); not deduping results (clear the terminal word).
- ⚠️ Binary trie: wrong bit width or MSB-first order breaks XOR maximization.
- ⚠️ Autocomplete DFS without a cap → huge result sets; add a top-k limit.
- ⚠️ Forgetting to backtrack shared state (`path`, visited grid cells).
- ⚠️ Memory: use compressed/radix tries or hash-map children for large alphabets.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Suggestions / autocomplete / words with prefix" → **trie + DFS collect**.
- "Maximize/what's the max XOR" → **binary trie, greedily take opposite bit**.
- "Find all dictionary words on a grid" → **trie + DFS with pruning**.
- "Replace word by its root / shortest prefix" → **trie longest/shortest prefix**.
- "Build words letter by letter where each step is valid" → **trie path where every node isEnd**.

---

## Example Problems

### Medium — Maximum XOR of Two Numbers in an Array
Insert all numbers into a binary trie; for each number, `maxXor` finds its best partner. O(n·32).
**Explanation.** Greedy opposite-bit descent maximizes the XOR from the MSB down.

### Medium — Replace Words
Trie of roots; for each word, walk until an `isEnd` root is found and substitute it. O(total characters).

### Hard — Word Search II
Trie of the word list + single grid DFS with prefix pruning. Far faster than per-word search. O(cells·4^L) worst case, heavily pruned.
**Explanation.** One traversal matches all words simultaneously; the trie prunes impossible paths immediately.

---

*Next →* [../Disjoint-Set/union_find.md](../Disjoint-Set/union_find.md)
