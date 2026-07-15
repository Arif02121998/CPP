# Chapter 1 — Foundations

[← Back to Table of Contents](README.md)

---

## 1.1 What Are Data Structures & Algorithms?

A **data structure** is a way of **organizing and storing data** so that it can be accessed and modified efficiently. It is not just "where" data lives, but the *rules and relationships* that govern how the data is connected.

An **algorithm** is a **finite, well-defined sequence of steps** that transforms an input into a desired output. It is a recipe: unambiguous, terminating, and effective.

> 💡 **Data structures and algorithms are two halves of the same coin.** A data structure determines *what operations are cheap*; an algorithm exploits those cheap operations to solve a problem. Choosing the wrong structure forces an expensive algorithm.

**Analogy.** Think of a library:
- The **data structure** is how books are arranged (by shelf, by Dewey number, by author).
- The **algorithm** is the procedure you follow to find a book.
- If books are sorted (good structure), binary search (good algorithm) finds any book in seconds. If books are dumped in a pile, you must scan linearly.

---

## 1.2 Why They Matter — Efficiency & Scalability

Computers are fast, but data grows faster. The difference between a good and bad approach is not 2× — it is often the difference between **milliseconds and centuries**.

Consider searching `n = 1,000,000,000` items:

| Approach | Complexity | Steps | Real time (≈1e9 ops/sec) |
|---|---|---|---|
| Linear scan | O(n) | 1,000,000,000 | ~1 second |
| Binary search | O(log n) | ~30 | ~30 nanoseconds |

The *machine* didn't change. The *idea* did. This is why DSA matters more than raw hardware:

1. **Efficiency** — do more work with less time and memory.
2. **Scalability** — an O(n²) solution that works for 1,000 users collapses at 1,000,000. An O(n log n) solution scales smoothly.
3. **Cost** — fewer CPU cycles and less RAM = lower cloud bills and energy use.
4. **Feasibility** — some problems are *only* solvable with the right structure (e.g., real-time routing needs graphs + Dijkstra).

> 💡 Hardware gives you a constant-factor speedup. Better algorithms give you an *asymptotic* speedup, which always wins as data grows.

---

## 1.3 Time & Space Complexity

Complexity analysis lets us reason about performance **independent of hardware, language, or compiler**. We count how the number of operations (time) or memory (space) **grows as the input size `n` grows**.

### 1.3.1 Asymptotic Notation

We care about behavior as `n → ∞`, ignoring constants and lower-order terms because they wash out at scale.

| Notation | Name | Meaning | Bounds |
|---|---|---|---|
| **O(f(n))** | Big-O | **Upper** bound — "grows no faster than" | Worst case |
| **Ω(f(n))** | Big-Omega | **Lower** bound — "grows no slower than" | Best case |
| **Θ(f(n))** | Big-Theta | **Tight** bound — both upper and lower | Exact growth |

Formally:
- $f(n) = O(g(n))$ if there exist constants $c > 0, n_0$ such that $f(n) \le c \cdot g(n)$ for all $n \ge n_0$.
- $f(n) = \Omega(g(n))$ if $f(n) \ge c \cdot g(n)$ for all $n \ge n_0$.
- $f(n) = \Theta(g(n))$ if $f(n) = O(g(n))$ **and** $f(n) = \Omega(g(n))$.

> 💡 In interviews people say "Big-O" loosely to mean the tight bound. Technically, an O(n²) algorithm is *also* O(n³). Always state the **tightest** bound you can justify.

### 1.3.2 Why constants are dropped

`3n + 100` and `n` are both `O(n)`. For large `n`, the `3` and `100` are noise. But ⚠️ **constants matter in practice** — an O(n) algorithm with a huge constant can be slower than an O(n log n) one for realistic inputs. Asymptotics tell you *which wins eventually*, not always *which wins today*.

### 1.3.3 Common complexity classes (best → worst)

| Class | Name | Example |
|---|---|---|
| O(1) | Constant | Array index access, hash lookup (avg) |
| O(log n) | Logarithmic | Binary search, balanced BST operations |
| O(n) | Linear | Single loop over data |
| O(n log n) | Linearithmic | Merge sort, heap sort, good comparison sorts |
| O(n²) | Quadratic | Nested loops, bubble/insertion sort |
| O(n³) | Cubic | Naive matrix multiply, Floyd-Warshall |
| O(2ⁿ) | Exponential | Subsets, naive recursion (Fibonacci) |
| O(n!) | Factorial | Permutations, brute-force TSP |

Growth at `n = 20` vs `n = 1000`:

