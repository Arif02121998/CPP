<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 4</samp>
</p>

<h1 align="center">📘 Templates — Write Once, Use for Any Type</h1>

<p align="center"><em>"Why write the same logic for int, double, and string<br/>when you can write it once and let the compiler fill in the blanks?"</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Function Templates | [§1](#-1-function-templates) |
| 2 | Class Templates | [§2](#-2-class-templates) |
| 3 | Template Specialization | [§3](#-3-template-specialization) |
| 4 | Variadic Templates | [§4](#-4-variadic-templates) |

> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. Function Templates

> *📄 See full code → `01_function_templates.cpp`*

### The Concept

Imagine you own a printing shop. A customer asks you to print invitations on white paper. Next, someone asks for the same design on blue paper. Then pink. You don't redesign the invitation each time — you create **one template** and swap out the paper color.

A **function template** works the same way. You write a function with a **placeholder** type (`T`). The compiler generates a concrete version for every type you actually use.

### Example

```cpp
#include <iostream>
#include <string>

// ── One template → works for int, double, string, anything ──
template<typename T>
T maximum(T a, T b) {
    return (a > b) ? a : b;
}

// ── Multiple type parameters ──
template<typename T, typename U>
auto add(T a, U b) {
    return a + b;       // Return type deduced by compiler
}

// ── Non-type template parameter: array size ──
template<typename T, int N>
void printArray(const T (&arr)[N]) {
    for (int i = 0; i < N; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << "(size=" << N << ")\n";
}

int main() {
    std::cout << maximum(3, 7)           << '\n';    // 7
    std::cout << maximum(3.14, 2.71)     << '\n';    // 3.14
    std::cout << maximum<std::string>("apple", "banana") << '\n';  // banana

    std::cout << add(3, 4.5) << '\n';    // 7.5 (int + double → double)

    int nums[] = {10, 20, 30, 40, 50};
    printArray(nums);                     // 10 20 30 40 50 (size=5)
}
```

### How This Works

When the compiler sees `maximum(3, 7)`, it deduces `T = int` and generates:

```cpp
int maximum(int a, int b) {
    return (a > b) ? a : b;
}
```

When it sees `maximum(3.14, 2.71)`, it generates a **separate** function with `T = double`. You wrote one template; the compiler wrote two functions. This happens at **compile time** — zero runtime cost.

`template<typename T, int N>` — `N` is a **non-type** parameter. The compiler knows the array size at compile time, so `printArray` works with arrays of any size without passing the size manually.

---

## 🔹 2. Class Templates

> *📄 See full code → `02_class_templates.cpp`*

### The Concept

Just as function templates create type-generic functions, **class templates** create type-generic data structures. `std::vector<int>`, `std::vector<std::string>`, `std::map<int, double>` — these are all class templates from the standard library with specific types plugged in.

### Example — A Generic Stack

```cpp
#include <iostream>
#include <stdexcept>

template<typename T, int MaxSize = 100>
class Stack {
    T data_[MaxSize];
    int top_;

public:
    Stack() : top_(-1) {}

    void push(const T& value) {
        if (top_ >= MaxSize - 1)
            throw std::overflow_error("Stack full!");
        data_[++top_] = value;
    }

    T pop() {
        if (top_ < 0)
            throw std::underflow_error("Stack empty!");
        return data_[top_--];
    }

    const T& peek() const {
        if (top_ < 0)
            throw std::underflow_error("Stack empty!");
        return data_[top_];
    }

    bool empty() const { return top_ < 0; }
    int size() const { return top_ + 1; }
};

int main() {
    Stack<int> intStack;           // Stack of ints, max 100
    intStack.push(10);
    intStack.push(20);
    intStack.push(30);

    while (!intStack.empty()) {
        std::cout << intStack.pop() << " ";
    }
    std::cout << '\n';

    Stack<std::string, 50> names;  // Stack of strings, max 50
    names.push("Alice");
    names.push("Bob");
    std::cout << names.peek() << '\n';
}
```

### 🖥️ Output

```
30 20 10
Bob
```

### How This Works

`Stack<int>` tells the compiler: "Generate a `Stack` where every `T` is replaced with `int`." `Stack<std::string, 50>` generates a different class where `T` = `std::string` and `MaxSize` = 50. Each instantiation is a completely independent class at the machine-code level.

The `MaxSize = 100` is a **default template argument** — if you don't specify it, you get 100.

---

## 🔹 3. Template Specialization

> *📄 See full code → `03_template_specialization.cpp`*

### The Concept

Sometimes the generic template doesn't work well for a specific type. **Specialization** lets you provide a custom implementation for that type while keeping the generic version for everything else.

> Think of it like a cookie cutter (template). The general cutter works for most dough, but for gingerbread you need a special shape.

### Example

```cpp
#include <iostream>
#include <string>

// ── PRIMARY TEMPLATE (generic) ──
template<typename T>
class TypeInfo {
public:
    static void describe() {
        std::cout << "Unknown type, size = " << sizeof(T) << " bytes\n";
    }
};

// ── FULL SPECIALIZATION for int ──
template<>
class TypeInfo<int> {
public:
    static void describe() {
        std::cout << "Integer: 32-bit signed whole number\n";
    }
};

// ── FULL SPECIALIZATION for std::string ──
template<>
class TypeInfo<std::string> {
public:
    static void describe() {
        std::cout << "String: dynamic character sequence\n";
    }
};

// ── PARTIAL SPECIALIZATION for ALL pointer types ──
template<typename T>
class TypeInfo<T*> {
public:
    static void describe() {
        std::cout << "Pointer to something, size = " << sizeof(T*) << " bytes\n";
    }
};

int main() {
    TypeInfo<double>::describe();        // Unknown type, size = 8 bytes
    TypeInfo<int>::describe();           // Integer: 32-bit signed whole number
    TypeInfo<std::string>::describe();   // String: dynamic character sequence
    TypeInfo<int*>::describe();          // Pointer to something, size = 8 bytes
}
```

### How This Works

The compiler picks the **most specific** match:
1. `TypeInfo<int>` — exact match with full specialization → uses the `int` version
2. `TypeInfo<int*>` — matches partial specialization `T*` → uses the pointer version
3. `TypeInfo<double>` — no specialization → uses the generic primary template

**Full specialization** (`template<>`) — replace ALL parameters with concrete types.
**Partial specialization** (`template<typename T>` for `T*`) — specialize for a *pattern* (all pointers), keeping some genericity.

---

## 🔹 4. Variadic Templates

> *📄 See full code → `04_variadic_templates.cpp`*

### The Concept

Normal templates take a fixed number of type parameters. **Variadic templates** take **any number** — zero, one, five, a hundred. This is how `std::tuple`, `std::make_unique`, and `printf`-like functions work under the hood.

### Example — Print Any Number of Arguments

```cpp
#include <iostream>
#include <string>

// ── Base case: no arguments left ──
void print() {
    std::cout << '\n';
}

// ── Recursive case: peel off first argument, recurse on the rest ──
template<typename T, typename... Args>
void print(T first, Args... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) std::cout << ", ";
    print(rest...);     // Recurse with remaining arguments
}

// ── C++17 Fold Expressions (simpler!) ──
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);     // Right fold: a + (b + (c + d))
}

template<typename... Args>
void printAll(Args... args) {
    ((std::cout << args << " "), ...);     // Comma fold
    std::cout << '\n';
}

int main() {
    print(1, "hello", 3.14, true, 'X');
    // Output: 1, hello, 3.14, 1, X

    std::cout << "sum = " << sum(1, 2, 3, 4, 5) << '\n';
    // Output: sum = 15

    printAll("Alice", 42, 3.14, true);
    // Output: Alice 42 3.14 1
}
```

### How This Works

When you call `print(1, "hello", 3.14)`, the compiler unrolls it step by step:

```
print(1, "hello", 3.14)
  → prints "1", calls print("hello", 3.14)
    → prints "hello", calls print(3.14)
      → prints "3.14", calls print()
        → prints '\n' (base case)
```

Each step "peels off" the first argument (`T first`) and passes the remaining pack (`Args... rest`) to the next call. The compiler generates all these functions at compile time.

**Fold expressions** (C++17) replace the recursion with a single line. `(args + ...)` expands to `arg1 + (arg2 + (arg3 + arg4))`. Much cleaner.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 5: The Standard Template Library (STL)</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
