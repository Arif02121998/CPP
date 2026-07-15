# Chapter 4 — Library & STL Additions + Smaller Features

> *Level: Intermediate*
> Back to [Table of Contents](README.md)

C++14's library changes are small but **remove daily friction**. This chapter
collects every notable library and minor language addition.

---

## 4.1 `std::make_unique`

### What It Is

A factory function that constructs an object and returns a `std::unique_ptr`
owning it — the missing sibling of C++11's `std::make_shared`.

```cpp
auto p = std::make_unique<Widget>(arg1, arg2); // unique_ptr<Widget>
```

### Why It Was Introduced

C++11 shipped `make_shared` but **forgot `make_unique`** — an oversight. So
people wrote `std::unique_ptr<T>(new T(...))` by hand, which is:
- **Verbose** (type written twice),
- **Exception-unsafe** in certain call expressions.

### The Exception-Safety Problem — Before

```cpp
// C++11 — potential leak!
process(std::unique_ptr<A>(new A), std::unique_ptr<B>(new B));
```

The compiler may evaluate in this order:
1. `new A`
2. `new B`   ← if this **throws**, the raw `A*` from step 1 leaks (no owner yet)
3. construct `unique_ptr<A>`

`make_unique` makes each allocation+ownership **atomic**, eliminating the leak:

```cpp
process(std::make_unique<A>(), std::make_unique<B>()); // exception-safe
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Safety** | No raw `new`; exception-safe argument evaluation |
| **Readability** | Type named once, no `new` keyword |
| **Consistency** | Matches `make_shared` |

### Arrays

```cpp
auto arr = std::make_unique<int[]>(10); // unique_ptr<int[]>, 10 ints
arr[3] = 42;
```

### Pitfalls

- `make_unique` **value-initializes** — `make_unique<int>()` gives `0`,
  whereas `unique_ptr<int>(new int)` leaves it **uninitialized**.
- Unlike `make_shared`, `make_unique` does **not** combine control block +
  object allocation (unique_ptr has no control block).
- Can't use `make_unique` with a **custom deleter** — fall back to the explicit
  constructor there.

---

## 4.2 Transparent Comparators & Heterogeneous Lookup

### What It Is

C++14 lets associative containers (`std::map`, `std::set`, ...) be searched with
a **key of a different but comparable type**, *without constructing a temporary
key*, by using a **transparent comparator** (`std::less<>` with no type).

```cpp
std::set<std::string, std::less<>> s = {"alpha", "beta"};

// C++14: look up with a const char* / string_view — NO temporary std::string
auto it = s.find("alpha");   // no std::string("alpha") constructed
```

### Why It Was Introduced — The Problem Before

```cpp
// C++11: std::set<std::string> with default std::less<std::string>
std::set<std::string> s = {"alpha", "beta"};
s.find("alpha"); // constructs a TEMPORARY std::string just to compare — wasteful
```

Every lookup with a string literal allocated a temporary `std::string`.

### How It Improves Things

- **Performance:** avoids temporary allocations on each lookup.
- Enabled by opting in with `std::less<>` (the "diamond"/empty `<>` form),
  which exposes an `is_transparent` member type.

### Pitfall

- You must **opt in** with `std::less<>` (or another transparent comparator).
  The default `std::less<Key>` is **not** transparent, so it still builds
  temporaries.

---

## 4.3 `std::integer_sequence` / `std::index_sequence`

### What It Is

A compile-time sequence of integers used to **unpack tuples / parameter packs**
by index — a foundational metaprogramming helper.

```cpp
template <typename Tuple, std::size_t... I>
void print_impl(const Tuple& t, std::index_sequence<I...>) {
    ((std::cout << std::get<I>(t) << ' '), ...); // (fold is C++17 flavor)
}

template <typename... Ts>
void print(const std::tuple<Ts...>& t) {
    print_impl(t, std::index_sequence_for<Ts...>{});
}
```

### Why It Was Introduced

Before C++14, every project **hand-rolled** an "index sequence" trick to expand
tuples. C++14 standardized it as `std::integer_sequence`,
`std::index_sequence`, `std::make_index_sequence`, and `std::index_sequence_for`.

### Real-World Use Cases

- Calling a function with the elements of a `std::tuple` (pre-`std::apply`).
- Generating compile-time tables.
- Implementing `std::apply`-like and `std::make_from_tuple`-like utilities.

---

## 4.4 `std::exchange`

### What It Is

Sets an object to a new value and **returns its old value** — atomically in one
expression.

```cpp
template <class T, class U = T>
T exchange(T& obj, U&& new_val);   // returns old obj, assigns new_val
```

### Why It's Useful

Perfect for **move assignment operators** and **resetting state**:

```cpp
// Clean move constructor using std::exchange
Buffer(Buffer&& other) noexcept
    : data_(std::exchange(other.data_, nullptr)),  // take ptr, null the source
      size_(std::exchange(other.size_, 0)) {}
