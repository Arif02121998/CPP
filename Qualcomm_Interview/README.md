<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>Q U A L C O M M &nbsp; I N T E R V I E W</samp>
</p>

<h1 align="center">📘 Qualcomm C/C++ Interview — Questions & Answers</h1>

<p align="center"><em>"Embedded systems, real-time software, and low-level mastery — the Qualcomm way."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Section | Jump |
|:-:|---------|:----:|
| 1 | C Language Fundamentals | [§1](#-1-c-language-fundamentals) |
| 2 | C++ Core Concepts | [§2](#-2-c-core-concepts) |
| 3 | Memory Management | [§3](#-3-memory-management) |
| 4 | Pointers & References | [§4](#-4-pointers--references) |
| 5 | OOP & Design | [§5](#-5-oop--design) |
| 6 | Templates & STL | [§6](#-6-templates--stl) |
| 7 | Multithreading & Concurrency | [§7](#-7-multithreading--concurrency) |
| 8 | Embedded & Low-Level | [§8](#-8-embedded--low-level) |
| 9 | RTOS & Real-Time Concepts | [§9](#-9-rtos--real-time-concepts) |
| 10 | Data Structures & Algorithms | [§10](#-10-data-structures--algorithms) |
| 11 | Networking & Protocols | [§11](#-11-networking--protocols) |
| 12 | Linux / QNX Internals | [§12](#-12-linux--qnx-internals) |
| 13 | Debugging & Tools | [§13](#-13-debugging--tools) |
| 14 | Design Patterns | [§14](#-14-design-patterns) |
| 15 | Coding Problems (Whiteboard) | [§15](#-15-coding-problems-whiteboard) |
| 16 | Behavioral / System Design | [§16](#-16-behavioral--system-design) |

---

## 🔹 1. C Language Fundamentals

---

### Q1. What is the difference between `const int *p`, `int *const p`, and `const int *const p`?

**Answer:**

```c
const int *p;        // Pointer to a constant int — can't change the VALUE through p
int *const p;        // Constant pointer to int — can't change WHERE p POINTS
const int *const p;  // Both — can't change value OR pointer
```

Read it **right to left**:
- `const int *p` → `p` is a pointer to `int` that is `const` → value is constant
- `int *const p` → `p` is a `const` pointer to `int` → pointer is constant
- `const int *const p` → `p` is a `const` pointer to `const int` → both locked

```c
int x = 10, y = 20;

const int *p1 = &x;
// *p1 = 30;         // ❌ Error: can't modify value
p1 = &y;             // ✅ OK: can change where it points

int *const p2 = &x;
*p2 = 30;            // ✅ OK: can modify value
// p2 = &y;          // ❌ Error: can't change pointer

const int *const p3 = &x;
// *p3 = 30;         // ❌ Error
// p3 = &y;          // ❌ Error
```

---

### Q2. What is the difference between `struct` and `union`?

**Answer:**

| Feature | `struct` | `union` |
|---------|----------|---------|
| Memory | Each member gets its own memory | All members share the SAME memory |
| Size | Sum of all members (+ padding) | Size of the LARGEST member |
| Access | All members valid simultaneously | Only ONE member valid at a time |

```c
struct MyStruct {
    int   i;    // 4 bytes
    float f;    // 4 bytes
    char  c;    // 1 byte
};
// sizeof(MyStruct) = 12 (with padding)

union MyUnion {
    int   i;    // 4 bytes
    float f;    // 4 bytes  ← shares same 4 bytes
    char  c;    // 1 byte   ← shares same bytes
};
// sizeof(MyUnion) = 4 (largest member)
```

**Qualcomm context:** Unions are heavily used in embedded systems for register overlays and protocol parsing — reading the same memory as different types.

---

### Q3. What are `volatile` and `restrict` qualifiers?

**Answer:**

**`volatile`** tells the compiler: "This variable can change unexpectedly — don't optimize reads/writes to it."

```c
volatile int *hardware_reg = (volatile int *)0x40021000;

// Without volatile: compiler might cache the value and read it only once
// With volatile: compiler reads from memory EVERY time
while (*hardware_reg & 0x01) {
    // Wait for hardware bit to clear
}
```

**When to use `volatile`:**
- Memory-mapped hardware registers
- Variables modified by an ISR (interrupt service routine)
- Variables shared between threads (though `atomic` is preferred in C11/C++)

**`restrict`** (C99, not in C++) tells the compiler: "This pointer is the ONLY way to access this memory — no aliasing."

```c
void add_arrays(int *restrict a, int *restrict b, int *restrict result, int n) {
    for (int i = 0; i < n; i++)
        result[i] = a[i] + b[i];
}
// Compiler can vectorize aggressively because a, b, result don't overlap
```

---

### Q4. Explain `static` in C — all its uses.

**Answer:**

`static` has **three different meanings** depending on context:

```c
// 1. Static local variable — persists across function calls
void counter() {
    static int count = 0;    // Initialized ONCE, lives forever
    count++;
    printf("%d\n", count);
}
// counter() → 1, counter() → 2, counter() → 3

// 2. Static global variable / function — file scope (internal linkage)
static int secret = 42;            // Only visible in THIS .c file
static void helper() { /* ... */ } // Only callable from THIS .c file

// 3. Static in structs — N/A in C (but in C++, static means class-level)
```

| Context | Meaning |
|---------|---------|
| Inside a function | Lifetime = entire program, scope = local to function |
| At file scope | Restricts visibility to the translation unit (file) |
| In a class (C++) | Shared across all instances, belongs to the class |

---

### Q5. What is the difference between `#define`, `const`, and `enum`?

**Answer:**

```c
#define MAX_SIZE 100          // Preprocessor text replacement — no type, no scope
const int MAX_SIZE = 100;    // Typed constant — has scope, debuggable
enum { MAX_SIZE = 100 };     // Compile-time integer constant — has scope
```

| Feature | `#define` | `const` | `enum` |
|---------|-----------|---------|--------|
| Type checking | ❌ None | ✅ Yes | ✅ Yes (int) |
| Scope | ❌ Global (until `#undef`) | ✅ Respects scope | ✅ Respects scope |
| Debuggable | ❌ Replaced before compilation | ✅ Has symbol | ✅ Has symbol |
| Memory | ❌ No storage | ✅ May use storage | ❌ No storage |
| Can be pointer | ❌ No | ✅ Yes | ❌ No |

**Qualcomm preference:** Use `enum` or `constexpr` (C++) for integer constants. Use `const` for typed values. Avoid `#define` for constants.

---

### Q6. What is `typedef` vs `#define` for creating aliases?

**Answer:**

```c
typedef int* IntPtr;
#define INTPTR int*

IntPtr a, b;    // Both a AND b are int*
INTPTR c, d;    // c is int*, but d is just int! (text substitution: int* c, d;)
```

`typedef` understands types. `#define` is dumb text replacement.

---

### Q7. Explain bit manipulation — set, clear, toggle, check a bit.

**Answer:**

```c
#define BIT(n) (1U << (n))

uint32_t reg = 0;

// Set bit 3
reg |= BIT(3);           // reg = 0000 1000

// Clear bit 3
reg &= ~BIT(3);          // reg = 0000 0000

// Toggle bit 3
reg ^= BIT(3);           // Flips bit 3

// Check if bit 3 is set
if (reg & BIT(3)) {
    // Bit 3 is set
}

// Set bits 3-5
reg |= (0x7 << 3);       // 0x7 = 111 in binary, shifted left by 3

// Extract bits 4-7 from a register
uint8_t field = (reg >> 4) & 0x0F;
```

**Qualcomm context:** Bit manipulation is used constantly for hardware register access, protocol flags, and memory-efficient data packing.

---

### Q8. What is structure padding and how do you control it?

**Answer:**

CPUs access memory most efficiently at aligned addresses. The compiler inserts **padding bytes** to align struct members:

```c
struct Bad {
    char  a;     // 1 byte
                 // 3 bytes padding (to align int to 4-byte boundary)
    int   b;     // 4 bytes
    char  c;     // 1 byte
                 // 3 bytes padding (to align struct size to 4)
};
// sizeof = 12 (only 6 useful bytes!)

struct Good {
    int   b;     // 4 bytes
    char  a;     // 1 byte
    char  c;     // 1 byte
                 // 2 bytes padding
};
// sizeof = 8 (reordered members to minimize padding)
```

**Control padding:**

```c
// Pack tightly — no padding (may cause slower unaligned access)
#pragma pack(push, 1)
struct Packed {
    char  a;
    int   b;
    char  c;
};
#pragma pack(pop)
// sizeof = 6

// GCC/Clang attribute
struct __attribute__((packed)) Packed2 {
    char  a;
    int   b;
    char  c;
};
```

> ⚠️ **Warning:** Packed structs may cause **alignment faults** on some ARM processors (like Qualcomm Snapdragon). Use with caution on embedded targets.

---

## 🔹 2. C++ Core Concepts

---

### Q9. What are lvalue and rvalue? What is move semantics?

**Answer:**

```
lvalue — has a name and a persistent address (left side of =)
rvalue — temporary, about to die (right side of =, function returns)
```

```cpp
int x = 10;           // x = lvalue, 10 = rvalue
int y = x + 5;        // y = lvalue, (x + 5) = rvalue
std::string s = "hi"; // s = lvalue, "hi" = rvalue
```

**Move semantics** (C++11) let you *transfer* resources from an rvalue instead of copying:

```cpp
std::vector<int> createData() {
    std::vector<int> v(1000000);
    return v;    // Moved, not copied (compiler uses move constructor)
}

std::vector<int> data = createData();    // Zero copies thanks to move

// Explicit move:
std::string a = "hello";
std::string b = std::move(a);    // a is now empty, b owns "hello"
```

**Why Qualcomm cares:** In embedded/real-time systems, unnecessary copies waste CPU cycles and memory. Move semantics eliminate that overhead.

---

### Q10. What is the Rule of Five (and Rule of Zero)?

**Answer:**

If your class manages a resource (raw pointer, file handle, mutex), you likely need ALL five:

```cpp
class Buffer {
    int* data_;
    size_t size_;
public:
    // 1. Constructor
    Buffer(size_t n) : data_(new int[n]), size_(n) {}

    // 2. Destructor
    ~Buffer() { delete[] data_; }

    // 3. Copy constructor
    Buffer(const Buffer& o) : data_(new int[o.size_]), size_(o.size_) {
        std::copy(o.data_, o.data_ + size_, data_);
    }

    // 4. Copy assignment
    Buffer& operator=(const Buffer& o) {
        if (this != &o) {
            Buffer tmp(o);          // Copy-and-swap idiom
            std::swap(data_, tmp.data_);
            std::swap(size_, tmp.size_);
        }
        return *this;
    }

    // 5. Move constructor
    Buffer(Buffer&& o) noexcept : data_(o.data_), size_(o.size_) {
        o.data_ = nullptr;
        o.size_ = 0;
    }

    // 6. Move assignment
    Buffer& operator=(Buffer&& o) noexcept {
        if (this != &o) {
            delete[] data_;
            data_ = o.data_;  size_ = o.size_;
            o.data_ = nullptr; o.size_ = 0;
        }
        return *this;
    }
};
```

**Rule of Zero:** If you use smart pointers and RAII containers, you need NONE of the five:

```cpp
class Modern {
    std::vector<int> data_;    // Vector handles everything
    std::string name_;         // String handles everything
    // No destructor, no copy/move constructors needed!
};
```

---

### Q11. What is `constexpr` vs `const` vs `#define`?

**Answer:**

| Feature | `#define` | `const` | `constexpr` |
|---------|-----------|---------|-------------|
| Evaluated | Preprocessor | May be runtime | **Must** be compile-time |
| Type safe | ❌ | ✅ | ✅ |
| Can use in `switch` | ✅ | ❌ (non-integral) | ✅ |
| Can be function | ❌ | ❌ | ✅ |

```cpp
#define SQ(x) ((x) * (x))       // Macro — no type safety, watch for side effects
const int SIZE = 100;            // Might be computed at runtime
constexpr int SIZE = 100;        // GUARANTEED compile-time

constexpr int square(int x) { return x * x; }
constexpr int val = square(5);   // 25, computed at compile time

// constexpr if (C++17)
template<typename T>
auto process(T t) {
    if constexpr (std::is_integral_v<T>)
        return t * 2;
    else
        return t + 0.5;
}
```

---

### Q12. What is the difference between `new`/`delete` and `malloc`/`free`?

**Answer:**

| Feature | `malloc`/`free` | `new`/`delete` |
|---------|----------------|----------------|
| Language | C | C++ |
| Type safety | Returns `void*`, needs cast | Returns correct type |
| Constructor | ❌ Not called | ✅ Called |
| Destructor | ❌ Not called | ✅ Called |
| Overloadable | ❌ | ✅ (operator new) |
| On failure | Returns `NULL` | Throws `std::bad_alloc` |

```cpp
// C way
int* p1 = (int*)malloc(10 * sizeof(int));   // No constructor
free(p1);                                    // No destructor

// C++ way
int* p2 = new int[10];          // Default-initialized
delete[] p2;                    // Destructor called for each element

// Modern C++ way — PREFERRED
auto p3 = std::make_unique<int[]>(10);   // No manual delete needed
```

---

## 🔹 3. Memory Management

---

### Q13. What are stack, heap, data, BSS, and text segments?

**Answer:**

```
┌─────────────────────┐  High Address
│       Stack         │  ← Local variables, function calls (grows DOWN)
│         ↓           │
│                     │
│         ↑           │
│        Heap         │  ← malloc/new (grows UP)
├─────────────────────┤
│   BSS (uninitialized│  ← Global/static vars initialized to 0
│       data)         │
├─────────────────────┤
│   Data (initialized │  ← Global/static vars with initial values
│       data)         │
├─────────────────────┤
│       Text          │  ← Executable code (read-only)
└─────────────────────┘  Low Address
```

```c
int global_init = 42;        // Data segment
int global_uninit;           // BSS segment (zeroed)
static int file_static;     // BSS segment

void foo() {
    int local = 10;          // Stack
    static int s = 5;       // Data segment (persists)
    int *p = malloc(100);   // p on stack, 100 bytes on heap
    free(p);
}

const char *str = "hello";  // "hello" in Text (read-only), str in Data
```

---

### Q14. What is a memory leak and how do you detect it?

**Answer:**

A **memory leak** happens when you allocate memory but never free it. Over time, the program consumes more and more RAM until it crashes or the system slows down.

```cpp
void leak() {
    int* p = new int[1000];    // Allocated
    return;                     // p goes out of scope — memory leaked!
}

void noLeak() {
    auto p = std::make_unique<int[]>(1000);   // RAII — auto-freed
    return;                                     // unique_ptr destructor frees it
}
```

**Detection tools:**

| Tool | Usage |
|------|-------|
| **Valgrind** | `valgrind --leak-check=full ./program` |
| **AddressSanitizer** | `-fsanitize=address` (compile flag) |
| **heaptrack** | `heaptrack ./program && heaptrack_gui heaptrack.*.gz` |
| **mtrace** (glibc) | `MALLOC_TRACE=trace.log ./program && mtrace ./program trace.log` |

**Qualcomm context:** On embedded devices with limited RAM, even small leaks are critical. Memory profiling tools are essential.

---

### Q15. Explain smart pointers — `unique_ptr`, `shared_ptr`, `weak_ptr`.

**Answer:**

```cpp
// unique_ptr — exclusive ownership. Cannot be copied.
std::unique_ptr<int> p1 = std::make_unique<int>(42);
// std::unique_ptr<int> p2 = p1;          // ❌ Error: deleted copy
std::unique_ptr<int> p3 = std::move(p1);  // ✅ Transfer ownership

// shared_ptr — shared ownership. Reference counted.
std::shared_ptr<int> s1 = std::make_shared<int>(42);
std::shared_ptr<int> s2 = s1;    // ref count = 2
s1.reset();                       // ref count = 1, memory still alive
s2.reset();                       // ref count = 0, memory freed

// weak_ptr — non-owning observer. Doesn't affect ref count.
std::shared_ptr<int> owner = std::make_shared<int>(42);
std::weak_ptr<int> observer = owner;

if (auto locked = observer.lock()) {   // Temporarily get shared_ptr
    std::cout << *locked << "\n";       // 42
}
owner.reset();                          // Memory freed
// observer.lock() now returns nullptr — safe, no dangling
```

**When to use what:**

| Smart Pointer | When |
|---------------|------|
| `unique_ptr` | Default choice. Single owner. Zero overhead. |
| `shared_ptr` | Multiple objects share ownership of the same resource. |
| `weak_ptr` | Break circular references. Observe without owning (caches, back-pointers). |

---

### Q16. What is a dangling pointer? Double free? Buffer overflow?

**Answer:**

```cpp
// DANGLING POINTER — points to freed memory
int* p = new int(42);
delete p;
// p still holds the old address — using it is UNDEFINED BEHAVIOR
*p = 10;    // 💀 Dangling pointer dereference

// Fix: set to nullptr after delete
delete p;
p = nullptr;

// DOUBLE FREE — freeing the same memory twice
int* q = new int(42);
delete q;
delete q;    // 💀 Double free — heap corruption

// BUFFER OVERFLOW — writing beyond allocated boundary
int arr[5];
arr[10] = 42;    // 💀 Writes to unowned memory — stack smashing

char buf[8];
strcpy(buf, "This string is way too long");  // 💀 Buffer overflow
// Fix: strncpy(buf, str, sizeof(buf) - 1);  buf[sizeof(buf)-1] = '\0';
```

**Qualcomm context:** Buffer overflows are security vulnerabilities (CVEs). Qualcomm code must pass static analysis (Coverity, Klocwork) and dynamic analysis (ASan).

---

## 🔹 4. Pointers & References

---

### Q17. What is a function pointer? How is it used in C callbacks?

**Answer:**

```c
// Declaration: return_type (*name)(parameter_types)
int (*operation)(int, int);

int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

operation = add;
printf("%d\n", operation(3, 4));   // 7

operation = mul;
printf("%d\n", operation(3, 4));   // 12
```

**Callback pattern** (extremely common in Qualcomm HAL/driver code):

```c
typedef void (*EventCallback)(int event_id, void* user_data);

void register_callback(EventCallback cb, void* data) {
    // Store cb and data, call later when event occurs
    cb(42, data);
}

void my_handler(int event_id, void* data) {
    printf("Event %d received, data=%s\n", event_id, (char*)data);
}

int main() {
    register_callback(my_handler, "sensor");
    // Output: Event 42 received, data=sensor
}
```

---

### Q18. What is a `void*` pointer? When is it used?

**Answer:**

`void*` is a **generic pointer** — it can point to any type but cannot be dereferenced directly.

```c
void* p;
int x = 42;
float f = 3.14;

p = &x;                       // ✅ void* can hold any pointer
printf("%d\n", *(int*)p);     // Must cast before dereferencing

p = &f;
printf("%f\n", *(float*)p);   // Cast to correct type

// Common uses:
// 1. Generic functions (qsort, bsearch, pthread_create)
// 2. Callback user_data parameters
// 3. Hardware abstraction layers
```

---

### Q19. What is the difference between pass by value, pointer, and reference?

**Answer:**

```cpp
void byValue(int x)     { x = 100; }     // Modifies LOCAL copy
void byPointer(int* p)  { *p = 100; }    // Modifies ORIGINAL via address
void byReference(int& r){ r = 100; }     // Modifies ORIGINAL via alias

int main() {
    int a = 1, b = 1, c = 1;
    byValue(a);       // a = 1  (unchanged)
    byPointer(&b);    // b = 100
    byReference(c);   // c = 100
}
```

| Method | Can modify original? | Can be null? | Syntax at call site |
|--------|:-------------------:|:------------:|:------------------:|
| Value | ❌ | N/A | `f(x)` |
| Pointer | ✅ | ✅ | `f(&x)` |
| Reference | ✅ | ❌ | `f(x)` |

---

## 🔹 5. OOP & Design

---

### Q20. What is virtual function? What is vtable?

**Answer:**

A `virtual` function enables **runtime polymorphism** — the actual function called depends on the object's real type, not the pointer type.

```cpp
class Shape {
public:
    virtual void draw() { std::cout << "Shape\n"; }
    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    void draw() override { std::cout << "Circle\n"; }
};

Shape* s = new Circle();
s->draw();    // "Circle" — not "Shape"! Dynamic dispatch via vtable.
delete s;     // Virtual destructor ensures Circle's destructor runs
```

**How vtable works:**

```
  ┌──────────────┐
  │ Circle object│
  │ ┌──────────┐ │      ┌──────────────────────┐
  │ │ vptr ────┼──┼─────→│ Circle's vtable       │
  │ ├──────────┤ │      │ [0] → Circle::draw()  │
  │ │ data...  │ │      │ [1] → Shape::~Shape() │
  │ └──────────┘ │      └──────────────────────┘
  └──────────────┘
```

- Each class with virtual functions has a **vtable** (array of function pointers).
- Each object has a hidden **vptr** pointing to its class's vtable.
- Virtual call: `object->vptr->vtable[index]()` — one pointer dereference overhead.

**Qualcomm context:** Virtual function overhead (cache miss on vtable lookup) matters in hot loops. Use CRTP for static polymorphism when performance is critical.

---

### Q21. What is the diamond problem? How does virtual inheritance solve it?

**Answer:**

```cpp
class A { public: int x; };
class B : public A {};
class C : public A {};
class D : public B, public C {};

D d;
// d.x is AMBIGUOUS — does it mean B::A::x or C::A::x?

// Fix: Virtual inheritance
class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};
// Now only ONE copy of A exists in D
D d;
d.x = 42;    // ✅ Unambiguous
```

```
   WITHOUT virtual            WITH virtual
     A     A                      A
     |     |                     / \
     B     C                    B   C
      \   /                      \ /
        D                         D
   (2 copies of A)         (1 copy of A)
```

---

### Q22. What is an abstract class vs an interface in C++?

**Answer:**

```cpp
// Abstract class — has at least one pure virtual function
// Can have data members and implemented methods
class AbstractShape {
protected:
    std::string name_;
public:
    AbstractShape(const std::string& n) : name_(n) {}
    virtual double area() const = 0;       // Pure virtual — MUST override
    void printName() const { std::cout << name_ << "\n"; }  // Implemented
    virtual ~AbstractShape() = default;
};

// Interface — ALL methods pure virtual, no data members (convention)
class IDrawable {
public:
    virtual void draw() const = 0;
    virtual void resize(double factor) = 0;
    virtual ~IDrawable() = default;
};

// Concrete class implements both
class Circle : public AbstractShape, public IDrawable {
    double r_;
public:
    Circle(double r) : AbstractShape("Circle"), r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; }
    void draw() const override { /* ... */ }
    void resize(double f) override { r_ *= f; }
};
```

---

## 🔹 6. Templates & STL

---

### Q23. What is template specialization? When would you use it?

**Answer:**

```cpp
// Primary template
template<typename T>
std::string typeName() { return "unknown"; }

// Full specialization for int
template<>
std::string typeName<int>() { return "int"; }

// Full specialization for double
template<>
std::string typeName<double>() { return "double"; }

std::cout << typeName<int>() << "\n";      // "int"
std::cout << typeName<char>() << "\n";     // "unknown"
```

**Partial specialization** (only for class templates):

```cpp
// Primary
template<typename T, typename U>
struct Pair { /* general */ };

// Partial: when both types are the same
template<typename T>
struct Pair<T, T> { /* optimized for same-type pair */ };

// Partial: when second type is a pointer
template<typename T, typename U>
struct Pair<T, U*> { /* handles pointer case */ };
```

**Qualcomm use case:** Specialize a serialization template for hardware-specific types or protocol buffers.

---

### Q24. What is the difference between `std::map` and `std::unordered_map`?

**Answer:**

| Feature | `std::map` | `std::unordered_map` |
|---------|-----------|---------------------|
| Underlying DS | Red-Black Tree | Hash Table |
| Order | Sorted by key | No order |
| Lookup | O(log n) | O(1) average, O(n) worst |
| Insert | O(log n) | O(1) average |
| Memory | Less (no hash table overhead) | More (buckets + load factor) |
| Key requirement | `operator<` | `std::hash` + `operator==` |
| Iterator invalidation | Stable | Rehash invalidates all |

```cpp
// Use map when you need sorted order or range queries
std::map<std::string, int> sorted;
auto it = sorted.lower_bound("M");    // Efficient range query

// Use unordered_map when you need fastest lookup
std::unordered_map<int, std::string> lookup;
lookup[42] = "answer";    // O(1) insert and lookup
```

---

### Q25. How does `std::vector` grow? What is capacity vs size?

**Answer:**

```
vector with size=4, capacity=4:
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ 4 │
└───┴───┴───┴───┘

push_back(5) triggers REALLOCATION:
1. Allocate new buffer (capacity = 8, typically doubles)
2. Move all elements to new buffer
3. Add element 5
4. Free old buffer

┌───┬───┬───┬───┬───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ 4 │ 5 │   │   │   │
└───┴───┴───┴───┴───┴───┴───┴───┘
 size=5              capacity=8
```

```cpp
std::vector<int> v;
v.reserve(1000);          // Pre-allocate capacity — avoids reallocations
v.size();                  // Number of elements (0)
v.capacity();              // Allocated slots (1000)
v.shrink_to_fit();         // Release unused capacity
```

**Performance tip:** Always `reserve()` if you know the approximate size. Reallocation is expensive — it copies/moves ALL elements.

---

## 🔹 7. Multithreading & Concurrency

---

### Q26. What is a race condition? How do you prevent it?

**Answer:**

A **race condition** occurs when two threads access shared data simultaneously and at least one is writing.

```cpp
int counter = 0;

// TWO THREADS RUNNING THIS:
void increment() {
    for (int i = 0; i < 100000; i++)
        counter++;    // NOT atomic! Read-modify-write = 3 steps
}
// Result: counter < 200000 (data race)
```

**Prevention methods:**

```cpp
// 1. Mutex
std::mutex mtx;
void safe_increment() {
    std::lock_guard<std::mutex> lock(mtx);
    counter++;
}

// 2. Atomic
std::atomic<int> atomic_counter{0};
void atomic_increment() {
    atomic_counter++;    // Single hardware instruction
}

// 3. Thread-local storage
thread_local int local_counter = 0;    // Each thread has its own copy
```

---

### Q27. What is a deadlock? How do you avoid it?

**Answer:**

**Deadlock** occurs when two threads each hold a lock and wait for the other's lock — forever.

```
Thread A: lock(mutex1) → waiting for mutex2
Thread B: lock(mutex2) → waiting for mutex1
→ DEADLOCK ☠️
```

```cpp
std::mutex m1, m2;

// ❌ DEADLOCK PRONE
void threadA() { std::lock_guard l1(m1); std::lock_guard l2(m2); }
void threadB() { std::lock_guard l1(m2); std::lock_guard l2(m1); }  // Reversed order!

// ✅ FIX 1: Always lock in the same order
void threadA() { std::lock_guard l1(m1); std::lock_guard l2(m2); }
void threadB() { std::lock_guard l1(m1); std::lock_guard l2(m2); }  // Same order

// ✅ FIX 2: std::scoped_lock (C++17) — locks both atomically
void safe() {
    std::scoped_lock lock(m1, m2);    // Deadlock-free, order doesn't matter
}
```

**Four conditions for deadlock** (all must be true):
1. **Mutual exclusion** — resource can't be shared
2. **Hold and wait** — holding one resource while waiting for another
3. **No preemption** — can't forcibly take a lock
4. **Circular wait** — A waits for B, B waits for A

Break **any one** condition → no deadlock.

---

### Q28. What is the difference between `mutex`, `semaphore`, and `spinlock`?

**Answer:**

| Mechanism | Behavior | Best For |
|-----------|----------|----------|
| **Mutex** | Binary lock. Thread sleeps if locked. | Protecting shared data |
| **Semaphore** | Counting lock. Allows N concurrent accesses. | Resource pools, producer-consumer |
| **Spinlock** | Busy-waits (loops) instead of sleeping. | Very short critical sections, no context switch |

```cpp
// Mutex — thread sleeps while waiting
std::mutex mtx;
mtx.lock();      // Blocks (sleeps) if already locked
// critical section
mtx.unlock();

// Semaphore (C++20)
std::counting_semaphore<3> sem(3);   // Allow 3 concurrent accesses
sem.acquire();     // Decrement (blocks if 0)
// access resource
sem.release();     // Increment

// Spinlock (manual — no std:: version)
std::atomic_flag lock = ATOMIC_FLAG_INIT;
while (lock.test_and_set(std::memory_order_acquire))
    ;    // Busy-wait (spin)
// critical section
lock.clear(std::memory_order_release);
```

**Qualcomm context:** Spinlocks are used in kernel/driver code where the critical section is just a few instructions and context switching is more expensive than spinning.

---

### Q29. Explain `std::condition_variable` usage.

**Answer:**

```cpp
std::mutex mtx;
std::condition_variable cv;
std::queue<int> dataQueue;
bool done = false;

// PRODUCER
void producer() {
    for (int i = 0; i < 5; i++) {
        {
            std::lock_guard lock(mtx);
            dataQueue.push(i);
        }
        cv.notify_one();    // Wake one waiting consumer
    }
    {
        std::lock_guard lock(mtx);
        done = true;
    }
    cv.notify_all();         // Wake all consumers to check 'done'
}

// CONSUMER
void consumer() {
    while (true) {
        std::unique_lock lock(mtx);
        cv.wait(lock, [&] { return !dataQueue.empty() || done; });

        if (dataQueue.empty() && done) break;

        int val = dataQueue.front();
        dataQueue.pop();
        lock.unlock();

        std::cout << "Consumed: " << val << "\n";
    }
}
```

**How `cv.wait()` works:**
1. Atomically **unlocks** the mutex and **sleeps**
2. When notified, **wakes up** and **re-locks** the mutex
3. Checks the predicate — if false, sleeps again (**spurious wakeup protection**)

---

## 🔹 8. Embedded & Low-Level

---

### Q30. What is memory-mapped I/O?

**Answer:**

In embedded systems, hardware peripherals (UART, GPIO, SPI) are controlled by reading/writing to specific memory addresses. This is **memory-mapped I/O**.

```c
// GPIO register at physical address 0x40020014
#define GPIOB_ODR (*(volatile uint32_t *)0x40020014)

// Turn on LED connected to pin 5
GPIOB_ODR |= (1 << 5);     // Set bit 5

// Turn off LED
GPIOB_ODR &= ~(1 << 5);    // Clear bit 5

// Better approach — use a register struct
typedef struct {
    volatile uint32_t MODER;     // 0x00
    volatile uint32_t OTYPER;    // 0x04
    volatile uint32_t OSPEEDR;   // 0x08
    volatile uint32_t PUPDR;     // 0x0C
    volatile uint32_t IDR;       // 0x10
    volatile uint32_t ODR;       // 0x14
} GPIO_TypeDef;

#define GPIOB ((GPIO_TypeDef *)0x40020000)
GPIOB->ODR |= (1 << 5);
```

> `volatile` is **mandatory** — without it, the compiler may optimize away reads/writes to hardware registers.

---

### Q31. What is endianness? How do you handle it?

**Answer:**

```
Value: 0x12345678

Big-Endian (network byte order):
Address:  0x00  0x01  0x02  0x03
Data:     0x12  0x34  0x56  0x78    (MSB first)

Little-Endian (x86, most ARM):
Address:  0x00  0x01  0x02  0x03
Data:     0x78  0x56  0x34  0x12    (LSB first)
```

```c
// Check endianness at runtime
int checkEndian() {
    uint32_t x = 1;
    return *(uint8_t *)&x;    // 1 = little-endian, 0 = big-endian
}

// Convert between byte orders
#include <arpa/inet.h>
uint32_t network = htonl(0x12345678);    // Host to Network (big-endian)
uint32_t host    = ntohl(network);        // Network to Host

// Manual byte swap
uint32_t swap32(uint32_t x) {
    return ((x >> 24) & 0xFF)
         | ((x >>  8) & 0xFF00)
         | ((x <<  8) & 0xFF0000)
         | ((x << 24) & 0xFF000000);
}

// GCC built-in
uint32_t swapped = __builtin_bswap32(0x12345678);
```

**Qualcomm context:** Snapdragon processors are little-endian, but network protocols use big-endian. Byte-order conversion is essential in modem/connectivity code.

---

### Q32. What is DMA? How does it relate to software?

**Answer:**

**DMA (Direct Memory Access)** allows hardware peripherals to transfer data directly to/from memory **without CPU involvement**.

```
WITHOUT DMA:                          WITH DMA:
CPU reads byte from peripheral        CPU sets up DMA controller:
CPU writes byte to memory              - Source address
CPU reads next byte...                  - Destination address
(CPU is 100% busy!)                     - Transfer size
                                      DMA controller does the transfer
                                      CPU is FREE to do other work
                                      DMA interrupts CPU when done
```

**Software responsibilities:**

```c
// 1. Configure DMA
dma_config.src_addr  = UART_DATA_REG;
dma_config.dst_addr  = (uint32_t)buffer;
dma_config.size      = 1024;
dma_config.direction = PERIPH_TO_MEM;

// 2. Ensure cache coherency
// CPU cache may hold stale data after DMA writes to memory
cache_invalidate(buffer, 1024);   // Invalidate BEFORE reading DMA data
cache_flush(buffer, 1024);        // Flush BEFORE DMA reads from memory

// 3. Handle DMA completion interrupt
void dma_complete_isr(void) {
    // Data is now in buffer
    process_data(buffer, 1024);
}
```

---

### Q33. What is an interrupt? What is an ISR?

**Answer:**

An **interrupt** is a hardware signal that tells the CPU to stop what it's doing and run a specific function — the **Interrupt Service Routine (ISR)**.

```c
// ISR for a timer interrupt
void __attribute__((interrupt)) Timer_ISR(void) {
    // Keep it SHORT — minimal work
    timer_flag = 1;                    // Set a flag
    clear_interrupt_pending();         // Acknowledge the interrupt
    // DO NOT: printf, malloc, sleep, or call long functions
}

// Main loop checks the flag
int main() {
    while (1) {
        if (timer_flag) {
            timer_flag = 0;
            do_heavy_processing();     // Long work happens HERE, not in ISR
        }
    }
}
```

**ISR rules:**
1. Keep it **short** — microseconds, not milliseconds
2. **No blocking** calls (malloc, printf, mutex lock)
3. Use `volatile` for variables shared between ISR and main code
4. **Acknowledge** the interrupt to prevent retriggering

---

## 🔹 9. RTOS & Real-Time Concepts

---

### Q34. What is an RTOS? How is it different from Linux?

**Answer:**

| Feature | RTOS (FreeRTOS, QNX) | General OS (Linux) |
|---------|----------------------|-------------------|
| Scheduling | **Deterministic** — guaranteed response time | Best-effort |
| Latency | Microseconds | Milliseconds (variable) |
| Priority | Strict priority — highest priority task ALWAYS runs | Time-sharing |
| Size | Kilobytes to megabytes | Hundreds of MB+ |
| Use case | Safety-critical, real-time control | Servers, desktops |

**Qualcomm context:** QNX is used in automotive (ADAS, digital cockpit). FreeRTOS runs on Qualcomm's MCU subsystems. Linux runs on the application processor.

---

### Q35. What is priority inversion? How is it solved?

**Answer:**

```
Priority Inversion:
1. Low-priority task (L) acquires mutex
2. High-priority task (H) tries to acquire the SAME mutex — blocks
3. Medium-priority task (M) preempts L and runs
4. H is waiting for L, but M is preventing L from running!
→ H (highest priority) is effectively blocked by M (medium priority)
```

**Solutions:**

| Solution | How It Works |
|----------|-------------|
| **Priority Inheritance** | When H blocks on L's mutex, L temporarily gets H's priority |
| **Priority Ceiling** | Mutex has a ceiling priority. Any task that acquires it runs at that priority |

```
With Priority Inheritance:
1. L acquires mutex
2. H blocks on mutex → L's priority is BOOSTED to H's level
3. M can't preempt L anymore (L runs at H's priority)
4. L finishes, releases mutex → priority drops back
5. H runs immediately
```

> This was famously encountered on the **Mars Pathfinder** rover in 1997, where priority inversion caused system resets.

---

### Q36. What are the different RTOS task states?

**Answer:**

```
                    ┌──────────┐
         create →   │  Ready   │ ←── preempted
                    └────┬─────┘
                         │ scheduler picks
                         ↓
                    ┌──────────┐
                    │ Running  │
                    └──┬───┬───┘
            wait/yield │   │ terminate
                       ↓   ↓
                 ┌─────────┐ ┌────────────┐
                 │ Blocked │ │ Terminated │
                 └────┬────┘ └────────────┘
                      │ event/timeout
                      ↓
                 ┌──────────┐
                 │  Ready   │
                 └──────────┘
```

| State | Description |
|-------|-------------|
| **Ready** | Can run, waiting for CPU (scheduler hasn't picked it) |
| **Running** | Currently executing on the CPU |
| **Blocked** | Waiting for an event (mutex, semaphore, timer, I/O) |
| **Suspended** | Explicitly paused by another task or itself |
| **Terminated** | Finished execution |

---

## 🔹 10. Data Structures & Algorithms

---

### Q37. Implement a linked list reversal.

**Answer:**

```cpp
struct Node {
    int data;
    Node* next;
};

Node* reverse(Node* head) {
    Node* prev = nullptr;
    Node* curr = head;

    while (curr) {
        Node* next = curr->next;   // Save next
        curr->next = prev;         // Reverse the link
        prev = curr;               // Move prev forward
        curr = next;               // Move curr forward
    }
    return prev;    // prev is the new head
}
```

**Walkthrough:**

```
Input:  1 → 2 → 3 → nullptr

Step 1: prev=null, curr=1     1→null, prev=1, curr=2
Step 2: prev=1,    curr=2     2→1,    prev=2, curr=3
Step 3: prev=2,    curr=3     3→2,    prev=3, curr=null

Output: 3 → 2 → 1 → nullptr
```

---

### Q38. Detect a cycle in a linked list.

**Answer:**

**Floyd's Tortoise and Hare algorithm:**

```cpp
bool hasCycle(Node* head) {
    Node* slow = head;
    Node* fast = head;

    while (fast && fast->next) {
        slow = slow->next;           // 1 step
        fast = fast->next->next;     // 2 steps

        if (slow == fast) return true;    // They met → cycle exists
    }
    return false;    // fast reached end → no cycle
}

// Find the start of the cycle
Node* findCycleStart(Node* head) {
    Node* slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }

    if (!fast || !fast->next) return nullptr;    // No cycle

    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;    // Both move 1 step now
    }
    return slow;    // Meeting point = cycle start
}
```

---

### Q39. Implement binary search.

**Answer:**

```cpp
int binarySearch(const std::vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;    // Avoid integer overflow

        if (arr[mid] == target)      return mid;
        else if (arr[mid] < target)  left = mid + 1;
        else                         right = mid - 1;
    }
    return -1;    // Not found
}
```

**Time: O(log n), Space: O(1)**

> 💡 Use `left + (right - left) / 2` instead of `(left + right) / 2` to prevent integer overflow when `left + right > INT_MAX`.

---

### Q40. What is the time complexity of common operations?

**Answer:**

| Data Structure | Access | Search | Insert | Delete |
|----------------|:------:|:------:|:------:|:------:|
| Array | O(1) | O(n) | O(n) | O(n) |
| Linked List | O(n) | O(n) | O(1)* | O(1)* |
| Hash Table | N/A | O(1) avg | O(1) avg | O(1) avg |
| BST (balanced) | O(log n) | O(log n) | O(log n) | O(log n) |
| Stack / Queue | O(n) | O(n) | O(1) | O(1) |
| Heap | O(n) | O(n) | O(log n) | O(log n) |

\* With pointer to position. Otherwise O(n) to find the position first.

| Sorting Algorithm | Best | Average | Worst | Space | Stable? |
|-------------------|:----:|:-------:|:-----:|:-----:|:-------:|
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) | ✅ |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n) | ❌ |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) | ✅ |
| Heap Sort | O(n log n) | O(n log n) | O(n log n) | O(1) | ❌ |

---

## 🔹 11. Networking & Protocols

---

### Q41. Explain TCP vs UDP.

**Answer:**

| Feature | TCP | UDP |
|---------|-----|-----|
| Connection | Connection-oriented (handshake) | Connectionless |
| Reliability | Guaranteed delivery, ordering | No guarantee |
| Flow control | Yes (sliding window) | No |
| Speed | Slower (overhead) | Faster (minimal overhead) |
| Header size | 20-60 bytes | 8 bytes |
| Use case | HTTP, file transfer, SSH | Video streaming, DNS, gaming, VoIP |

**TCP 3-way handshake:**

```
Client              Server
  │── SYN ──────────→│     1. Client requests connection
  │←── SYN + ACK ────│     2. Server acknowledges
  │── ACK ──────────→│     3. Client confirms
  │←─── DATA ────────│     Connection established
```

**Qualcomm context:** Qualcomm's modem firmware handles TCP/IP stack at the hardware level. Understanding protocol headers is essential for modem development.

---

### Q42. What is the OSI model?

**Answer:**

```
Layer 7: Application    │ HTTP, FTP, DNS, SMTP           │ Data
Layer 6: Presentation   │ SSL/TLS, JPEG, encryption      │ Data
Layer 5: Session        │ Session management, RPC         │ Data
Layer 4: Transport      │ TCP, UDP                        │ Segments
Layer 3: Network        │ IP, ICMP, routing               │ Packets
Layer 2: Data Link      │ Ethernet, Wi-Fi (MAC)           │ Frames
Layer 1: Physical       │ Cables, signals, radio waves    │ Bits
```

> **Mnemonic:** "**A**ll **P**eople **S**eem **T**o **N**eed **D**ata **P**rocessing" (top to bottom)

---

## 🔹 12. Linux / QNX Internals

---

### Q43. What is the difference between process and thread?

**Answer:**

| Feature | Process | Thread |
|---------|---------|--------|
| Memory | Own address space | Share parent process's memory |
| Creation cost | Heavy (fork + exec) | Light (just stack + registers) |
| Communication | IPC (pipes, sockets, shared mem) | Direct memory access |
| Crash impact | Other processes unaffected | Crash kills ALL threads in process |
| Context switch | Expensive (TLB flush) | Cheaper (same address space) |

```
Process A                    Process B
┌─────────────────┐          ┌─────────────────┐
│ Code  │ Data    │          │ Code  │ Data    │
│ Stack │ Heap    │          │ Stack │ Heap    │
│ Thread1 Thread2 │          │ Thread1          │
└─────────────────┘          └─────────────────┘
  Separate memory              Separate memory
```

---

### Q44. What is IPC? List common mechanisms.

**Answer:**

**Inter-Process Communication (IPC)** — ways for processes to exchange data:

| Mechanism | Description | Speed | Complexity |
|-----------|-------------|:-----:|:----------:|
| **Pipe** | Unidirectional byte stream (parent ↔ child) | Medium | Low |
| **Named Pipe (FIFO)** | Bidirectional, any processes | Medium | Low |
| **Shared Memory** | Both processes map the same memory region | **Fastest** | Medium |
| **Message Queue** | Kernel-managed message queue | Medium | Medium |
| **Socket** | TCP/UDP — works across network too | Slower | Medium |
| **Signal** | Asynchronous notification (SIGTERM, SIGUSR1) | Fast | Low |
| **Semaphore** | Synchronization between processes | N/A | Low |

```c
// Shared memory example (POSIX)
int fd = shm_open("/my_shm", O_CREAT | O_RDWR, 0666);
ftruncate(fd, 4096);
void* ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

sprintf((char*)ptr, "Hello from Process A");
// Process B can mmap the same "/my_shm" and read the message
```

**QNX-specific:** QNX uses **message passing** as its primary IPC — `MsgSend()`, `MsgReceive()`, `MsgReply()`. This is a core part of QNX's microkernel architecture.

---

### Q45. What happens when you type `./program` in Linux?

**Answer:**

```
1. Shell calls fork()           → Creates child process (copy of shell)
2. Child calls execve("./program") → Replaces shell code with program code
3. Kernel loads ELF binary:
    a. Reads ELF header
    b. Maps .text, .data, .bss segments into virtual memory
    c. Sets up stack
    d. Loads dynamic libraries (ld-linux.so)
    e. Resolves symbols (printf → libc address)
4. Jumps to _start (C runtime)
5. _start calls __libc_start_main
6. __libc_start_main calls main()
7. main() returns → exit() → kernel reclaims resources
```

---

## 🔹 13. Debugging & Tools

---

### Q46. How would you debug a segmentation fault?

**Answer:**

```bash
# 1. Compile with debug symbols
gcc -g -fsanitize=address program.c -o program

# 2. Run with AddressSanitizer (instant crash report)
./program
# Output shows exact file:line:column of the bug

# 3. Or use GDB
gdb ./program
(gdb) run
# Program crashes
(gdb) bt              # Backtrace — shows call stack
(gdb) frame 3         # Go to frame 3
(gdb) print variable  # Inspect variables
(gdb) list            # Show source code around crash

# 4. Analyze core dump
ulimit -c unlimited            # Enable core dumps
./program                       # Crashes → generates core file
gdb ./program core              # Analyze
(gdb) bt                        # See where it crashed
```

**Common segfault causes:**
- Null pointer dereference
- Use after free (dangling pointer)
- Stack overflow (infinite recursion)
- Buffer overflow
- Accessing unmapped memory

---

### Q47. What build system tools do you know?

**Answer:**

| Tool | Type | Used For |
|------|------|----------|
| **Make** | Build automation | C/C++ projects (Makefile) |
| **CMake** | Meta-build system | Generates Makefiles/Ninja/VS projects |
| **Bazel** | Build system | Large-scale monorepos (Google, Qualcomm) |
| **Ninja** | Build executor | Fast, used as CMake backend |
| **GCC/Clang** | Compiler | C/C++ compilation |
| **GDB/LLDB** | Debugger | Runtime debugging |
| **Valgrind** | Memory analyzer | Leak detection, memory errors |
| **Coverity** | Static analysis | Bug finding without running code |
| **Clang-Tidy** | Linter | Code quality checks |
| **perf** | Profiler | Performance analysis on Linux |

```bash
# CMake typical workflow
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
ctest

# Bazel typical workflow
bazel build //path/to:target
bazel test //path/to:target_test
```

---

## 🔹 14. Design Patterns

---

### Q48. Explain Singleton, Observer, and Factory patterns.

**Answer:**

**Singleton** — exactly one instance:

```cpp
class Config {
public:
    static Config& instance() {
        static Config cfg;      // Thread-safe since C++11
        return cfg;
    }
    std::string get(const std::string& key) const;

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
private:
    Config() { /* load config file */ }
};
```

**Observer** — notify many listeners when something changes:

```cpp
class Button {
    std::vector<std::function<void()>> listeners_;
public:
    void onClick(std::function<void()> cb) {
        listeners_.push_back(std::move(cb));
    }
    void click() {
        for (auto& l : listeners_) l();   // Notify all
    }
};
```

**Factory** — create objects without knowing concrete types:

```cpp
std::unique_ptr<Sensor> createSensor(const std::string& type) {
    if (type == "temperature") return std::make_unique<TempSensor>();
    if (type == "pressure")    return std::make_unique<PressureSensor>();
    if (type == "accelerometer") return std::make_unique<AccelSensor>();
    return nullptr;
}
```

---

### Q49. What is the SOLID principle?

**Answer:**

| Letter | Principle | Meaning |
|:------:|-----------|---------|
| **S** | Single Responsibility | A class should have ONE reason to change |
| **O** | Open/Closed | Open for extension, closed for modification |
| **L** | Liskov Substitution | Subclass must be usable wherever base class is expected |
| **I** | Interface Segregation | Many small interfaces > one fat interface |
| **D** | Dependency Inversion | Depend on abstractions, not concrete classes |

```cpp
// ❌ Violates SRP — class does TWO things
class Report {
    void generateData();       // Business logic
    void printToPDF();         // Presentation logic
};

// ✅ SRP — separate responsibilities
class ReportGenerator { void generateData(); };
class PDFPrinter       { void print(const Report&); };

// ✅ Dependency Inversion
class Logger {
    IOutputStream& out_;       // Depends on abstraction
public:
    Logger(IOutputStream& out) : out_(out) {}
    void log(const std::string& msg) { out_.write(msg); }
};
// Can inject FileStream, ConsoleStream, NetworkStream — Logger doesn't care
```

---

## 🔹 15. Coding Problems (Whiteboard)

---

### Q50. Reverse a string in place.

```cpp
void reverseString(char* s, int len) {
    int left = 0, right = len - 1;
    while (left < right) {
        std::swap(s[left], s[right]);
        left++;
        right--;
    }
}
// "hello" → "olleh"
```

---

### Q51. Check if a string is a palindrome.

```cpp
bool isPalindrome(const std::string& s) {
    int l = 0, r = s.size() - 1;
    while (l < r) {
        if (s[l] != s[r]) return false;
        l++; r--;
    }
    return true;
}
// "racecar" → true, "hello" → false
```

---

### Q52. Find the first non-repeating character.

```cpp
char firstUnique(const std::string& s) {
    int freq[256] = {0};
    for (char c : s) freq[(unsigned char)c]++;
    for (char c : s) if (freq[(unsigned char)c] == 1) return c;
    return '\0';
}
// "aabbcdd" → 'c'
```

---

### Q53. Implement `atoi` (string to integer).

```cpp
int myAtoi(const std::string& s) {
    int i = 0, sign = 1;
    long result = 0;

    // Skip whitespace
    while (i < s.size() && s[i] == ' ') i++;

    // Handle sign
    if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
        sign = (s[i] == '-') ? -1 : 1;
        i++;
    }

    // Convert digits
    while (i < s.size() && isdigit(s[i])) {
        result = result * 10 + (s[i] - '0');
        if (result * sign > INT_MAX) return INT_MAX;
        if (result * sign < INT_MIN) return INT_MIN;
        i++;
    }

    return (int)(result * sign);
}
// "  -42abc" → -42
```

---

### Q54. Find two numbers in an array that sum to a target.

```cpp
std::pair<int,int> twoSum(const std::vector<int>& nums, int target) {
    std::unordered_map<int,int> seen;    // value → index

    for (int i = 0; i < nums.size(); i++) {
        int complement = target - nums[i];
        if (seen.count(complement))
            return {seen[complement], i};
        seen[nums[i]] = i;
    }
    return {-1, -1};
}
// nums = {2, 7, 11, 15}, target = 9 → {0, 1}
```

---

### Q55. Implement a circular buffer (ring buffer).

**Qualcomm favorite** — used in audio pipelines, DMA buffers, log systems.

```cpp
template<typename T, size_t N>
class RingBuffer {
    T buffer_[N];
    size_t head_ = 0, tail_ = 0;
    bool full_ = false;

public:
    bool push(const T& item) {
        if (full_) return false;
        buffer_[head_] = item;
        head_ = (head_ + 1) % N;
        full_ = (head_ == tail_);
        return true;
    }

    bool pop(T& item) {
        if (empty()) return false;
        item = buffer_[tail_];
        tail_ = (tail_ + 1) % N;
        full_ = false;
        return true;
    }

    bool empty() const { return (!full_ && head_ == tail_); }
    bool isFull() const { return full_; }
    size_t size() const {
        if (full_) return N;
        return (head_ >= tail_) ? (head_ - tail_) : (N - tail_ + head_);
    }
};
```

```
Ring Buffer (N=5):
     tail             head
      ↓                ↓
  ┌───┬───┬───┬───┬───┐
  │ A │ B │ C │   │   │
  └───┴───┴───┴───┴───┘
   [0]  [1]  [2]  [3]  [4]

After push(D), push(E), pop():
          tail         head
           ↓            ↓
  ┌───┬───┬───┬───┬───┐
  │   │ B │ C │ D │ E │
  └───┴───┴───┴───┴───┘
  head wraps around when it reaches the end
```

---

## 🔹 16. Behavioral / System Design

---

### Q56. How would you design a logging system for an embedded device?

**Answer:**

**Requirements:** Low memory, minimal CPU overhead, thread-safe, non-blocking, different severity levels.

```
Architecture:
┌──────────┐    push()    ┌──────────────┐    flush()    ┌──────────┐
│ Thread 1 │────────────→ │  Lock-free   │──────────────→│  Output  │
│ Thread 2 │────────────→ │  Ring Buffer │               │ (UART/   │
│   ISR    │────────────→ │  (4 KB)      │               │  File/   │
└──────────┘              └──────────────┘               │  Network)│
                                                          └──────────┘
```

**Key decisions:**
- **Ring buffer** — fixed size, no malloc, overwrites oldest entries on overflow
- **Lock-free** for ISR context (can't use mutex in interrupts)
- **Deferred formatting** — store raw data, format during flush (not in hot path)
- **Severity levels** — `FATAL`, `ERROR`, `WARN`, `INFO`, `DEBUG`, `TRACE`
- **Compile-time filtering** — strip `DEBUG`/`TRACE` from release builds

---

### Q57. Tell me about a challenging bug you fixed.

**Framework answer:**

```
SITUATION: What was the project and context?
  → "Working on ADAS camera pipeline, intermittent frame drops..."

TASK: What was your responsibility?
  → "I was responsible for the real-time video processing module..."

ACTION: What did you do to diagnose and fix it?
  → "Used perf to profile, found cache thrashing in the DMA buffer..."
  → "Aligned buffers to cache line boundaries, used double-buffering..."

RESULT: What was the outcome?
  → "Frame drops eliminated, latency reduced from 12ms to 3ms..."
```

---

### Q58. How do you optimize code for embedded systems?

**Answer:**

| Area | Technique |
|------|-----------|
| **Memory** | Avoid dynamic allocation (use pools, stack, static) |
| **CPU** | Minimize branching, use lookup tables, bitwise ops |
| **Cache** | Data locality, structure packing, avoid pointer chasing |
| **I/O** | Use DMA, batch transfers, reduce interrupts |
| **Compiler** | `-O2`, `-Os` (size), LTO, PGO, `__builtin_expect()` |
| **Algorithm** | Choose O(1)/O(log n) over O(n), pre-compute constants |
| **Power** | Sleep modes, clock gating, reduce wake-ups |

```c
// Branch prediction hint
if (__builtin_expect(error_condition, 0)) {   // Unlikely path
    handle_error();
}

// Cache-friendly iteration
// ✅ Row-major access (sequential in memory)
for (int i = 0; i < N; i++)
    for (int j = 0; j < M; j++)
        matrix[i][j] = 0;

// ❌ Column-major access (cache miss every access)
for (int j = 0; j < M; j++)
    for (int i = 0; i < N; i++)
        matrix[i][j] = 0;
```

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 Qualcomm Interview Preparation — Complete</em><br/>
  <em>58 Questions covering C, C++, Embedded, RTOS, DS&A, and System Design.</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
