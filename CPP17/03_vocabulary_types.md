# Chapter 3 — Vocabulary Types (C++17 STL Additions)

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

"Vocabulary types" are standard types that become a **shared language** for API
design. C++17 added four that changed how modern C++ interfaces are written:
`optional`, `variant`, `any`, and `string_view`.

---

## 3.1 `std::optional<T>`

### What It Is

A type that **either holds a `T` or holds nothing**. It models "a value that may
be absent" without sentinels, magic numbers, or out-parameters.

```cpp
#include <optional>

std::optional<int> parse_int(const std::string& s) {
    try { return std::stoi(s); }      // has a value
    catch (...) { return std::nullopt; } // empty
}

if (auto n = parse_int("42"); n)      // contextually bool
    std::cout << *n;                   // dereference to access
```

### Why It Was Introduced

Functions that "might not return a value" had only **bad options**:

### The Problem Before (C++14)

```cpp
// Option A: sentinel value — ambiguous (is -1 valid data or "not found"?)
int find_index(...);          // returns -1 on failure

// Option B: out-parameter + bool — clunky, two-step
bool find(..., int& out);

// Option C: pointer that may be null — ownership/lifetime unclear
const Value* lookup(...);

// Option D: pair<bool, T> — must always construct a T even when absent
std::pair<bool, int> tryParse(...);
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Clarity** | The signature *documents* that the result may be absent |
| **Safety** | No sentinel confusion; empty state is explicit |
| **Efficiency** | No heap allocation — value stored inline |

### API

```cpp
std::optional<int> o;
o.has_value();        // or: if (o)
o.value();            // checked access — throws std::bad_optional_access if empty
*o;  o->member;       // unchecked access — UB if empty
o.value_or(-1);       // value, or a fallback if empty
o.reset();            // make empty
o = 5;                // assign a value
o.emplace(args...);   // construct in place
```

### Real-World Use Cases

- Lookup/parse functions ("found or not").
- Optional configuration fields.
- Lazy/deferred values.
- Replacing nullable pointers where ownership isn't intended.

### Pitfalls

1. **`*opt` / `opt->x` on an empty optional is undefined behavior.** Use
   `.value()` (throws) or check first.
2. **`std::optional<T&>` is not allowed.** For an optional reference, use a
   pointer (`T*`) instead.
3. `optional<bool>` has **three** states (empty/true/false) — be careful in
   conditionals; `if (o)` tests *presence*, not the contained bool.

---

## 3.2 `std::variant<...>`

### What It Is

A **type-safe union** — it holds exactly one value from a fixed set of
alternative types, and it knows which one.

```cpp
#include <variant>

std::variant<int, double, std::string> v;
v = 42;                  // holds int
v = 3.14;                // now holds double
v = "hello";             // now holds std::string

std::cout << v.index();  // which alternative (0,1,2)
```

### Why It Was Introduced

C-style `union` is **unsafe**: it doesn't track the active member, doesn't call
constructors/destructors, and reading the wrong member is UB.

### The Problem Before (C++14)

```cpp
// Raw union: you must track the type yourself and manage lifetimes by hand
struct Value {
    enum Tag { INT, STR } tag;
    union {
        int i;
        std::string s;   // non-trivial member → manual ctor/dtor needed!
    };
    // ...lots of error-prone boilerplate to construct/destroy 's'...
};
// Or depend on boost::variant (third-party).
```

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Safety** | Tracks the active type; manages ctors/dtors automatically |
| **Correctness** | Wrong-type access throws instead of silent UB |
| **Expressiveness** | Models sum types / state machines cleanly |

### Accessing the Value

```cpp
std::variant<int, std::string> v = "hi";

// 1. std::get<T> — throws std::bad_variant_access on wrong type
auto s = std::get<std::string>(v);

// 2. std::get_if<T> — returns pointer or nullptr (no throw)
if (auto p = std::get_if<std::string>(&v)) use(*p);

// 3. std::holds_alternative<T>
if (std::holds_alternative<int>(v)) {}

// 4. std::visit — apply a callable to whichever type is active (idiomatic)
std::visit([](const auto& x){ std::cout << x; }, v);
```

### The Overload / Visitor Pattern

```cpp
// Helper to build an overload set from lambdas (very common C++17 idiom)
template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...) -> overload<Ts...>;   // CTAD guide

std::visit(overload{
    [](int i)                { std::cout << "int: "    << i; },
    [](double d)             { std::cout << "double: " << d; },
    [](const std::string& s) { std::cout << "string: " << s; }
}, v);
```

### Real-World Use Cases

- **State machines** (each state a distinct type).
- **Parsers / ASTs** (a node is one of several kinds).
- **Message/event types** in protocols.
- Return types that are "one of several results."

### Pitfalls

1. **`std::get<T>` on the wrong alternative throws `bad_variant_access`.**
2. A `variant` is **as large as its largest alternative** (plus a tag).
3. **`std::visit` requires the visitor to handle *every* alternative** (the
   `overload` trick helps).
4. Default-constructs the **first** alternative — order matters.

---

## 3.3 `std::any`

### What It Is

A type-erased container that can hold **a value of any copy-constructible type**,
with type-safe retrieval.

```cpp
#include <any>

std::any a = 42;              // holds int
a = std::string("hello");    // now holds string
a = 3.14;                    // now holds double

