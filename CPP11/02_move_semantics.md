# Chapter 2 — Move Semantics & Rvalue References (C++11)

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

**This is the single most important C++11 feature.** It changed how C++ handles
value types, eliminating unnecessary copies and making "return by value" fast.

---

## 2.1 Lvalues vs Rvalues — The Foundation

### What They Are

- **lvalue** — has a name / identity / persistent address. You can take its
  address. (e.g., a variable `x`).
- **rvalue** — a temporary without a persistent identity (e.g., the result of
  `a + b`, a literal `42`, a function returning by value).

```cpp
int x = 5;        // x is an lvalue; 5 is an rvalue
int y = x + 1;    // (x + 1) is an rvalue (temporary)
int* p = &x;      // OK — can take address of lvalue
// int* q = &(x+1); // ERROR — cannot take address of rvalue
```

### Why It Matters

An **rvalue is about to be destroyed anyway**, so we can safely *steal* its
internal resources instead of copying them. That insight is the whole basis of
move semantics.

---

## 2.2 The Problem Move Semantics Solves

### C++03: Everything Was a Copy

```cpp
// C++03: returning/assigning a big object deep-copies its heap data
std::vector<int> makeData() {
    std::vector<int> v(1'000'000);  // large
    return v;                       // COPY of a million ints (expensive!)
}

std::vector<int> data = makeData(); // potentially another copy

std::string a = "long string ...";
std::string b = a + a;              // temporary built, then COPIED into b
```

Every temporary was duplicated and then thrown away — a huge waste for
heap-owning types (`string`, `vector`, etc.).

### The Limitation

C++03 had **no way to distinguish** "copy from a value I still need" versus
"copy from a temporary that's about to die." Both went through the copy
constructor.

---

## 2.3 Rvalue References (`&&`)

### What It Is

A new reference type, `T&&`, that **binds to rvalues (temporaries)**. It lets a
function know its argument is expendable.

```cpp
void f(const std::string& s); // binds to lvalues AND rvalues (copy world)
void f(std::string&& s);      // binds ONLY to rvalues (move world)

std::string a = "hi";
f(a);              // calls f(const std::string&)  — a is an lvalue
f(std::string{});  // calls f(std::string&&)        — temporary is an rvalue
```

---

## 2.4 Move Constructor & Move Assignment

### What They Are

Special members that **transfer (steal) resources** from a source object instead
of copying them, leaving the source in a valid-but-empty state.

```cpp
class Buffer {
    int* data_;
    std::size_t size_;
public:
    // Move constructor — steal the pointer, null out the source
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;   // source no longer owns it
        other.size_ = 0;
    }

    // Move assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;          // release current resource
            data_ = other.data_;     // steal
            size_ = other.size_;
            other.data_ = nullptr;   // empty the source
            other.size_ = 0;
        }
        return *this;
    }
    ~Buffer() { delete[] data_; }
};
```

### Before vs After (Performance)

```cpp
// Copy: allocate + copy a million ints   → O(n), heap allocation
// Move: copy a pointer + a size          → O(1), no allocation
```

For the earlier `makeData()` example, C++11 **moves** the returned vector — the
million-int allocation is transferred, not duplicated.

### How It Improves Things

| Aspect | Improvement |
|--------|-------------|
| **Performance** | Heap-owning returns/inserts become O(1) pointer swaps |
| **Idioms** | "Return by value" is now efficient and recommended |
| **Containers** | `vector` reallocation/`push_back` move elements, not copy |

### Why `noexcept` Matters Here

Mark move operations `noexcept`. STL containers (e.g., `std::vector`) will only
**move** elements during reallocation if the move constructor is `noexcept`;
otherwise they **copy** (to preserve the strong exception guarantee). Forgetting
`noexcept` silently kills your performance win.

---

## 2.5 `std::move` — The Most Misunderstood Function

### What It *Actually* Does

`std::move` **does not move anything.** It is just a **cast** to an rvalue
reference, signaling "you may steal from this." The move happens later, inside a
move constructor/assignment.

```cpp
template <class T>
constexpr std::remove_reference_t<T>&& move(T&& x) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(x);  // just a cast!
}
```

### Usage

```cpp
std::string a = "hello";
std::string b = std::move(a);   // b steals a's buffer; a is now empty-ish
// 'a' is valid but unspecified — don't rely on its value
```

### Pitfalls

