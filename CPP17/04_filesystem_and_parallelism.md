# Chapter 4 — Filesystem & Parallel Algorithms (C++17)

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

---

## 4.1 `std::filesystem`

### What It Is

A complete, **portable filesystem library** (`<filesystem>`, namespace
`std::filesystem`, commonly aliased `namespace fs = std::filesystem;`) for
working with paths, files, and directories.

```cpp
#include <filesystem>
namespace fs = std::filesystem;

fs::path p = "/home/user/data.txt";
if (fs::exists(p))
    std::cout << "size: " << fs::file_size(p) << " bytes\n";

for (const auto& entry : fs::directory_iterator("/home/user"))
    std::cout << entry.path() << '\n';
```

### Why It Was Introduced

Before C++17 there was **no standard way** to manipulate files/directories. You
had to use:
- **Platform APIs** (`<dirent.h>`/POSIX on Linux, Win32 on Windows) — non-portable.
- **Boost.Filesystem** — third-party (which became the basis for this standard).

### The Problem Before (C++14)

```cpp
// C++14: list a directory — completely different code per OS
#ifdef _WIN32
    // WIN32_FIND_DATA, FindFirstFile, FindNextFile ...
#else
    // opendir, readdir, closedir ...
#endif
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Portability** | One API across Windows/Linux/macOS |
| **Safety** | Strong `path` type; error-code or exception variants |
| **Productivity** | Iteration, queries, and operations built-in |

### Core Capabilities

```cpp
namespace fs = std::filesystem;

// Path manipulation (no disk access)
fs::path p = "/a/b/file.txt";
p.filename();      // "file.txt"
p.stem();          // "file"
p.extension();     // ".txt"
p.parent_path();   // "/a/b"
p / "sub";         // path concatenation via operator/

// Queries
fs::exists(p);
fs::is_directory(p);
fs::is_regular_file(p);
fs::file_size(p);

// Operations
fs::create_directories("/a/b/c");
fs::copy("src", "dst");
fs::remove(p);
fs::remove_all("/a/b");                 // recursive delete
fs::rename("old", "new");
fs::current_path();                     // CWD

// Iteration
for (auto& e : fs::directory_iterator(".")) {}          // one level
for (auto& e : fs::recursive_directory_iterator(".")) {} // recurse
```

### Error Handling — Two Flavors

```cpp
// 1. Throwing version — throws fs::filesystem_error
try {
    fs::remove("/protected/file");
} catch (const fs::filesystem_error& e) {
    std::cerr << e.what();
}

// 2. Non-throwing version — fills an error_code
std::error_code ec;
fs::remove("/protected/file", ec);
if (ec) std::cerr << ec.message();
```

### Real-World Use Cases

- Cross-platform tools that scan/process directories.
- Build systems, asset pipelines, log rotation.
- Backup/sync utilities.

### Pitfalls

1. **Linking:** GCC 8 needs `-lstdc++fs`; older libc++ needs `-lc++fs`
   (GCC ≥ 9 / modern toolchains link it automatically).
2. **TOCTOU races:** `exists()` then open is racy — prefer attempting the
   operation and handling the error.
3. **Throwing vs non-throwing overloads** — pick consistently; the throwing form
   can surprise you in tight loops.
4. Paths are **OS-encoded**; be mindful of Unicode/encoding on Windows.

---

## 4.2 Parallel Algorithms (Execution Policies)

### What It Is

Most STL algorithms gained an optional first argument — an **execution policy** —
that requests **parallel and/or vectorized** execution.

```cpp
#include <algorithm>
#include <execution>
#include <vector>

std::vector<int> v(1'000'000);
// ...fill v...

std::sort(std::execution::par, v.begin(), v.end());        // parallel sort

auto total = std::reduce(std::execution::par,
                         v.begin(), v.end(), 0);           // parallel sum
```

### The Three (Four) Policies

| Policy | Meaning |
|--------|---------|
| `std::execution::seq` | Sequential (like the classic algorithm) |
| `std::execution::par` | **Parallel** across threads |
| `std::execution::par_unseq` | Parallel **and** vectorized (SIMD) |
| `std::execution::unseq` *(C++20)* | Vectorized, single thread |

### Why It Was Introduced

Before C++17, parallelizing an algorithm meant **manual threading** (`std::thread`,
thread pools) or third-party libraries (Intel TBB, OpenMP). The standard had no
turnkey parallelism.

### Before vs After

```cpp
// ─────────── C++14 — manual threads to parallelize a sum ───────────
// partition data, spawn threads, sum partials, join, combine... (lots of code)