| n | log n | n | n log n | n² | 2ⁿ |
|---|---|---|---|---|---|
| 20 | ~4 | 20 | ~86 | 400 | ~1,000,000 |
| 1000 | ~10 | 1000 | ~10,000 | 1,000,000 | astronomically huge |

### 1.3.4 How to compute complexity — practical rules

1. **Sequential statements** add: `O(a) + O(b) = O(a + b)` → keep the dominant term.
2. **Loops** multiply by iteration count: a loop running `n` times with O(1) body is O(n).
3. **Nested loops** multiply: two nested `n`-loops → O(n²).
4. **Halving** gives a log: `for (i = n; i > 0; i /= 2)` → O(log n).
5. **Recursion** → use the recurrence / Master Theorem (below).

```cpp
// O(n) — single loop
int sum(const std::vector<int>& a) {
    int s = 0;
    for (int x : a) s += x;   // n iterations, O(1) each
    return s;
}

// O(n^2) — nested loops
void printPairs(const std::vector<int>& a) {
    for (size_t i = 0; i < a.size(); ++i)        // n
        for (size_t j = i + 1; j < a.size(); ++j) // n
            std::cout << a[i] << "," << a[j] << "\n";
}

// O(log n) — index halves each step
int countHalvings(int n) {
    int c = 0;
    while (n > 1) { n /= 2; ++c; }
    return c;
}
```

### 1.3.5 The Master Theorem (for divide & conquer)

For recurrences of the form $T(n) = a\,T(n/b) + f(n)$ where $a \ge 1, b > 1$:

Let $c = \log_b a$. Compare $f(n)$ with $n^c$:

1. If $f(n) = O(n^{c - \epsilon})$ → $T(n) = \Theta(n^c)$. *(leaf-dominated)*
2. If $f(n) = \Theta(n^c)$ → $T(n) = \Theta(n^c \log n)$. *(balanced)*
3. If $f(n) = \Omega(n^{c + \epsilon})$ → $T(n) = \Theta(f(n))$. *(root-dominated)*

**Examples:**
- Merge sort: $T(n) = 2T(n/2) + O(n)$ → $c = 1$, case 2 → **Θ(n log n)**.
- Binary search: $T(n) = T(n/2) + O(1)$ → $c = 0$, case 2 → **Θ(log n)**.
- Naive matrix multiply (Strassen base): $T(n)=8T(n/2)+O(n^2)$ → $c=3$, case 1 → **Θ(n³)**.

### 1.3.6 Space complexity

Counts **extra** memory beyond the input: auxiliary arrays, recursion stack, bookkeeping.

> ⚠️ Recursion is not free. Each call frame uses stack space. A recursion of depth `n` costs **O(n) space** even if it looks "simple." Deep recursion can cause stack overflow.

```cpp
// O(n) extra space: builds a new array
std::vector<int> doubled(const std::vector<int>& a) {
    std::vector<int> out;
    out.reserve(a.size());
    for (int x : a) out.push_back(x * 2);
    return out;             // auxiliary O(n)
}

// O(1) extra space: in-place
void doubleInPlace(std::vector<int>& a) {
    for (int& x : a) x *= 2; // no extra allocation
}
```

---

## 1.4 Amortized Analysis

**Amortized analysis** averages the cost of operations **over a sequence**, so that occasional expensive operations are "paid for" by many cheap ones. It is *not* average-case (which uses probability) — it is a **guaranteed average over any sequence**.

### Motivating example: `std::vector` push_back

A dynamic array doubles its capacity when full. Most `push_back`s are O(1), but a resize copies all `n` elements — O(n). Is `push_back` slow?

**No.** Consider `n` pushes starting from capacity 1, doubling each time. The total copy work across all resizes is:

$$1 + 2 + 4 + 8 + \dots + n \;\approx\; 2n = O(n)$$

Spread over `n` operations → **O(1) amortized per push_back.**

> 💡 **The doubling trick is everywhere** (`vector`, `unordered_map`, Java `ArrayList`, Python `list`). Growing by a *constant factor* (not a constant amount) is what gives O(1) amortized. Growing by +1 each time would be O(n) amortized — quadratic total.

### Three methods to prove amortized bounds