```

Replaces the classic three-line "copy, reset, return" idiom with one call.

---

## 4.5 `std::quoted`

### What It Is

An I/O manipulator that **quotes and escapes** strings on output and **unquotes**
on input — round-trips strings containing spaces/quotes safely.

```cpp
std::string in = R"(she said "hi" there)";
std::stringstream ss;
ss << std::quoted(in);          // writes: "she said \"hi\" there"

std::string out;
ss >> std::quoted(out);         // recovers the original exactly
assert(in == out);
```

### Why It Matters

Before this, serializing strings with embedded spaces/quotes through streams was
error-prone. `std::quoted` makes simple text serialization reliable.

---

## 4.6 Smaller Language Features

### Binary Literals

```cpp
int flags = 0b1011'0010;   // base-2 literal (was octal/hex/dec only before)
```

### Digit Separators (`'`)

```cpp
long population = 7'900'000'000;     // readable large numbers
double pi       = 3.141'592'653;
int hex         = 0xFF'EC'DE'5E;
```

Improves readability; the `'` is ignored by the compiler.

### `[[deprecated]]` Attribute

```cpp
[[deprecated("use new_api() instead")]]
void old_api();

old_api(); // compiler warning with your message
```

Standard, portable way to mark APIs for removal (replaces compiler-specific
`__attribute__((deprecated))` / `__declspec(deprecated)`).

### `get<T>()` — Access `std::tuple` by Type

```cpp
std::tuple<int, std::string, double> t{1, "x", 2.5};
auto s = std::get<std::string>(t);   // by TYPE (C++14), not just index
```

Works only when the type is **unique** within the tuple.

### Standard User-Defined Literals

C++14 added official UDL suffixes:

```cpp
using namespace std::chrono_literals;
auto delay = 250ms;            // std::chrono::milliseconds
auto hour  = 1h;

using namespace std::string_literals;
auto str = "hello"s;           // std::string (not const char*)

using namespace std::complex_literals;
auto z = 2.0 + 3.0i;           // std::complex<double>
```

---

## C++11 vs C++14 — Library Comparison

| Feature | C++11 | C++14 |
|---------|:-----:|:-----:|
| `make_shared` | ✅ | ✅ |
| `make_unique` | ❌ | ✅ |
| Transparent comparators / heterogeneous lookup | ❌ | ✅ |
| `integer_sequence` / `index_sequence` | ❌ | ✅ |
| `std::exchange` | ❌ | ✅ |
| `std::quoted` | ❌ | ✅ |
| `get<T>()` (tuple by type) | ❌ | ✅ |
| Binary literals / digit separators | ❌ | ✅ |
| `[[deprecated]]` | ❌ | ✅ |
| Standard UDLs (`s`, `ms`, `i`) | ❌ | ✅ |

---

## Chapter 4 Cheat Sheet

```cpp
auto p   = std::make_unique<T>(args);     // safe ownership
auto arr = std::make_unique<T[]>(n);      // array form

std::set<std::string, std::less<>> s;     // transparent: no temp on find()

auto old = std::exchange(x, newVal);      // set new, return old

std::cout << std::quoted(text);           // quote+escape strings

int b = 0b1010'1100;                      // binary + separators
auto t = 100ms;                           // chrono UDL
auto v = std::get<std::string>(tuple);    // tuple by type
```

**Remember:**
- Prefer `make_unique` over raw `new` — safety + brevity.
- Opt into `std::less<>` for allocation-free associative lookups.
- `std::exchange` makes move operations one-liners.

---

## Common Mistakes (Library)

1. Using `make_unique` with a **custom deleter** (not supported — use the ctor).
2. Forgetting `std::less<>` and paying for temporary keys on every lookup.
3. Calling `std::get<T>()` on a tuple with **duplicate** `T` (ill-formed).
4. Expecting `make_unique<int>()` to be uninitialized (it value-initializes).

## When NOT to Use

- **`std::quoted`** for real structured data — use a proper serialization format
  (JSON, protobuf) instead of stream tricks.
- **Digit separators** mid-token in ways that hurt clarity — keep groupings
  conventional (thousands, byte boundaries).

---

*Next: [Chapter 5 — Interview Preparation Bank](05_interview_questions.md)*
