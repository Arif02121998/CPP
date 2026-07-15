# Chapter 5 — Interview Preparation Bank (C++17)

> Back to [Table of Contents](README.md)
> Categories: **Beginner → Intermediate → Advanced**, plus **code-based** and
> **tricky edge-case** questions. Concise, complete answers.

---

## 5.1 Beginner Questions (Conceptual)

**Q1. What kind of release is C++17?**
A medium-sized, pragmatic release. Highlights: vocabulary types
(`optional`/`variant`/`any`/`string_view`), `if constexpr`, structured bindings,
CTAD, fold expressions, `std::filesystem`, parallel algorithms.

**Q2. What is a structured binding?**
Syntax to decompose a pair/tuple/array/struct into named variables:
`auto [key, val] = *map.begin();`. Replaces `.first`/`.second`/`std::get`.

**Q3. What is `std::optional`?**
A type that holds a value **or nothing**. Models "maybe a value" without
sentinels or out-parameters. Check with `if (opt)`, access with `*opt`/`.value()`.

**Q4. What is `std::string_view`?**
A non-owning, read-only `{pointer, length}` view of characters. Avoids copying
when passing read-only strings. **Doesn't own** the data → watch for dangling.

**Q5. What does `if constexpr` do?**
Branches at **compile time** and discards the untaken branch (it isn't
instantiated), enabling type-dependent code in templates.

**Q6. What is CTAD?**
Class Template Argument Deduction — the compiler deduces a class template's type
args from constructor arguments: `std::pair p{1, 2.5};` → `pair<int,double>`.

**Q7. What is a fold expression?**
A concise way to apply a binary operator over a parameter pack:
`(args + ...)` sums all arguments without recursion.

---

## 5.2 Intermediate Questions

**Q8. `std::variant` vs raw `union`?**
`variant` is type-safe: it tracks the active type, calls ctors/dtors, and throws
on wrong-type access. Raw `union` does none of this — wrong-member access is UB.

**Q9. `std::variant` vs `std::any`?**
`variant` holds one of a **fixed, known** set of types (compile-time checked,
stored inline). `any` holds **any** type (runtime-checked cast, often heap).
Use `variant` when the type set is finite; `any` only when truly open.

**Q10. Why is `string_view` faster than `const std::string&`?**
Passing a literal or `const char*` to a `const std::string&` parameter
constructs a **temporary `std::string` (heap allocation)**. `string_view` just
references the existing characters — no allocation, no copy.

**Q11. How do you access a `std::variant`'s value?**
`std::get<T>` (throws on wrong type), `std::get_if<T>` (returns pointer/nullptr),
`std::holds_alternative<T>`, or `std::visit` with a callable (idiomatic).

**Q12. What problem do `if`/`switch` initializers solve?**
They scope a variable to the `if`/`switch` (and its `else`) instead of leaking it
into the enclosing scope or needing extra braces:
`if (auto it = m.find(k); it != m.end())`.

**Q13. What are inline variables for?**
They let a `static`/global variable be **defined in a header** with one shared
instance — no separate `.cpp` definition. Crucial for header-only libraries.

**Q14. What is guaranteed copy elision?**
C++17 mandates that returning a prvalue constructs the object directly in its
destination — no copy/move. This lets you **return non-movable types by value**.

**Q15. How do parallel algorithms work in C++17?**
Pass an execution policy (`std::execution::seq/par/par_unseq`) as the first
argument to STL algorithms to request parallel/vectorized execution.

---

## 5.3 Advanced & Tricky Edge-Case Questions

**Q16. (Footgun) What's wrong here?**
```cpp
std::string_view sv = std::string("hello") + " world";
std::cout << sv;
```
The concatenation produces a **temporary `std::string`** that is destroyed at the
end of the full expression. `sv` then points to freed memory → **dangling view /
UB**.

**Q17. What does this print and why?**
```cpp
std::vector v{5};
std::cout << v.size();
```
`1`. With CTAD + braced init, `{5}` is an **initializer list with one element**,
so `v` is `vector<int>{5}` (one element), **not** five default ints. Use
parentheses `std::vector<int> v(5)` for five elements.

**Q18. Does `auto [a, b] = p;` reference `p`?**
No — it **copies** `p` into a hidden object and binds names to the copy. Use
`auto& [a, b] = p;` to bind references to `p`'s members.

