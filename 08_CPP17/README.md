<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 8</samp>
</p>

<h1 align="center">📘 C++17 — Practical Powerups</h1>

<p align="center"><em>"C++17 made everyday code simpler, cleaner, and safer."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Structured Bindings | [§1](#-1-structured-bindings) |
| 2 | `optional`, `variant`, `any` | [§2](#-2-optional-variant-any) |
| 3 | `std::string_view` | [§3](#-3-stdstring_view) |
| 4 | `if constexpr` & Other Syntax | [§4](#-4-if-constexpr--other-syntax) |
| 5 | `std::filesystem` | [§5](#-5-stdfilesystem) |

> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. Structured Bindings

> *📄 See full code → `01_structured_bindings.cpp`*

### The Concept

**Structured bindings** let you unpack a struct, pair, tuple, or array into named variables in a single line. No more `.first` / `.second` or `std::get<0>()` clutter.

Think of it like **opening a gift box** — instead of reaching in one item at a time, you dump everything out onto the table with names.

### Example

```cpp
#include <iostream>
#include <tuple>
#include <map>
#include <string>

int main() {
    // ── From a struct ──
    struct Point { double x, y, z; };
    Point p{1.0, 2.0, 3.0};
    auto [x, y, z] = p;
    std::cout << "x=" << x << " y=" << y << " z=" << z << '\n';

    // ── From a tuple ──
    std::tuple<int, std::string, double> student{42, "Alice", 3.8};
    auto [id, name, gpa] = student;
    std::cout << name << " (ID:" << id << ") GPA:" << gpa << '\n';

    // ── From a pair ──
    std::pair<std::string, int> person{"Bob", 25};
    auto [pname, age] = person;

    // ── Game changer: map iteration ──
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}, {"Charlie", 92}};

    for (const auto& [student, score] : scores) {
        std::cout << student << ": " << score << '\n';
    }
    // Compare with C++11:
    // for (const auto& pair : scores) {
    //     std::cout << pair.first << ": " << pair.second << '\n';
    // }

    // ── With if-initializer ──
    if (auto [iter, success] = scores.insert({"Eve", 91}); success) {
        std::cout << "Added " << iter->first << '\n';
    }
}
```

### 🖥️ Output

```
x=1 y=2 z=3
Alice (ID:42) GPA:3.8
Alice: 95
Bob: 87
Charlie: 92
Added Eve
```

The `auto [key, value]` syntax is purely syntactic sugar — the compiler generates the same code as accessing members directly. Zero overhead, maximum readability.

---

## 🔹 2. `optional`, `variant`, `any`

> *📄 See full code → `02_optional_variant_any.cpp`*

### The Concept

C++17 adds three vocabulary types that replace common C-era patterns:

| Type | Replaces | Meaning |
|------|----------|---------|
| `std::optional<T>` | Returning `T*` or `T` + `bool` | "Maybe a value, maybe nothing" |
| `std::variant<T, U, ...>` | `union` + `enum` tag | "One of these types, type-safe" |
| `std::any` | `void*` | "Literally anything" |

### `std::optional` — A Value That Might Be Missing

```cpp
#include <optional>
#include <iostream>
#include <string>

std::optional<std::string> findUser(int id) {
    if (id == 1) return "Alice";
    if (id == 2) return "Bob";
    return std::nullopt;               // "No value"
}

int main() {
    auto user = findUser(1);

    if (user) {                        // Check: does it have a value?
        std::cout << "Found: " << *user << '\n';    // Dereference to get value
    }

    auto missing = findUser(99);
    std::cout << missing.value_or("Unknown") << '\n';   // "Unknown" (default)
}
```

No more returning `nullptr` and checking for null. No more returning a `std::pair<T, bool>`. `optional` makes the intent crystal clear.

### `std::variant` — A Type-Safe Union

```cpp
#include <variant>
#include <iostream>
#include <string>

int main() {
    std::variant<int, double, std::string> v;

    v = 42;
    std::cout << std::get<int>(v) << '\n';           // 42

    v = 3.14;
    std::cout << std::get<double>(v) << '\n';        // 3.14

    v = std::string("hello");
    std::cout << std::get<std::string>(v) << '\n';   // hello

    // ── Safe access ──
    if (auto* p = std::get_if<double>(&v)) {
        std::cout << "It's a double: " << *p << '\n';
    } else {
        std::cout << "Not a double right now\n";     // ← This prints
    }

    // ── Visitor pattern ──
    std::visit([](const auto& val) {
        std::cout << "Current value: " << val << '\n';
    }, v);   // "Current value: hello"
}
```

A `variant` can hold **one of** the listed types at any time. Unlike a raw `union`, it **knows** which type it currently holds and throws `std::bad_variant_access` if you try to read the wrong type.

### `std::any` — Hold Anything

```cpp
#include <any>
#include <iostream>
#include <string>

int main() {
    std::any x = 42;
    std::cout << std::any_cast<int>(x) << '\n';       // 42

    x = std::string("hello");
    std::cout << std::any_cast<std::string>(x) << '\n'; // hello

    // Wrong type → throws std::bad_any_cast
    try {
        std::any_cast<int>(x);
    } catch (const std::bad_any_cast& e) {
        std::cout << "Error: " << e.what() << '\n';
    }
}
```

> 💡 **Guideline:** Prefer `optional` over `variant` over `any`. Use the *most specific* type that fits your need.

---

## 🔹 3. `std::string_view`

> *📄 See full code → `03_string_view.cpp`*

### The Concept

`std::string_view` is a **read-only, non-owning window** into a string. It doesn't allocate memory or copy characters — it just stores a pointer and a length.

> Think of it as looking through a window at someone's garden. You can see everything, but you don't own the garden and you can't change it. And "looking" costs nothing.

### Example

```cpp
#include <iostream>
#include <string>
#include <string_view>

// Takes a string_view: works with std::string, C-string, or substring — no copies!
void greet(std::string_view name) {
    std::cout << "Hello, " << name << "!\n";
}

int main() {
    std::string s = "Hello, Beautiful World!";
    std::string_view sv = s;

    // ── Zero-cost substrings ──
    auto word = sv.substr(7, 9);         // "Beautiful" — no allocation!
    std::cout << word << '\n';

    // ── Works with any string type ──
    greet("Alice");                      // C-string literal
    greet(std::string("Bob"));           // std::string
    greet(sv.substr(7, 9));              // Another string_view

    // ── Useful operations ──
    std::cout << "Length: " << sv.length() << '\n';
    std::cout << "Starts with 'Hello': " << sv.starts_with("Hello") << '\n';

    auto pos = sv.find("World");
    if (pos != std::string_view::npos) {
        std::cout << "'World' found at position " << pos << '\n';
    }
}
```

### When to Use `string_view`

| Use `string_view` | Use `std::string` |
|---|---|
| Function parameters (read-only) | Need to own / modify the string |
| Parsing / extracting substrings | Need string to outlive the source |
| High-performance string processing | Concatenation or mutation |

> ⚠️ **Danger:** A `string_view` is only valid as long as the underlying string exists. Returning a `string_view` to a local `std::string` = dangling reference!

---

## 🔹 4. `if constexpr` & Other Syntax

> *📄 See full code → `04_if_constexpr.cpp`*

### `if constexpr` — Compile-Time Branching

Normal `if` runs at runtime. **`if constexpr`** resolves at **compile time** — the false branch is *discarded entirely*, not compiled. This is essential in templates where different types need completely different code.

```cpp
#include <iostream>
#include <string>
#include <type_traits>

template<typename T>
std::string describe(T value) {
    if constexpr (std::is_integral_v<T>) {
        return "Integer: " + std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "Float: " + std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "String: " + value;
    } else {
        return "Unknown type";
    }
}

int main() {
    std::cout << describe(42)           << '\n';   // Integer: 42
    std::cout << describe(3.14)         << '\n';   // Float: 3.140000
    std::cout << describe(std::string("Hello")) << '\n';   // String: Hello
}
```

Without `if constexpr`, calling `std::to_string(value)` when `T = std::string` would fail to compile (can't convert string to string via `to_string`). With `if constexpr`, that branch is completely removed for `T = std::string`.

### Nested Namespaces

```cpp
// ── C++11/14: ──
namespace App { namespace Math { namespace Geometry {
    double circleArea(double r) { return 3.14159 * r * r; }
}}}

// ── C++17: ──
namespace App::Math::Geometry {
    double circleArea(double r) { return 3.14159 * r * r; }
}
```

### Attributes

```cpp
[[nodiscard]] int compute() { return 42; }
// compute();    // ⚠️ Warning: ignoring return value

[[maybe_unused]] int debug = 1;  // No warning even if unused

switch (level) {
    case 3: handle3();  [[fallthrough]];  // Intentional: no warning
    case 2: handle2();  break;
}
```

### CTAD — Class Template Argument Deduction

```cpp
std::pair p(1, 2.5);             // pair<int, double>  — no need to write types!
std::tuple t(1, "hello", 3.14);  // tuple<int, const char*, double>
std::vector v{1, 2, 3};          // vector<int>
```

---

## 🔹 5. `std::filesystem`

> *📄 See full code → `05_filesystem.cpp`*

### The Concept

Before C++17, accessing the filesystem required platform-specific code (`<dirent.h>` on Linux, `<windows.h>` on Windows). C++17 provides a **portable, comprehensive filesystem library**.

### Example

```cpp
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    // ── Path manipulation ──
    fs::path p = "/home/user/documents/report.txt";

    std::cout << "Full path:  " << p             << '\n';
    std::cout << "Filename:   " << p.filename()  << '\n';   // report.txt
    std::cout << "Extension:  " << p.extension() << '\n';   // .txt
    std::cout << "Parent dir: " << p.parent_path() << '\n'; // /home/user/documents
    std::cout << "Stem:       " << p.stem()      << '\n';   // report

    // ── Build paths safely (uses correct separator) ──
    fs::path file = fs::path("/home") / "user" / "main.cpp";
    std::cout << file << '\n';   // /home/user/main.cpp

    // ── Check existence ──
    if (fs::exists("/tmp")) {
        std::cout << "/tmp exists and is a directory: "
                  << fs::is_directory("/tmp") << '\n';
    }

    // ── Create directories ──
    fs::create_directories("/tmp/test/sub1/sub2");    // Creates all levels

    // ── Iterate a directory ──
    for (const auto& entry : fs::directory_iterator("/tmp/test")) {
        std::cout << entry.path().filename();
        if (entry.is_directory()) std::cout << "/";
        std::cout << '\n';
    }

    // ── Disk space ──
    auto info = fs::space("/");
    std::cout << "Disk capacity: " << info.capacity / (1024*1024*1024) << " GB\n";
    std::cout << "Available:     " << info.available / (1024*1024*1024) << " GB\n";
}
```

### Key Operations

| Operation | Code |
|-----------|------|
| Check existence | `fs::exists(path)` |
| Is file / dir? | `fs::is_regular_file(p)` / `fs::is_directory(p)` |
| Create dirs | `fs::create_directories(p)` |
| Copy file | `fs::copy_file(src, dst)` |
| Rename / move | `fs::rename(old, new_)` |
| Delete | `fs::remove(p)` or `fs::remove_all(p)` |
| File size | `fs::file_size(p)` |
| Iterate dir | `fs::directory_iterator(p)` |
| Recursive iter | `fs::recursive_directory_iterator(p)` |

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 9: C++20 — The Big Four</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