auto d = std::any_cast<double>(a);          // throws bad_any_cast if wrong
if (auto p = std::any_cast<int>(&a)) {}     // pointer form: nullptr if wrong
a.has_value();               // is it non-empty?
a.type();                    // std::type_info of the stored type
```

### Why It Was Introduced

Sometimes you genuinely don't know the type at compile time (plugin systems,
dynamic property bags). The pre-C++17 answer was `void*` (unsafe) or
`boost::any`.

### `variant` vs `any` — Choosing

| | `std::variant` | `std::any` |
|--|----------------|------------|
| Type set | **Fixed, known** at compile time | **Open / unknown** |
| Type safety | Compile-time checked alternatives | Runtime-checked cast |
| Storage | Inline (size of largest) | Often heap-allocated |
| Performance | Faster, no allocation | Slower, type erasure |
| Use when | You know all possibilities | Truly arbitrary types |

### Pitfalls

1. **`std::any_cast<T>` with the wrong `T` throws `std::bad_any_cast`.**
2. **Likely heap allocation** → avoid in hot paths.
3. **Overuse is a design smell** — prefer `variant` or templates when the type
   set is finite.

---

## 3.4 `std::string_view`

### What It Is

A **non-owning, read-only view** into a contiguous character sequence — a
`{pointer, length}` pair. It does **not** copy or own the characters.

```cpp
#include <string_view>

void log(std::string_view msg) {            // accepts string, const char*, literal
    std::cout << msg << '\n';               // no allocation, no copy
}

std::string s = "hello";
log(s);            // no copy of s
log("literal");    // no temporary std::string
log(s.substr(0));  // careful (see pitfalls)
```

### Why It Was Introduced — A Major Performance Win

A function taking `const std::string&` forces a **temporary allocation** when
called with a string literal or `const char*`:

### The Problem Before (C++14)

```cpp
void process(const std::string& s);   // C++14 signature

process("literal");   // constructs a TEMPORARY std::string (heap alloc!) per call
```

Worse, overloading on `const char*` and `const std::string&` to avoid this is
ugly and combinatorial.

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Performance** | Zero allocation/copy for read-only string params |
| **Generality** | One parameter type accepts `string`, `const char*`, literals, buffers |
| **Substrings** | `sv.substr()` is O(1) — just adjusts pointer/length, no copy |

### Why Substrings Are Cheap

```cpp
std::string_view sv = "hello world";
auto word = sv.substr(0, 5);   // "hello" — NO new allocation; just a view
```

`std::string::substr` copies; `std::string_view::substr` doesn't.

### Real-World Use Cases

- **Read-only string parameters** (logging, parsing, tokenizing).
- **High-performance parsers** that slice a big buffer into many views.
- **APIs that should accept any string-like source** without overloads.

### Pitfalls — The #1 C++17 Footgun: Dangling Views

A `string_view` **does not own** its data. If the underlying buffer dies, the
view dangles:

```cpp
std::string_view get_bad() {
    std::string local = "temp";
    return local;        // DANGLING: local is destroyed on return!
}                        // the returned view points to freed memory

// Subtler: viewing a temporary
std::string_view sv = std::string("temp") + "x"; // temporary dies at ';' → dangling
```

**Rules of thumb:**
1. Use `string_view` for **function parameters** (caller owns the data during the call).
2. **Never store** a `string_view` that outlives the buffer it points to.
3. **Don't return** a `string_view` to a local string.
4. A `string_view` is **not guaranteed null-terminated** — don't pass `.data()`
   to C APIs expecting `\0`-terminated strings.

---

## C++14 vs C++17 — Vocabulary Types Comparison

| Need | C++14 Approach | C++17 |
|------|----------------|-------|
| Maybe-a-value | sentinel / `pair<bool,T>` / `T*` | `std::optional<T>` |
| Type-safe union | raw `union` + tag / `boost::variant` | `std::variant<...>` |
| Hold any type | `void*` / `boost::any` | `std::any` |
| Read-only string param | `const std::string&` (allocates) | `std::string_view` |

---

## Chapter 3 Cheat Sheet

```cpp
// optional
std::optional<int> o = find();
if (o) use(*o);                 // or o.value() (throws if empty)
int x = o.value_or(0);

// variant
std::variant<int, std::string> v = "hi";
std::visit([](auto&& a){ /*...*/ }, v);
if (auto p = std::get_if<int>(&v)) {}

// any
std::any a = 42;
if (auto p = std::any_cast<int>(&a)) {}

// string_view
void f(std::string_view sv);    // accept any string source, no copy
auto sub = sv.substr(0, 4);     // O(1) slice
```

**Remember:**
- `optional` = maybe-a-value; never `*` it when empty.
- `variant` = closed set of types; `any` = open/arbitrary types.
- `string_view` = fast read-only param, but **watch for dangling**.

---

## Common Mistakes (Vocabulary Types)

1. Dereferencing an empty `optional` (UB) — use `.value()` or check.
2. `std::get<T>` on a `variant`'s inactive alternative (throws).
3. Storing a `string_view` into a temporary/local string (dangling).
4. Reaching for `std::any` when a `variant` (finite types) would be safer/faster.
5. Passing `string_view::data()` to a C API expecting null termination.

## When NOT to Use

- **`std::any`** when the type set is known — use `variant` or a template.
- **`string_view`** as a member that may outlive its source — own a `std::string`.
- **`optional<bool>`** in confusing conditionals — consider an explicit enum.

---

*Next: [Chapter 4 — Filesystem & Parallel Algorithms](04_filesystem_and_parallelism.md)*
