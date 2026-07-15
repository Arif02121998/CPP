# Chapter 7 — STL & Library Additions (C++11)

> *Level: Intermediate*
> Back to [Table of Contents](README.md)

Beyond the headline features, C++11 enriched the standard library with new
containers, utilities, and whole new subsystems.

---

## 7.1 Unordered (Hash) Containers

### What They Are

Hash-table-based containers offering **average O(1)** lookup/insert/erase —
versus the **O(log n)** of the tree-based `map`/`set`.

```cpp
#include <unordered_map>
#include <unordered_set>

std::unordered_map<std::string, int> ages;
ages["Alice"] = 30;                 // average O(1) insert
int a = ages["Alice"];              // average O(1) lookup

std::unordered_set<int> seen;
seen.insert(42);
if (seen.count(42)) { /* found */ }
```

### `map` vs `unordered_map`

| | `std::map` | `std::unordered_map` |
|--|-----------|----------------------|
| Structure | Balanced tree | Hash table |
| Lookup | O(log n) | **O(1) average**, O(n) worst |
| Ordering | **Sorted** by key | **Unordered** |
| Use when | Need sorted iteration / ranges | Need fastest lookup, order irrelevant |

### Pitfalls

- Worst-case O(n) on many hash collisions.
- No ordering — don't rely on iteration order.
- Custom key types need a `std::hash` specialization (and `operator==`).

---

## 7.2 `std::array` — Fixed-Size Array

### What It Is

A thin, zero-overhead wrapper over a C array with a **size known at compile time**
and the full container interface.

```cpp
#include <array>

std::array<int, 4> a{1, 2, 3, 4};
a.size();          // 4 (knows its own size — unlike a raw array)
a.at(2);           // bounds-checked access
for (int x : a) {} // works with range-for and algorithms
```

### Why Over a C Array?

```cpp
int raw[4];               // decays to a pointer; loses size; no .begin()/.size()
std::array<int, 4> arr;   // keeps size; STL-compatible; no decay; bounds checking
```

Same performance as a raw array, but safe and STL-friendly.

---

## 7.3 `std::tuple` — Heterogeneous Fixed Collection

### What It Is

A fixed-size collection of values of **different types**.

```cpp
#include <tuple>

std::tuple<int, std::string, double> t(1, "hello", 3.14);

std::get<0>(t);                       // 1     (access by index)
std::get<1>(t);                       // "hello"

auto t2 = std::make_tuple(2, "x", 1.5);

// Unpack with std::tie (structured bindings came in C++17)
int i; std::string s; double d;
std::tie(i, s, d) = t;                // distribute fields into variables
std::tie(std::ignore, s, std::ignore) = t; // skip fields with std::ignore
```

### Use Cases

- Returning **multiple values** from a function (pre-structured-bindings).
- Generic code over heterogeneous data.

> C++17 **structured bindings** make tuple/pair unpacking far cleaner — see the
> [C++17 book](../CPP17/01_core_language_features.md).

---

## 7.4 `std::initializer_list` — Brace Initialization Support

### What It Is

The mechanism that lets your own types accept a `{...}` list, like the standard
containers do.

```cpp
#include <initializer_list>

class IntBag {
    std::vector<int> data_;
public:
    IntBag(std::initializer_list<int> list)   // enables brace-init
        : data_(list) {}
};

IntBag b{1, 2, 3, 4};            // calls the initializer_list constructor
std::vector<int> v{1, 2, 3};     // this is how vector supports {…}
```

### Pitfall — Constructor Ambiguity

When a class has both an `initializer_list` constructor and others, braces
**prefer the `initializer_list` one** (the `vector<int>{3}` vs `vector<int>(3)`
trap from Chapter 1).

---

## 7.5 `std::chrono` — Type-Safe Time

### What It Is

A type-safe library for **durations, time points, and clocks** — no more raw
`int` seconds/milliseconds confusion.

```cpp
#include <chrono>
using namespace std::chrono;

auto start = steady_clock::now();
do_work();
auto end = steady_clock::now();

auto ms = duration_cast<milliseconds>(end - start).count();
std::cout << "took " << ms << " ms\n";

std::this_thread::sleep_for(seconds(2));   // type-safe sleep
```

### Why It Matters

- **Type safety:** mixing `seconds` and `milliseconds` won't silently misbehave —
  conversions are explicit and correct.
- **Clarity:** `seconds(2)` instead of an ambiguous `2`.

> The literal suffixes `2s`, `100ms` are a **C++14** addition (`chrono_literals`).

---

## 7.6 `<random>` — Proper Random Numbers

### What It Is

A real random-number framework separating **engines** (generators) from
**distributions** — replacing the flawed C `rand()`.

```cpp
#include <random>

std::random_device rd;                       // non-deterministic seed
std::mt19937 gen(rd());                       // Mersenne Twister engine
std::uniform_int_distribution<int> dist(1, 6);// fair die

int roll = dist(gen);                         // a uniform integer in [1,6]
```