1. **Aggregate method** — total cost of `n` ops ÷ `n` (used above).
2. **Accounting (banker's) method** — overcharge cheap ops, store "credit" to pay for expensive ones. E.g., charge 3 units per push: 1 to insert, 2 saved to fund future copies.
3. **Potential method** — define a potential function Φ (stored energy in the structure); amortized cost = actual cost + ΔΦ.

```cpp
// Demonstration: reserve() avoids repeated reallocation.
std::vector<int> v;
v.reserve(1'000'000);          // one allocation up front
for (int i = 0; i < 1'000'000; ++i)
    v.push_back(i);            // every push is truly O(1) now
```

⚠️ Without `reserve`, the code still runs in O(n) total, but performs ~20 reallocations. `reserve` removes the constant-factor overhead.

---

## 1.5 Recursion vs Iteration

Both express **repetition**, but think differently.

| Aspect | Recursion | Iteration |
|---|---|---|
| Definition | Function calls itself on smaller input | Loop repeats until condition fails |
| State | Stored on the **call stack** | Stored in **loop variables** |
| Space | O(depth) stack frames | Usually O(1) |
| Readability | Natural for self-similar problems (trees, divide & conquer) | Natural for linear sweeps |
| Risk | Stack overflow, hidden cost | Can be verbose for nested structures |

### Anatomy of recursion

Every correct recursion needs:
1. **Base case(s)** — when to stop (no further recursion).
2. **Recursive case** — reduce toward the base case.
3. **Progress** — each call must move *closer* to the base, or it never terminates.

```cpp
// Recursive factorial — clear but uses O(n) stack
long long factR(int n) {
    if (n <= 1) return 1;        // base case
    return n * factR(n - 1);     // recursive case (progress: n-1)
}

// Iterative factorial — O(1) space
long long factI(int n) {
    long long r = 1;
    for (int i = 2; i <= n; ++i) r *= i;
    return r;
}
```

### Tail recursion

A call is **tail-recursive** if the recursive call is the *last* action. Compilers *may* optimize it into a loop (tail-call optimization, TCO), reusing one frame.

```cpp
long long factTail(int n, long long acc = 1) {
    if (n <= 1) return acc;
    return factTail(n - 1, acc * n);  // nothing happens after the call
}
```

> ⚠️ C++ does **not guarantee** TCO. Do not rely on it for deep recursion — convert to iteration or use an explicit stack when depth can exceed ~10⁴–10⁵.

### When to prefer which
- **Recursion:** trees, graphs (DFS), backtracking, divide & conquer, problems with a self-similar definition.
- **Iteration:** linear scans, tight performance loops, when stack depth would be dangerous.

💡 **Any recursion can be converted to iteration** using an explicit stack. Sometimes you *must* (e.g., DFS on a 10⁶-node graph) to avoid stack overflow.

---

## 1.6 Tradeoffs in Algorithm Design

There is rarely a "best" solution — only the best **for your constraints**. The recurring tensions:

### 1. Time vs Space
- A hash table trades **memory** for **O(1) lookups**.
- Recomputation saves memory but costs time; **memoization/caching** does the reverse.
- Example: detecting duplicates — sort in O(n log n)/O(1) space, *or* use a hash set in O(n) time / O(n) space.

### 2. Preprocessing vs Query
- Sorting once (O(n log n)) makes thousands of binary searches cheap (O(log n) each).
- Building a segment tree (O(n)) makes range queries O(log n) instead of O(n).
- 💡 If you query the same data many times, **invest in preprocessing**.

### 3. Simplicity vs Performance
- A clean O(n²) solution may be the right choice if `n ≤ 1000` and the code ships tomorrow.
- Premature optimization wastes effort and adds bugs. **Measure first.**

### 4. Worst-case vs Average-case
- Quicksort: O(n log n) average, O(n²) worst. Fast in practice but unsafe for adversarial input.
- Merge sort: O(n log n) guaranteed, but needs O(n) extra space.
- For real-time/safety-critical systems, prefer **predictable worst-case** bounds.

### 5. Generality vs Specialization
- A general structure (balanced BST) handles many operations; a specialized one (bitset, counting sort) is faster but narrower.

> 💡 **The senior-engineer skill is not memorizing algorithms — it's articulating tradeoffs and picking the right one for the constraints in front of you.** Always ask: *How big is n? How often do I query? Read-heavy or write-heavy? Worst-case guarantees needed? Memory budget?*

---

## Key Takeaways

- Complexity measures **growth**, abstracting away hardware — drop constants, keep the dominant term.
- **O / Ω / Θ** = upper / lower / tight bounds.
- **Amortized** = guaranteed average over a sequence; the doubling trick gives O(1) amortized appends.
- **Recursion** trades stack space for clarity; convert to iteration when depth is dangerous.
- Design is **tradeoffs**: time↔space, preprocessing↔query, simplicity↔speed, worst↔average.

---

*Next →* [Chapter 2: Core Data Structures](02_Core_Data_Structures.md)