**Q19. Why can a regular `if` fail where `if constexpr` succeeds?**
A runtime `if` requires **both** branches to compile for every instantiated `T`.
`if constexpr` discards the false branch, so it may contain code that's
ill-formed for that `T` (e.g., `*t` when `T` isn't a pointer).

**Q20. What happens on `std::get<int>(v)` when the variant holds a string?**
It throws `std::bad_variant_access`. Use `std::get_if<int>(&v)` (returns
`nullptr`) for a non-throwing check.

**Q21. Is `std::optional<int&>` valid?**
No — `optional` of a reference is not allowed. For an optional reference, use a
pointer (`int*`), where `nullptr` means "absent".

**Q22. With a fold expression, why does `(args + ...)` fail for zero arguments?**
Only `&&`, `||`, and `,` have a defined identity for an **empty pack**. For other
operators, use the binary form with an init value: `(0 + ... + args)`.

**Q23. What must hold for `std::reduce` to give the same result as
`std::accumulate`?**
The binary operation must be **associative and commutative**. `reduce` may
reorder/regroup operations for parallelism; `accumulate` is strictly ordered.

**Q24. What is the `overload` pattern used with `std::visit`?**
A struct that inherits `operator()` from multiple lambdas to form a visitor that
handles every alternative:
```cpp
template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...) -> overload<Ts...>;
```

**Q25. Why might `std::execution::par` be slower than `seq`?**
Thread creation/scheduling overhead. For small ranges or cheap per-element work,
that overhead exceeds the parallel speedup.

---

## 5.4 Code-Based Challenges

**C1. Safe integer parse returning `optional`.**
```cpp
std::optional<int> to_int(std::string_view s) {
    int value{};
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec == std::errc{} && ptr == s.data() + s.size()) return value;
    return std::nullopt;
}
```

**C2. Generic stringify with `if constexpr`.**
```cpp
template <typename T>
std::string stringify(const T& v) {
    if constexpr (std::is_arithmetic_v<T>) return std::to_string(v);
    else                                   return std::string(v);
}
```

**C3. Variadic `print` via fold expression.**
```cpp
template <typename... Ts>
void print(const Ts&... xs) { ((std::cout << xs << ' '), ...); std::cout << '\n'; }
```

**C4. Visit a variant with the overload pattern.**
```cpp
template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...) -> overload<Ts...>;

std::variant<int, std::string> v = "hi";
std::visit(overload{
    [](int i){ std::cout << "int " << i; },
    [](const std::string& s){ std::cout << "str " << s; }
}, v);
```

**C5. Count files in a directory tree.**
```cpp
namespace fs = std::filesystem;
std::size_t count_files(const fs::path& root) {
    std::size_t n = 0;
    for (const auto& e : fs::recursive_directory_iterator(root))
        if (e.is_regular_file()) ++n;
    return n;
}
```

**C6. Upsert into a map.**
```cpp
std::map<std::string, int> m;
m.insert_or_assign("hits", 1);   // insert
m.insert_or_assign("hits", 2);   // overwrite to 2
m.try_emplace("misses", 0);      // only if absent
```

---

## 5.5 Rapid-Fire Cheat Sheet (Interview)

| Question | One-line answer |
|----------|-----------------|
| C++17 in one phrase? | "Pragmatic release: vocabulary types + less boilerplate." |
| Killer features? | `optional`/`variant`/`string_view`, `if constexpr`, structured bindings, CTAD, filesystem. |
| `variant` vs `any`? | Fixed type set vs arbitrary type. |
| `string_view` danger? | Dangling — it doesn't own the data. |
| Empty `optional` access? | `*opt` is UB; `.value()` throws. |
| Why `if constexpr`? | Compile-time branch; discards untaken branch. |
| `vector v{5}` size? | 1 (init-list), not 5. |
| `auto [a,b]` references? | No, copies; use `auto&`. |
| `reduce` vs `accumulate`? | Parallel/unordered vs sequential/ordered. |
| Return non-movable by value? | Yes — guaranteed copy elision. |

---

## Final C++17 Recap Table

| Theme | C++14 Pain | C++17 Fix |
|-------|-----------|-----------|
| Maybe-a-value | sentinels / `pair<bool,T>` | `std::optional` |
| Type-safe union | raw union + tag | `std::variant` |
| Read-only strings | `const string&` allocates | `std::string_view` |
| Compile-time branch | tag dispatch / SFINAE | `if constexpr` |
| Unpacking pairs | `.first`/`std::tie` | structured bindings |
| Class template args | explicit / `make_*` | CTAD |
| Variadic reduce | recursion | fold expressions |
| File handling | platform/Boost | `std::filesystem` |
| Parallelism | manual threads | execution policies |

---

*End of the C++17 book. See also the companion [C++14 book »](../CPP14/README.md)*