// ─────────── C++17 — one argument ───────────
auto total = std::reduce(std::execution::par, v.begin(), v.end(), 0);
```

### `std::reduce` vs `std::accumulate`

`std::accumulate` is **strictly left-to-right** (not parallelizable).
`std::reduce` allows **reordering/regrouping**, enabling parallelism — but your
operation must be **associative and commutative** for a deterministic result.

```cpp
std::accumulate(v.begin(), v.end(), 0);                 // sequential, ordered
std::reduce(std::execution::par, v.begin(), v.end(), 0); // parallel, unordered
```

### How It Improves Things

- **Performance** on large datasets via multicore/SIMD with minimal code change.
- **Portability** — the standard, not a specific threading library.

### Pitfalls

1. **Overhead:** for small ranges, parallelism is *slower* (thread setup cost).
   Measure; only parallelize large workloads.
2. **Data races in your callable:** with `par`/`par_unseq`, your function object
   **must not** have unsynchronized shared mutable state.
3. **`par_unseq` forbids synchronization** (no mutexes/allocations) inside the
   callable — it may interleave SIMD lanes.
4. **Library/runtime support:** some standard libraries needed Intel TBB as a
   backend (e.g., older libstdc++ requires linking TBB).
5. **Exceptions** that escape an element function call `std::terminate`.

---

## 4.3 Other Notable Library Additions

### `std::apply` — Call a Function with a Tuple

```cpp
auto args = std::make_tuple(1, 2, 3);
auto sum = [](int a, int b, int c){ return a + b + c; };
int r = std::apply(sum, args);     // unpacks the tuple as arguments → 6
```

### `std::invoke` — Uniform Call Syntax

Calls anything callable (functions, member functions, member data, functors)
with one syntax — the foundation for generic "call this" code.

```cpp
struct S { int f(int x) { return x * 2; } int data = 7; };
S s;
std::invoke(&S::f, s, 21);   // 42   (member function)
std::invoke(&S::data, s);    // 7    (member data access)
std::invoke([](int x){ return x; }, 5); // 5 (lambda)
```

### `std::make_from_tuple`

Construct an object using tuple elements as constructor args.

```cpp
auto t = std::make_tuple(1, 2.0, "x");
auto obj = std::make_from_tuple<MyType>(t);
```

### `std::byte`

A distinct type for **raw memory** (not arithmetic) — clearer than `char`/
`unsigned char` for byte buffers.

```cpp
#include <cstddef>
std::byte b{0xFF};
b = b & std::byte{0x0F};        // bitwise ops allowed
int v = std::to_integer<int>(b);// explicit conversion to a number
```

### New Map/Set Member Functions

```cpp
std::map<std::string, int> m;

m.try_emplace("key", 42);          // insert only if absent; doesn't move args if present
m.insert_or_assign("key", 99);     // upsert: insert or overwrite, returns whether inserted

// Node handling — move nodes between containers without reallocating
auto node = m.extract("key");      // detach a node
other_map.insert(std::move(node)); // splice it in
m1.merge(m2);                      // splice all unique nodes from m2 into m1
```

### `std::clamp`, `std::gcd`, `std::lcm`

```cpp
std::clamp(value, lo, hi);   // confine value to [lo, hi]
std::gcd(12, 18);            // 6
std::lcm(4, 6);              // 12
```

---

## C++14 vs C++17 — Library/System Comparison

| Feature | C++14 | C++17 |
|---------|:-----:|:-----:|
| Filesystem | ❌ (platform/Boost) | ✅ `std::filesystem` |
| Parallel algorithms | ❌ (manual threads/TBB) | ✅ execution policies |
| `std::reduce` | ❌ | ✅ |
| `std::apply` / `std::invoke` | ❌ | ✅ |
| `std::byte` | ❌ | ✅ |
| `try_emplace` / `insert_or_assign` | ❌ | ✅ |
| node extract/merge | ❌ | ✅ |
| `std::clamp` / `gcd` / `lcm` | ❌ | ✅ |

---

## Chapter 4 Cheat Sheet

```cpp
namespace fs = std::filesystem;
for (auto& e : fs::recursive_directory_iterator(".")) {}
fs::create_directories("a/b/c");
fs::file_size(p);

std::sort(std::execution::par, v.begin(), v.end());
auto s = std::reduce(std::execution::par, v.begin(), v.end(), 0);

std::apply(f, tuple);            // call f with unpacked tuple
std::invoke(callable, args...);  // uniform call

m.try_emplace(k, v);             // insert if absent
m.insert_or_assign(k, v);        // upsert
std::clamp(x, lo, hi);
```

**Remember:**
- `std::filesystem` = portable file/dir ops; mind the link flag on old compilers.
- Execution policies parallelize STL algorithms — **only worth it for big data**.
- `std::reduce` needs an associative/commutative op (unlike `accumulate`).

---

## Common Mistakes

1. Parallelizing tiny ranges (overhead > benefit).
2. Shared mutable state in a `par`/`par_unseq` callable → data races.
3. Forgetting `-lstdc++fs`/TBB linkage on older toolchains.
4. Using `std::reduce` with a non-associative operation → non-deterministic results.
5. Relying on `exists()` before opening (TOCTOU race).

## When NOT to Use

- **Parallel policies** for small/medium datasets or I/O-bound work.
- **`par_unseq`** when your callable must lock or allocate.
- **`std::filesystem`** in extremely hot loops without caching results (syscalls).

---

*Next: [Chapter 5 — Interview Preparation Bank](05_interview_questions.md)*