### Why Over `rand()`?

`rand()` has poor statistical quality, a small/implementation-defined range, and
biased modulo reduction. `<random>` gives high-quality engines and **correct,
unbiased distributions**.

---

## 7.7 `<regex>` — Regular Expressions

```cpp
#include <regex>

std::string text = "Contact: john@example.com";
std::regex email(R"(\w+@\w+\.\w+)");          // raw string keeps regex readable

std::smatch match;
if (std::regex_search(text, match, email))
    std::cout << "found: " << match.str() << '\n';

std::string clean = std::regex_replace(text, email, "[redacted]");
```

> Note: some standard-library `<regex>` implementations are notably slow; for
> hot paths, benchmark or consider a dedicated regex engine.

---

## 7.8 `std::function` & `std::bind`

### `std::function` — Type-Erased Callable

Stores **any** callable (function, lambda, functor, member function) with a
matching signature behind one uniform type.

```cpp
#include <functional>

std::function<int(int, int)> op = [](int a, int b){ return a + b; };
op = std::plus<int>();                          // reassign to a different callable
std::vector<std::function<void()>> handlers;    // heterogeneous callable store
```

### `std::bind` — Partial Application

Binds arguments to a callable, producing a new callable.

```cpp
auto add = [](int a, int b){ return a + b; };
auto add5 = std::bind(add, 5, std::placeholders::_1);  // fix first arg = 5
add5(10);                                              // 15
```

> In modern code, **a lambda is usually clearer and faster than `std::bind`**.
> Prefer `[](int x){ return add(5, x); }` over `std::bind`.

---

## 7.9 Other Library Additions (Quick Hits)

- **`std::to_string` / `std::stoi`, `std::stod`, ...** — easy number↔string.
- **`std::move_iterator`**, `std::begin`/`std::end` free functions.
- **New algorithms:** `std::all_of`, `std::any_of`, `std::none_of`,
  `std::copy_n`, `std::iota`, `std::minmax`.
- **`std::forward_list`** — singly-linked list (lower overhead than `list`).
- **`std::ref` / `std::cref`** — reference wrappers (e.g., to pass by reference
  through `std::thread`/`std::bind`).
- **`emplace` / `emplace_back`** — construct elements in place (forwarding args),
  avoiding a temporary + move/copy.

```cpp
std::vector<std::pair<int,std::string>> v;
v.push_back(std::make_pair(1, "a"));    // build temp, then move
v.emplace_back(1, "a");                 // construct in place — no temporary
```

---

## C++03 vs C++11 — Library Comparison

| Feature | C++03 | C++11 |
|---------|:-----:|:-----:|
| Hash containers (`unordered_*`) | ❌ (Boost/TR1) | ✅ |
| `std::array` | ❌ | ✅ |
| `std::tuple` | ❌ (Boost/TR1) | ✅ |
| `std::initializer_list` | ❌ | ✅ |
| `std::chrono` | ❌ | ✅ |
| `<random>` | ❌ (`rand()`) | ✅ |
| `<regex>` | ❌ (Boost) | ✅ |
| `std::function` / `std::bind` | ❌ (Boost/TR1) | ✅ |
| `emplace` operations | ❌ | ✅ |

---

## Chapter 7 Cheat Sheet

```cpp
std::unordered_map<std::string,int> m;       // O(1) average lookup
std::array<int,4> a{1,2,3,4};                // fixed-size, knows its size
std::tuple<int,std::string> t{1,"x"};         // heterogeneous; std::get<0>(t)
std::tie(i, s) = t;                           // unpack (structured bindings: C++17)

auto start = std::chrono::steady_clock::now();// type-safe timing

std::mt19937 gen{std::random_device{}()};     // good RNG
std::uniform_int_distribution<int> d(1,6);    // unbiased distribution

std::function<int(int)> f = [](int x){return x;};
v.emplace_back(args...);                       // construct in place
```

**Remember:**
- `unordered_map` = O(1) average but unordered; `map` = sorted, O(log n).
- `std::array` is a safe, STL-friendly fixed array with zero overhead.
- Use `<random>` engines+distributions, never `rand()`.
- Prefer **lambdas over `std::bind`**, and **`emplace_back` over `push_back`** for
  in-place construction.

---

## Common Mistakes (Library)

1. Relying on `unordered_map` iteration order (there is none).
2. Using `rand()` / modulo for random numbers (biased, low quality).
3. The `vector<int>{n}` vs `vector<int>(n)` brace trap.
4. Custom keys in `unordered_map` without a `std::hash` specialization.
5. Reaching for `std::bind` where a lambda is clearer.

## When NOT to Use

- **`unordered_map`** when you need ordered traversal or range queries — use `map`.
- **`<regex>`** in performance-critical loops without benchmarking.
- **`std::function`** in hot paths where a template/`auto` lambda avoids the
  type-erasure overhead.

---

*Next: [Chapter 8 — Interview Preparation Bank](08_interview_questions.md)*
