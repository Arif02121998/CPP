# File: Advanced/suffix_tree.md

[← Repository Index](../README.md) · **Advanced:** [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · [suffix_array](suffix_array.md) · suffix_tree · [sparse_table](sparse_table.md) · [bit_manipulation](bit_manipulation.md) · [advanced_data_structures](advanced_data_structures.md)

---

## Conceptual Understanding

**Definition.** A **suffix tree** is a compressed trie ([Patricia tree](../Tries/trie.md)) of **all suffixes** of a string. Every suffix corresponds to a root-to-leaf path; common prefixes are shared, and single-child chains are compressed into edges labeled with substrings.

**Why it exists.** It answers substring queries in **O(m)** (pattern length, independent of text length) and supports many string problems in **linear** time. It's the most powerful string index — the price is memory and implementation complexity.

**Problem it solves.** Substring existence/counting, longest repeated/common substring, longest palindrome, and streaming/online text indexing — many in O(n) or O(m).

**Real-world analogy.** A master index of a book where every possible phrase-ending is pre-threaded, so locating any phrase is immediate regardless of book size.

---

## Internal Working

For `s = "banana$"` (sentinel `$` guarantees every suffix ends at a distinct leaf):

```
Edges labeled with substrings (compressed):

(root)
 ├─ a ──┬─ $                     (suffix "a")
 │      └─ na ──┬─ $             (suffix "ana")
 │              └─ na$           (suffix "anana")
 ├─ banana$                      (suffix "banana")
 ├─ na ──┬─ $                    (suffix "na")
 │       └─ na$                  (suffix "nana")
 └─ $
```

- **n leaves** (one per suffix), internal nodes only where suffixes diverge.
- Edges store substring ranges `(start, end)` into `s` — so the whole tree is **O(n)** space despite representing O(n²) substring characters.

### Construction
- Naive: insert each suffix into a trie → O(n²).
- **Ukkonen's algorithm:** builds the tree **online** in **O(n)** using suffix links, active points, and the "extension rules." Powerful but notoriously intricate.

💡 **Suffix links** (from an internal node spelling `xα` to the node spelling `α`) are what make Ukkonen linear and enable fast traversal.

---

## Operations & What They Enable

| Query | How | Time |
|---|---|---|
| substring exists? | walk the pattern down edges | O(m) |
| count occurrences | leaves under the match node | O(m + occ) |
| longest repeated substring | deepest internal node (≥2 leaves) | O(n) |
| longest common substring (k strings) | generalized suffix tree | O(total) |
| longest palindromic substring | suffix tree of `s + rev(s)` (+ LCA) | O(n) |

---

## C++ Note

A correct Ukkonen implementation is long (100+ lines) and beyond a concise snippet. **In practice, prefer a [suffix array + LCP](suffix_array.md)**, which is far simpler, more cache-friendly, and solves most of the same problems. Reach for a suffix tree (or **suffix automaton**) when you specifically need:
- explicit tree/automaton structure,
- **online** construction as characters stream in,
- linear-time generalized (multi-string) indexing.

```cpp
// Conceptual node (edges as [start, end) ranges into the text):
struct STNode {
    std::map<char, STNode*> children;   // or array for fixed alphabet
    int start; int* end;                // edge label = s[start..*end)
    STNode* suffixLink = nullptr;       // key to Ukkonen's linear time
};
```

**Suffix Automaton (SAM)** is a popular linear-size alternative: a DAG of the smallest automaton recognizing all substrings, built online in O(n), often easier than Ukkonen and great for counting distinct substrings and substring occurrences.

---

## Suffix Tree vs Suffix Array vs Suffix Automaton

| | Suffix tree | Suffix array | Suffix automaton |
|---|---|---|---|
| Space | O(n) but large constant | O(n) small | O(n) |
| Build | O(n) (Ukkonen, hard) | O(n log n)–O(n) | O(n) online |
| Substring query | O(m) | O(m log n) | O(m) |
| Online (append chars) | yes (Ukkonen) | no | yes |
| Ease | hard | easiest | medium |

💡 **Default to suffix array**; use suffix tree/automaton for online or structural needs.

---

## Complexity

| Task | Time | Space |
|---|---|---|
| Build (Ukkonen) | O(n) | O(n) (big constant) |
| Substring search | O(m) | — |
| Longest repeated substring | O(n) | — |
| Longest common substring (generalized) | O(total length) | O(total) |

---

## Tradeoffs
- ✅ O(m) queries and many linear-time string algorithms; online construction (Ukkonen).
- ❌ Large memory constant and very complex to implement correctly.
- ❌ Poorer cache behavior than a suffix array.
- **vs suffix array:** array is simpler/smaller and usually sufficient; tree/automaton win for online/structural cases.

---

## Use Cases
- Bioinformatics (genome/substring matching, alignment).
- Full-text search engines, data compression.
- Longest common/repeated substring, longest palindrome.
- Plagiarism/duplicate detection; streaming text indexing (Ukkonen/SAM).

---

## Pitfalls
- ⚠️ Missing sentinel `$` → some suffixes not represented by distinct leaves.
- ⚠️ Implementing Ukkonen without fully understanding active point / suffix links → subtle bugs.
- ⚠️ Storing edge labels as copied substrings (O(n²) memory) instead of `(start, end)` ranges.
- ⚠️ Choosing a suffix tree when a suffix array/automaton would be simpler and enough.
- ⚠️ Generalized trees: forgetting per-string end markers.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Substring queries in O(pattern length), possibly online" → **suffix tree / automaton**.
- "Longest repeated substring" → **deepest internal node** (or max-LCP via suffix array).
- "Longest common substring of multiple strings" → **generalized suffix tree/automaton**.
- "Count distinct substrings online" → **suffix automaton**.
- If it's static and one language: **prefer a suffix array**.

---

## Example Problems

### Medium — Longest Repeated Substring
The deepest internal node with ≥2 leaves gives it (or `max(LCP)` via a suffix array). O(n) build.
**Explanation.** An internal node represents a prefix shared by all suffixes beneath it — depth = repeat length.

### Hard — Longest Common Substring of Multiple Strings
Build a **generalized** suffix tree/automaton over all strings; find the deepest node whose subtree contains a leaf from **every** string. O(total length).
**Explanation.** Such a node spells a substring present in all inputs; the deepest one is the longest.

### Hard — Count Distinct Substrings (online)
A **suffix automaton** counts distinct substrings incrementally as characters are appended, in O(n).
**Explanation.** Each new SAM state contributes `len(state) − len(link(state))` new distinct substrings.

---

*Next →* [sparse_table.md](sparse_table.md)