1. **Using a moved-from object's value.** It's valid but unspecified (for
   `std::string` typically empty, but don't depend on it).
2. **`std::move` on a `const` object copies.** You can't steal from a `const`
   source, so it silently falls back to the copy constructor.
   ```cpp
   const std::string c = "x";
   std::string d = std::move(c);  // still a COPY (c is const)
   ```
3. **`return std::move(local);` is usually wrong** — it can **disable RVO/copy
   elision**. Just `return local;`.

---

## 2.6 Perfect Forwarding, `std::forward` & Reference Collapsing

### The Problem

A generic wrapper should pass its arguments to an inner function **preserving
their value category** — lvalues stay lvalues, rvalues stay rvalues — so the
inner function can copy or move as appropriate.

```cpp
template <class T>
void wrapper(T&& arg) {           // forwarding (universal) reference
    inner(std::forward<T>(arg));  // preserves lvalue/rvalue-ness
}
```

### Forwarding (Universal) References

`T&&` where `T` is a **deduced template parameter** is special — it's a
**forwarding reference** that binds to *both* lvalues and rvalues. (A `T&&` with
a concrete type, like `std::string&&`, is a plain rvalue reference.)

### Reference Collapsing Rules

When references combine during deduction, they "collapse":

| Combination | Result |
|-------------|:------:|
| `T& &`  | `T&`  |
| `T& &&` | `T&`  |
| `T&& &` | `T&`  |
| `T&& &&`| `T&&` |

Rule of thumb: **"any lvalue reference wins; only `&& &&` stays `&&`."**

### `std::forward`

Conditionally casts back to an rvalue **only if** the original argument was an
rvalue — the partner of `std::move` for generic code.

```cpp
// std::move:    UNCONDITIONALLY cast to rvalue (use on a value you own)
// std::forward: CONDITIONALLY cast to rvalue (use on a forwarding reference)
```

### Before vs After

```cpp
// ─────────── C++03 — overload explosion to handle const/non-const, l/r value
template <class T> void wrap(T& x)       { inner(x); }
template <class T> void wrap(const T& x) { inner(x); }
// ...and you still can't forward rvalue-ness for moving...

// ─────────── C++11 — one perfect-forwarding template
template <class T> void wrap(T&& x) { inner(std::forward<T>(x)); }
```

### Real-World Use Cases

- **Factory functions** (`make_unique`/`make_shared` forward constructor args).
- **`emplace_back`** — forwards arguments to construct elements in place.
- **Wrappers/decorators** that must relay arguments untouched.

---

## 2.7 The Rule of Five

Once you define one of these, you usually must consider **all five**:

```cpp
class Resource {
public:
    ~Resource();                               // 1. destructor
    Resource(const Resource&);                 // 2. copy constructor
    Resource& operator=(const Resource&);      // 3. copy assignment
    Resource(Resource&&) noexcept;             // 4. move constructor
    Resource& operator=(Resource&&) noexcept;  // 5. move assignment
};
```

> **Rule of Zero (preferred):** design classes that own resources via RAII
> members (`std::vector`, `std::unique_ptr`, ...) so you need to write **none**
> of the five — the compiler-generated ones are correct.

---

## C++03 vs C++11 — Move Semantics Comparison

| Capability | C++03 | C++11 |
|-----------|:-----:|:-----:|
| Rvalue references `&&` | ❌ | ✅ |
| Move constructor/assignment | ❌ | ✅ |
| `std::move` | ❌ | ✅ |
| Perfect forwarding / `std::forward` | ❌ | ✅ |
| Efficient return-by-value | ❌ (deep copy) | ✅ (move) |
| `emplace_back` | ❌ | ✅ |

---

## Chapter 2 Cheat Sheet

```cpp
// Move constructor (steal + null out source), mark noexcept!
Buf(Buf&& o) noexcept : p(o.p), n(o.n) { o.p = nullptr; o.n = 0; }

std::string b = std::move(a);          // cast-to-rvalue → enables stealing

template <class T>                      // perfect forwarding
void wrap(T&& x) { inner(std::forward<T>(x)); }

// std::move    = unconditional rvalue cast (own value)
// std::forward = conditional rvalue cast (forwarding reference)
// return local;  (NOT return std::move(local);) — keep copy elision
```

**Remember:**
- `std::move` **casts**, it doesn't move; the move happens in a move ctor/assign.
- Mark move operations **`noexcept`** or containers fall back to copying.
- `T&&` on a **deduced** template param = forwarding reference (binds to both).
- Prefer the **Rule of Zero**: let RAII members manage resources.

---

## Common Mistakes (Move Semantics)

1. Reading a moved-from object's value (valid but unspecified).
2. `std::move` on a `const` object (silently copies).
3. Forgetting `noexcept` on move ops → containers copy instead of move.
4. `return std::move(local);` → defeats copy elision (RVO).
5. Confusing a plain rvalue ref (`std::string&&`) with a forwarding ref (`T&&`).

## When NOT to Use

- **Don't `std::move`** a local you still need afterward.
- **Don't add `&&` overloads** for cheap-to-copy types (no benefit, more code).
- **Don't hand-write the Rule of Five** if RAII members make Rule of Zero possible.

---

*Next: [Chapter 3 — Lambda Expressions](03_lambdas.md)*
