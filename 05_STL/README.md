<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 5</samp>
</p>

<h1 align="center">📘 The Standard Template Library (STL)</h1>

<p align="center"><em>"Don't reinvent the wheel.<br/>C++ already has a world-class toolkit."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | `std::vector` — The Dynamic Array | [§1](#-1-stdvector--the-dynamic-array) |
| 2 | `std::map` & `std::set` | [§2](#-2-stdmap--stdset) |
| 3 | Iterators | [§3](#-3-iterators) |
| 4 | Algorithms | [§4](#-4-algorithms) |
| 5 | Other Containers | [§5](#-5-other-containers) |

> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. `std::vector` — The Dynamic Array

> *📄 See full code → `01_vector.cpp`*

### The Concept

A `vector` is a **resizable array**. Unlike raw arrays (`int arr[5]`) which have a fixed size, a `vector` grows automatically when you add elements. It's the most commonly used container in C++.

```
  Regular array:  [10][20][30][40][50]  ← Fixed. Can't add a 6th.
  
  vector:         [10][20][30][40][50][  ][  ][  ]
                   ▲ size = 5           ▲ capacity = 8
                   (elements used)       (memory reserved)
```

When you `push_back` a 6th element, it fits into the reserved space. When capacity runs out, the vector allocates a **bigger chunk** (typically 2x), copies everything over, and frees the old memory. This makes `push_back` **O(1) amortized** — fast on average.

### Example

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> v;

    // ── Adding elements ──
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);
    v.emplace_back(40);           // Constructs in-place (slightly faster)

    // ── Access ──
    std::cout << "First:  " << v.front()  << '\n';   // 10
    std::cout << "Last:   " << v.back()   << '\n';   // 40
    std::cout << "Index 2: " << v[2]      << '\n';   // 30
    std::cout << "Safe:   " << v.at(2)    << '\n';   // 30 (throws if out of bounds)

    // ── Metadata ──
    std::cout << "Size:     " << v.size()     << '\n';   // 4
    std::cout << "Capacity: " << v.capacity() << '\n';   // ≥ 4

    // ── Iterate ──
    for (const auto& x : v) {
        std::cout << x << " ";
    }
    std::cout << '\n';   // 10 20 30 40

    // ── Remove ──
    v.pop_back();                 // Remove last
    v.erase(v.begin());          // Remove first

    // ── Sort ──
    v = {5, 3, 8, 1, 9, 2};
    std::sort(v.begin(), v.end());
    for (auto x : v) std::cout << x << " ";   // 1 2 3 5 8 9
    std::cout << '\n';
}
```

### Quick-Reference

| Operation | Code | Time |
|-----------|------|:----:|
| Add to end | `v.push_back(x)` | O(1)* |
| Remove from end | `v.pop_back()` | O(1) |
| Access by index | `v[i]` or `v.at(i)` | O(1) |
| Insert in middle | `v.insert(pos, x)` | O(n) |
| Find | `std::find(v.begin(), v.end(), x)` | O(n) |
| Sort | `std::sort(v.begin(), v.end())` | O(n log n) |

*amortized

---

## 🔹 2. `std::map` & `std::set`

> *📄 See full code → `02_map_set.cpp`*

### The Concept

- **`std::map<Key, Value>`** — A dictionary. Every key maps to a value. Keys are **sorted** and **unique**. Internally a red-black tree → O(log n) lookups.
- **`std::set<T>`** — A sorted bag of **unique** values. Like a guest list — each name appears at most once.
- **`std::unordered_map`** — Hash table version → O(1) average lookups. Unordered.

### Example

```cpp
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <string>

int main() {
    // ── std::map ──
    std::map<std::string, int> ages;
    ages["Alice"] = 30;
    ages["Bob"]   = 25;
    ages.insert({"Charlie", 35});

    for (const auto& [name, age] : ages) {        // C++17 structured bindings
        std::cout << name << " is " << age << '\n';
    }
    // Output (sorted by key):
    // Alice is 30
    // Bob is 25
    // Charlie is 35

    if (ages.count("Eve") == 0) {
        std::cout << "Eve not found\n";
    }

    // ── std::set ──
    std::set<int> numbers = {5, 3, 8, 1, 3, 5};   // Duplicates removed!
    for (int n : numbers) std::cout << n << " ";    // 1 3 5 8
    std::cout << '\n';

    // ── std::unordered_map (hash map, O(1) average) ──
    std::unordered_map<std::string, double> prices = {
        {"apple", 1.50}, {"banana", 0.75}
    };
    std::cout << "Apple: $" << prices["apple"] << '\n';
}
```

### When to Use What?

| Container | Sorted? | Duplicates? | Lookup | Use When |
|:---------:|:-------:|:-----------:|:------:|----------|
| `map` | ✅ | Keys unique | O(log n) | Need sorted keys |
| `unordered_map` | ❌ | Keys unique | O(1) avg | Need fastest lookup |
| `set` | ✅ | No duplicates | O(log n) | Unique sorted collection |
| `multimap` | ✅ | Duplicate keys OK | O(log n) | Multiple values per key |

---

## 🔹 3. Iterators

> *📄 See full code → `03_iterators.cpp`*

### The Concept

An **iterator** is a **bookmark** that points to an element in a container. You can move forward (`++it`), read the current element (`*it`), and check if you've reached the end.

Iterators are the **bridge** between containers and algorithms. `std::sort` doesn't know about `vector` — it just knows about iterators. This is why the same `sort` works on vectors, arrays, deques, and lists.

### Example

```cpp
#include <iostream>
#include <vector>
#include <map>

int main() {
    std::vector<int> vec = {10, 20, 30, 40, 50};

    // ── Forward iteration ──
    std::cout << "Forward: ";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << '\n';   // 10 20 30 40 50

    // ── Reverse iteration ──
    std::cout << "Reverse: ";
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << '\n';   // 50 40 30 20 10

    // ── Random access (vectors & arrays) ──
    auto it = vec.begin();
    std::cout << "Third element: " << *(it + 2) << '\n';   // 30

    // ── Map iterators ──
    std::map<std::string, int> m = {{"a", 1}, {"b", 2}, {"c", 3}};
    for (auto it = m.begin(); it != m.end(); ++it) {
        std::cout << it->first << ":" << it->second << " ";
    }
    std::cout << '\n';   // a:1 b:2 c:3
}
```

### Iterator Type Reference

| Type | Movement | Supports | Example Containers |
|------|:--------:|----------|-------------------|
| **Input** | Forward only, read-only | `*it`, `++it` | `istream_iterator` |
| **Forward** | Forward only | Read/write | `forward_list`, `unordered_map` |
| **Bidirectional** | Forward & backward | `++it`, `--it` | `list`, `map`, `set` |
| **Random Access** | Jump anywhere | `it + n`, `it[n]` | `vector`, `deque`, `array` |

> 💡 **Pro tip:** In modern C++, prefer range-for (`for (auto& x : v)`) over manual iterators. Only use iterators when you need the position, or algorithms require them.

---

## 🔹 4. Algorithms

> *📄 See full code → `04_algorithms.cpp`*

### The Concept

The `<algorithm>` header provides **80+ ready-made algorithms** that work on any container via iterators. Sorting, searching, counting, transforming, removing — it's all there. Never write a manual loop when an algorithm exists.

### Example

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

int main() {
    std::vector<int> v = {5, 3, 8, 1, 9, 2, 7, 4, 6};

    // ── SORTING ──
    std::sort(v.begin(), v.end());                               // Ascending
    // v = {1, 2, 3, 4, 5, 6, 7, 8, 9}

    std::sort(v.begin(), v.end(), std::greater<int>());          // Descending
    // v = {9, 8, 7, 6, 5, 4, 3, 2, 1}

    std::sort(v.begin(), v.end(), [](int a, int b) {             // Custom
        return a % 2 < b % 2;     // Evens first
    });

    // ── SEARCHING ──
    auto it = std::find(v.begin(), v.end(), 5);
    if (it != v.end()) std::cout << "Found 5 at position " << (it - v.begin()) << '\n';

    auto it2 = std::find_if(v.begin(), v.end(), [](int x) { return x > 7; });

    // ── COUNTING ──
    int evens = std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    std::cout << "Even count: " << evens << '\n';

    // ── TRANSFORM (apply function to each element) ──
    std::vector<int> doubled(v.size());
    std::transform(v.begin(), v.end(), doubled.begin(), [](int x) { return x * 2; });

    // ── ACCUMULATE (sum, product, etc.) ──
    int total = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "Sum: " << total << '\n';

    // ── ERASE-REMOVE IDIOM (remove all 2s) ──
    std::vector<int> data = {1, 2, 3, 2, 4, 2, 5};
    data.erase(std::remove(data.begin(), data.end(), 2), data.end());
    // data = {1, 3, 4, 5}

    // ── PREDICATES ──
    bool allPositive = std::all_of(v.begin(), v.end(), [](int x) { return x > 0; });
    bool anyEven     = std::any_of(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
}
```

### Most-Used Algorithms Cheat Sheet

| Algorithm | What It Does | Example |
|-----------|-------------|---------|
| `sort` | Sort a range | `sort(v.begin(), v.end())` |
| `find` / `find_if` | Find an element | `find(v.begin(), v.end(), 5)` |
| `count` / `count_if` | Count matches | `count_if(v.begin(), v.end(), pred)` |
| `transform` | Apply function to all | `transform(in.begin(), in.end(), out.begin(), f)` |
| `accumulate` | Reduce to single value | `accumulate(v.begin(), v.end(), 0)` |
| `remove` / `remove_if` | Move unwanted to end | Use with `.erase()` |
| `all_of` / `any_of` / `none_of` | Boolean predicates | `all_of(v.begin(), v.end(), pred)` |
| `min_element` / `max_element` | Find min/max | `*max_element(v.begin(), v.end())` |
| `reverse` | Reverse in-place | `reverse(v.begin(), v.end())` |
| `unique` | Remove consecutive duplicates | Use with `.erase()` after sorting |

---

## 🔹 5. Other Containers

> *📄 See full code → `05_other_containers.cpp`*

### The Concept

The STL provides containers for every common data structure. Choose based on your access pattern:

### Container Overview

```
  ┌──────────────┐   ┌─────────────────┐   ┌──────────────┐
  │   vector     │   │     deque       │   │     list     │
  │ [1][2][3][4] │   │ [?][1][2][3][?] │   │ 1↔2↔3↔4     │
  │ Fast: back   │   │ Fast: both ends │   │ Fast: insert │
  │ Slow: front  │   │ Random access   │   │ No random    │
  └──────────────┘   └─────────────────┘   └──────────────┘

  ┌──────────────┐   ┌─────────────────┐   ┌──────────────┐
  │    stack     │   │     queue       │   │ priority_q   │
  │     [3]      │   │ IN→[1][2][3]→  │   │    [50]      │
  │     [2]      │   │     OUT         │   │   [30][10]   │
  │     [1]      │   │ FIFO            │   │   Max first  │
  │    LIFO      │   │                 │   │              │
  └──────────────┘   └─────────────────┘   └──────────────┘
```

### Example

```cpp
#include <iostream>
#include <array>
#include <deque>
#include <list>
#include <stack>
#include <queue>

int main() {
    // ── std::array (fixed-size, stack-allocated) ──
    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    std::cout << "array size: " << arr.size() << '\n';   // 5

    // ── std::deque (fast at BOTH ends) ──
    std::deque<int> dq = {2, 3, 4};
    dq.push_front(1);         // O(1) at front — vector can't do this!
    dq.push_back(5);          // O(1) at back
    // dq = {1, 2, 3, 4, 5}

    // ── std::stack (LIFO — Last In, First Out) ──
    std::stack<int> stk;
    stk.push(10);
    stk.push(20);
    stk.push(30);
    std::cout << "Top: " << stk.top() << '\n';   // 30
    stk.pop();                                     // Removes 30

    // ── std::queue (FIFO — First In, First Out) ──
    std::queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);
    std::cout << "Front: " << q.front() << '\n';   // 10
    q.pop();                                         // Removes 10

    // ── std::priority_queue (max-heap by default) ──
    std::priority_queue<int> pq;
    pq.push(30);
    pq.push(10);
    pq.push(50);
    while (!pq.empty()) {
        std::cout << pq.top() << " ";   // 50 30 10 (largest first)
        pq.pop();
    }
    std::cout << '\n';
}
```

### When to Use What?

| Need | Container |
|------|-----------|
| Dynamic array, random access | `std::vector` |
| Fixed-size array, no heap allocation | `std::array` |
| Fast insert/remove at both ends | `std::deque` |
| Fast insert/remove anywhere | `std::list` |
| LIFO (undo stack, DFS) | `std::stack` |
| FIFO (BFS, task queue) | `std::queue` |
| Always get max/min first | `std::priority_queue` |
| Key→Value lookup | `std::map` or `std::unordered_map` |
| Unique sorted values | `std::set` |

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 6: C++11 — The Modern Revolution</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
