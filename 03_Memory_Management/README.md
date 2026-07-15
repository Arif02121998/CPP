<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 3</samp>
</p>

<h1 align="center">📘 Memory Management</h1>

<p align="center"><em>"In C++ you are the landlord of your own memory.<br/>If you forget to clean up, nobody else will."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Stack vs. Heap | [§1](#-1-stack-vs-heap) |
| 2 | `new` and `delete` | [§2](#-2-new-and-delete) |
| 3 | Memory Pitfalls | [§3](#-3-memory-pitfalls) |
| 4 | RAII — The C++ Way | [§4](#-4-raii--the-c-way) |

> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. Stack vs. Heap

> *📄 See full code → `01_stack_vs_heap.cpp`*

### The Concept

Your program has two places to store data:

```
  ┌──────────────────────────┬──────────────────────────────┐
  │         STACK             │            HEAP               │
  ├──────────────────────────┼──────────────────────────────┤
  │ 🎫 Theater seats          │ 📦 Warehouse storage          │
  │ Fixed, numbered, fast     │ Any size, any time, slower    │
  ├──────────────────────────┼──────────────────────────────┤
  │ Automatic allocation      │ Manual (new / malloc)         │
  │ Auto-freed at scope end   │ Must manually free (delete)   │
  │ Very fast (pointer bump)  │ Slower (memory manager)       │
  │ Small (~8MB limit)        │ Large (limited by RAM)        │
  │ LIFO order                │ Any order alloc/dealloc       │
  │ Local variables, params   │ Dynamic objects, large data   │
  └──────────────────────────┴──────────────────────────────┘
```

### Example

```cpp
#include <iostream>

void stackExample() {
    int x = 42;               // Stack — auto-freed when function returns
    double arr[100];           // Stack — 800 bytes, auto-freed
    char name[] = "Hello";     // Stack

    std::cout << "Stack variable x = " << x << '\n';
}   // ← All stack variables destroyed here automatically

void heapExample() {
    int* p = new int(42);              // Heap — YOU must free it
    double* arr = new double[100];     // Heap — 800 bytes

    std::cout << "Heap variable *p = " << *p << '\n';

    delete p;            // Free the single int
    delete[] arr;        // Free the array (note the [])
}

int main() {
    stackExample();
    heapExample();
}
```

### How This Works

**Stack** — When you declare `int x = 42` inside a function, the compiler "pushes" it onto the stack (just increments a pointer — blazing fast). When the function returns, the stack unwinds and `x` is gone. No cleanup needed.

**Heap** — `new int(42)` asks the memory manager for space. The manager searches for a free block, marks it as used, and returns the address. When you're done, you **must** call `delete` to give it back. If you forget → **memory leak**.

> 💡 **When to use the heap:**
> - Object needs to live beyond the current function
> - Size isn't known at compile time
> - Object is too large for the stack (>1MB)
> - Object needs to be shared across parts of the program

---

## 🔹 2. `new` and `delete`

> *📄 See full code → `02_new_delete.cpp`*

### The Concept

`new` allocates memory on the heap and calls the constructor. `delete` calls the destructor and frees the memory. They always come in **matched pairs**.

### Example

```cpp
#include <iostream>
#include <string>

class Widget {
    int id_;
public:
    Widget(int id) : id_(id) {
        std::cout << "  Widget " << id_ << " constructed\n";
    }
    ~Widget() {
        std::cout << "  Widget " << id_ << " destroyed\n";
    }
};

int main() {
    // ── Single object ──
    int* pi = new int(42);
    std::cout << "*pi = " << *pi << '\n';
    delete pi;
    pi = nullptr;              // Good practice: prevent dangling pointer

    // ── Array ──
    int* arr = new int[5]{10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << '\n';
    delete[] arr;              // ⚠️ MUST use delete[] for arrays!

    // ── Objects ──
    Widget* w = new Widget(1);    // Constructor called
    delete w;                     // Destructor called
}
```

### 🖥️ Output

```
*pi = 42
10 20 30 40 50
  Widget 1 constructed
  Widget 1 destroyed
```

### Critical Rules

| Rule | Why |
|------|-----|
| Every `new` needs a `delete` | Otherwise: **memory leak** |
| Every `new[]` needs a `delete[]` | `delete` (without `[]`) on an array = **undefined behavior** |
| Set pointers to `nullptr` after delete | Prevents accidental use of freed memory |
| Never `delete` the same pointer twice | **Double free** = crash |

---

## 🔹 3. Memory Pitfalls

> *📄 See full code → `03_memory_pitfalls.cpp`*

### The Concept

C++ gives you raw power over memory. That power comes with traps. Here are the classic bugs that crash programs, leak memory, or create security vulnerabilities:

### 3a. Memory Leak — "Lost Key to the Storage Unit"

```cpp
void leak() {
    int* p = new int(42);     // Allocate
    // ... code ...
    return;                    // LEAK! 'p' is gone, but the heap memory isn't freed
}
// The memory stays allocated forever. Do this in a loop = your program eats all RAM.
```

### 3b. Dangling Pointer — "Address to a Demolished House"

```cpp
int* p = new int(42);
delete p;                      // Memory freed
// *p = 10;                    // 💥 UNDEFINED BEHAVIOR! Accessing freed memory

p = nullptr;                   // Fix: always nullify after delete
if (p) {                       // Now this check works
    std::cout << *p << '\n';
}
```

### 3c. Double Free — "Returning the Same Library Book Twice"

```cpp
int* p = new int(42);
delete p;
// delete p;                   // 💥 CRASH! Already freed

p = nullptr;                   // Fix: nullify after delete
delete p;                      // Deleting nullptr is safe (no-op)
```

### 3d. Buffer Overflow — "Writing Past the End of Your Locker"

```cpp
int* arr = new int[5];
// arr[10] = 42;              // 💥 Writing beyond allocated memory
//                            // Corrupts other data, potential security exploit
```

### The Solution? ➜ Smart Pointers!

```cpp
#include <memory>

auto ptr = std::make_unique<int>(42);    // Auto-freed when ptr goes out of scope
std::cout << *ptr << '\n';               // Use like a regular pointer
// No delete needed — EVER!
```

---

## 🔹 4. RAII — The C++ Way

> *📄 See full code → `04_raii.cpp`*

### The Concept

**RAII** = **R**esource **A**cquisition **I**s **I**nitialization.

The idea: **tie every resource to an object's lifetime.** When the object is created (constructor), acquire the resource. When the object is destroyed (destructor), release the resource. Since C++ **guarantees** destructors run when objects go out of scope, the resource is **always** cleaned up — even if an exception is thrown.

> Think of it as a self-closing door. You don't need to remember to close it — the spring mechanism (destructor) does it automatically when you walk away.

### Example — File Guard

```cpp
#include <iostream>
#include <fstream>
#include <string>

class FileGuard {
    std::ofstream file_;

public:
    FileGuard(const std::string& filename) : file_(filename) {
        if (!file_.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        std::cout << "File opened: " << filename << '\n';
    }

    void write(const std::string& data) {
        file_ << data << '\n';
    }

    ~FileGuard() {
        if (file_.is_open()) {
            file_.close();
            std::cout << "File automatically closed!\n";
        }
    }

    FileGuard(const FileGuard&) = delete;              // No copying
    FileGuard& operator=(const FileGuard&) = delete;
};

int main() {
    {
        FileGuard file("/tmp/test.txt");
        file.write("Hello, RAII!");
        file.write("Resource management done right.");
        // Even if an exception happens here, the file closes!
    }   // ← FileGuard destroyed here → file automatically closed
    std::cout << "After scope — file is already closed\n";
}
```

### 🖥️ Output

```
File opened: /tmp/test.txt
File automatically closed!
After scope — file is already closed
```

### Smart Pointers = RAII for Memory

The standard library gives you three RAII wrappers for heap memory:

```cpp
#include <memory>

// ── unique_ptr: ONE owner, auto-freed ──
auto p1 = std::make_unique<int>(42);
std::cout << *p1 << '\n';              // 42
// p1 freed automatically when it goes out of scope

// ── shared_ptr: MULTIPLE owners, freed when last owner dies ──
auto p2 = std::make_shared<int>(100);
auto p3 = p2;                          // Both point to same int
std::cout << p2.use_count() << '\n';   // 2

// ── weak_ptr: non-owning observer (breaks circular references) ──
std::weak_ptr<int> wp = p2;
if (auto locked = wp.lock()) {
    std::cout << *locked << '\n';      // 100
}
```

| Smart Pointer | Ownership | Copy? | Use Case |
|:---:|:---:|:---:|---|
| `unique_ptr` | **Exclusive** (one owner) | ❌ Move only | Default choice. "This object belongs to me alone." |
| `shared_ptr` | **Shared** (ref counted) | ✅ Yes | Multiple parts of code need the same object |
| `weak_ptr` | **None** (observer) | ✅ Yes | Break circular references between `shared_ptr`s |

### The Golden Rule

> **In modern C++, you should almost never write `new` or `delete` directly.**
> Use `std::make_unique` and `std::make_shared` instead.
> RAII guarantees no leaks, even in the face of exceptions.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 4: Templates</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
