# File: Advanced/suffix_array.md

[← Repository Index](../README.md) · **Advanced:** [segment_tree](segment_tree.md) · [fenwick_tree](fenwick_tree.md) · suffix_array · [suffix_tree](suffix_tree.md) · [sparse_table](sparse_table.md) · [bit_manipulation](bit_manipulation.md) · [advanced_data_structures](advanced_data_structures.md)

---

## Conceptual Understanding

**Definition.** A **suffix array** is the sorted array of the starting indices of **all suffixes** of a string. Paired with the **LCP array** (longest common prefix between adjacent sorted suffixes), it answers a wide range of substring queries efficiently.

**Why it exists.** Suffix trees solve the same problems but use a lot of memory and are hard to implement. A suffix array captures most of that power in a **compact integer array** with simpler code — the practical choice for string indexing.

**Problem it solves.** Fast substring search, counting distinct substrings, longest repeated/common substring, and full-text indexing.

**Real-world analogy.** An alphabetically sorted list of every "tail" of a book; to check if a phrase appears, binary-search the sorted tails.

---

## Internal Working

For `s = "banana"` (append a sentinel `$` smaller than all letters):

```
Suffixes and sorted order:
idx  suffix
 5   a          → sorted:
 3   ana        SA = [5, 3, 1, 0, 4, 2]
 1   anana      (indices of suffixes in lexicographic order)
 0   banana
 4   na
 2   nana
```

- **Construction:** naive sort of suffixes is O(n² log n). The **prefix-doubling** method sorts by first 1, 2, 4, … characters using ranks → **O(n log²n)** (or O(n log n) with radix sort; O(n) via DC3/SA-IS).
- **LCP array (Kasai's algorithm):** computes longest common prefixes of adjacent sorted suffixes in **O(n)** using the inverse suffix array.

💡 Suffix array + LCP + a [sparse table](sparse_table.md) (for range-min on LCP) answers many queries in O(log n) or O(1).

---

## C++ Implementation — O(n log²n) Suffix Array + Kasai LCP

```cpp
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

std::vector<int> buildSuffixArray(const std::string& s) {
    int n = s.size();
    std::vector<int> sa(n), rank(n), tmp(n);
    std::iota(sa.begin(), sa.end(), 0);
    for (int i = 0; i < n; ++i) rank[i] = s[i];
    for (int k = 1; k < n; k <<= 1) {
        auto cmp = [&](int a, int b) {
            if (rank[a] != rank[b]) return rank[a] < rank[b];
            int ra = a + k < n ? rank[a + k] : -1;
            int rb = b + k < n ? rank[b + k] : -1;
            return ra < rb;
        };
        std::sort(sa.begin(), sa.end(), cmp);
        tmp[sa[0]] = 0;
        for (int i = 1; i < n; ++i)
            tmp[sa[i]] = tmp[sa[i-1]] + (cmp(sa[i-1], sa[i]) ? 1 : 0);
        rank = tmp;
        if (rank[sa[n-1]] == n - 1) break;          // all ranks distinct → done
    }
    return sa;
}

// Kasai's O(n) LCP: lcp[i] = LCP(sa[i], sa[i-1])
std::vector<int> buildLCP(const std::string& s, const std::vector<int>& sa) {
    int n = s.size();
    std::vector<int> rank(n), lcp(n, 0);
    for (int i = 0; i < n; ++i) rank[sa[i]] = i;
    int h = 0;
    for (int i = 0; i < n; ++i) {
        if (rank[i] > 0) {
            int j = sa[rank[i] - 1];
            while (i + h < n && j + h < n && s[i+h] == s[j+h]) ++h;
            lcp[rank[i]] = h;
            if (h) --h;                              // reuse for the next suffix
        } else h = 0;
    }
    return lcp;
}
```

**STL note.** No standard suffix array. The prefix-doubling version above is standard for interviews/competitive use; production text search may use SA-IS (linear) or specialized libraries.

**When to use suffix array vs suffix tree vs hashing.** Suffix array for compact, powerful static indexing; [suffix tree](suffix_tree.md) when you need explicit tree structure/online construction (Ukkonen); [rolling hash](../Hashing/advanced_hashing.md) for simpler one-off substring searches.

---

## Complexity

| Task | Time | Space |
|---|---|---|
| Build SA (prefix doubling) | O(n log²n) | O(n) |
| Build SA (radix / SA-IS) | O(n log n) / O(n) | O(n) |
| Kasai LCP | O(n) | O(n) |
| Substring search (binary) | O(m log n) | — |
| Distinct substrings | O(n) from LCP | — |

---

## Key Applications via SA + LCP
- **Substring search:** binary-search the pattern among sorted suffixes → O(m log n).
- **Number of distinct substrings:** $\frac{n(n+1)}{2} - \sum \text{lcp}[i]$ (total substrings minus repeats).
- **Longest repeated substring:** the maximum LCP value.
- **Longest common substring of two strings:** concatenate with a separator, build SA+LCP, find the max LCP between suffixes originating from different strings.
- **k-th smallest substring**, pattern occurrence counts (contiguous SA range).

---

## Tradeoffs
- ✅ Compact (integer arrays) and cache-friendly vs suffix trees.
- ✅ Powerful with LCP; simpler to implement than suffix trees.
- ❌ Construction is trickier than a hash-based search; linear-time builds (SA-IS/DC3) are complex.
- ❌ Static — not designed for incremental text edits (suffix tree/automaton handle online better).

---

## Use Cases
- Full-text search / indexing, bioinformatics (genome alignment).
- Longest repeated/common substring, plagiarism detection.
- Data compression (BWT/bzip2 uses suffix sorting).
- Pattern counting, string statistics.

---

## Pitfalls
- ⚠️ Forgetting the **sentinel** `$` (or off-by-one at suffix ends) → wrong ordering.
- ⚠️ Mismatched SA/LCP indexing conventions (`lcp[i]` = LCP of `sa[i]` and `sa[i-1]`).
- ⚠️ Naive O(n²log n) construction on large inputs → TLE; use doubling/radix.
- ⚠️ Kasai's `h` reuse (decrement by 1) is subtle — get the reset right.
- ⚠️ For longest common substring, ensure the separator is unique and outside the alphabet.

---

## Problem Patterns

🎯 **Recognition hints:**
- "Substring search in a fixed text, many queries" → **suffix array**.
- "Longest repeated substring / distinct substrings" → **SA + LCP**.
- "Longest common substring of two strings" → **concatenate + SA + LCP**.
- "k-th smallest substring / count occurrences" → **suffix array range**.

---

## Example Problems

### Medium — Longest Repeated Substring
Build SA + LCP; the answer length is `max(lcp)`, located at the corresponding suffix. O(n log²n).
**Explanation.** Adjacent sorted suffixes share the longest prefixes; the maximum LCP is the longest substring that repeats.

### Medium — Number of Distinct Substrings
`n(n+1)/2 − Σ lcp[i]`. O(n log²n) to build, O(n) to sum.
**Explanation.** Every suffix contributes new substrings equal to its length minus its overlap (LCP) with the previous sorted suffix.

### Hard — Longest Common Substring of Two Strings
Concatenate `A + '#' + B`, build SA + LCP, and take the max LCP between adjacent suffixes that start in **different** original strings. O((n+m) log²(n+m)).
**Explanation.** Cross-string adjacency in sorted order with a large LCP marks the longest shared substring.

---

*Next →* [suffix_tree.md](suffix_tree.md)
