<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 1</samp>
</p>

<h1 align="center">📘 C++ Basics — The Building Blocks</h1>

<p align="center"><em>Every skyscraper starts with a single brick.<br/>Every C++ program starts here.</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Your First Program — Hello World | [§1](#-1-your-first-program--hello-world) |
| 2 | Variables & Data Types | [§2](#-2-variables--data-types) |
| 3 | Operators | [§3](#-3-operators) |
| 4 | Control Flow — Decisions & Loops | [§4](#-4-control-flow--decisions--loops) |
| 5 | Functions | [§5](#-5-functions) |
| 6 | Arrays | [§6](#-6-arrays) |
| 7 | Pointers | [§7](#-7-pointers) |
| 8 | References | [§8](#-8-references) |
| 9 | Strings | [§9](#-9-strings) |
| 10 | Input / Output | [§10](#-10-input--output) |

> **How to compile any example:**
> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. Your First Program — Hello World

> *📄 See full code → `01_hello_world.cpp`*

### The Concept

Every C++ program must have **exactly one** function called `main()`. This is the **entry point** — the first thing the operating system runs when you launch your program. Think of it as the front door of a building: no matter how many rooms exist inside, everyone enters through `main()`.

To display text on the screen, we use `std::cout` (short for "**c**haracter **out**put"). The `<<` operator sends data into this output stream — imagine it as pushing a letter into a mailbox.

### Example

```cpp
#include <iostream>   // ← Import the I/O toolbox

int main() {
    std::cout << "Hello, World!" << std::endl;
    std::cout << "I am " << 25 << " years old." << '\n';
    return 0;       // ← Tell the OS: "Everything went fine."
}
```

### How This Works — Line by Line

| Line | What it does |
|------|-------------|
| `#include <iostream>` | Loads the Input/Output library so we can use `cout`. Think of it as importing a toolbox before starting work. |
| `int main()` | Declares the entry point. `int` means it returns a number. Zero means success. |
| `std::cout << "Hello, World!"` | Sends the text `"Hello, World!"` to the screen. `<<` is the insertion operator. |
| `std::endl` | Inserts a new line **and** flushes the output buffer (forces text to appear immediately). |
| `'\n'` | Also inserts a new line, but faster — it doesn't flush. Prefer this in most cases. |
| `return 0;` | Signals the OS that the program finished successfully. |

### 🖥️ Output

```
Hello, World!
I am 25 years old.
```

> 💡 **Tip:** Avoid `using namespace std;` in real projects. It dumps thousands of names into your scope and can cause conflicts. Always write `std::cout`, `std::endl`, etc.

---

## 🔹 2. Variables & Data Types

> *📄 See full code → `02_variables_and_datatypes.cpp`*

### The Concept

A **variable** is a named container that holds data. A **data type** tells the compiler what *kind* of data the container holds and *how much memory* to reserve.

Think of it like a kitchen with different-sized jars:

| Jar (Type) | Holds | Size |
|:----------:|-------|:----:|
| `bool` | `true` / `false` (a light switch) | 1 byte |
| `char` | A single letter: `'A'`, `'7'`, `'$'` | 1 byte |
| `int` | Whole numbers: `-5`, `0`, `42` | 4 bytes |
| `double` | Decimal numbers: `3.14`, `-0.001` | 8 bytes |
| `std::string` | Text: `"Hello World"` | varies |

You can't fit a sofa into a shoebox — similarly, a data type defines what fits inside the variable.

### Example

```cpp
#include <iostream>
#include <string>

int main() {
    // ── Different ways to initialize ──
    int age = 25;                  // Copy initialization
    int height(180);               // Direct initialization
    int weight{75};                // Brace initialization (C++11, PREFERRED)

    double pi = 3.14159265;
    char grade = 'A';
    bool passed = true;
    std::string name = "Alice";

    // ── The compiler can figure out the type for you ──
    auto city = std::string("Berlin");   // auto → std::string
    auto count = 42;                     // auto → int

    // ── Check sizes ──
    std::cout << "int:    " << sizeof(int)    << " bytes\n";
    std::cout << "double: " << sizeof(double) << " bytes\n";
    std::cout << "char:   " << sizeof(char)   << " byte\n";

    // ── Type casting ──
    double result = 17.0 / 5;              // 3.4  (float division)
    int truncated = static_cast<int>(3.7); // 3  (decimal chopped off!)

    std::cout << name << " is " << age << ", grade " << grade << '\n';
}
```

### How This Works

**Brace initialization `{}`** is the preferred modern style because it catches dangerous mistakes at compile time:

```cpp
int x{3.14};    // ❌ COMPILER ERROR: losing decimal data
int x = 3.14;   // ⚠️ Silently truncates to 3 — a hidden bug!
```

**`auto`** tells the compiler: *"Look at the right side and figure the type out yourself."* It's great for long types like `std::map<std::string, std::vector<int>>::iterator`.

**`sizeof()`** returns how many bytes a type occupies. On a 64-bit system: `int` = 4, `double` = 8, `char` = 1.

**`static_cast<int>(3.7)`** explicitly converts `3.7` to `3`. The decimal part is *truncated* (chopped off), **not rounded**.

### 🖥️ Output

```
int:    4 bytes
double: 8 bytes
char:   1 byte
Alice is 25, grade A
```

---

## 🔹 3. Operators

> *📄 See full code → `03_operators.cpp`*

### The Concept

Operators are the **verbs** of programming — they *do things* to data. Just like `+` adds numbers in math, C++ has operators for arithmetic, comparison, logic, and more.

### 3a. Arithmetic Operators

```cpp
int a = 17, b = 5;

std::cout << a + b  << '\n';   // 22   — Addition
std::cout << a - b  << '\n';   // 12   — Subtraction
std::cout << a * b  << '\n';   // 85   — Multiplication
std::cout << a / b  << '\n';   // 3    — Integer division (truncates!)
std::cout << a % b  << '\n';   // 2    — Modulus (remainder)

std::cout << 17.0 / 5 << '\n'; // 3.4  — Float division
```

> ⚠️ **Watch out:** `17 / 5` = **3**, not 3.4! When both sides are integers, C++ does integer division and chops the decimal. To get 3.4, make at least one operand a `double`: `17.0 / 5`.

### 3b. Increment / Decrement

```cpp
int x = 10;
std::cout << ++x << '\n';   // 11 — Increment FIRST, then use
std::cout << x++ << '\n';   // 11 — Use FIRST, then increment
std::cout << x   << '\n';   // 12 — Now x has been incremented
```

| Expression | Returns | `x` after |
|:----------:|:-------:|:---------:|
| `++x` (pre) | 11 | 11 |
| `x++` (post) | 11 | 12 |

### 3c. Comparison & Logical Operators

```cpp
int a = 10, b = 20;

// ── Comparison (returns true or false) ──
a == b     // false  — is equal?     (Double == !)
a != b     // true   — not equal?
a < b      // true   — less than?

// ── Logical (combine conditions) ──
(a > 5) && (b > 15)    // true  — AND: both must be true
(a > 15) || (b > 15)   // true  — OR:  at least one true
!(a > 15)              // true  — NOT: flips true ↔ false
```

### 3d. The Ternary Operator — One-Line `if/else`

```cpp
int age = 20;
std::string status = (age >= 18) ? "Adult" : "Minor";
// Reads as: "If age >= 18 → Adult, otherwise → Minor"
```

---

## 🔹 4. Control Flow — Decisions & Loops

> *📄 See full code → `04_control_flow.cpp`*

### The Concept

Without control flow, code runs top-to-bottom like a grocery list. Control flow adds **forks in the road** (decisions) and **roundabouts** (loops) so your program can make choices and repeat tasks.

### 4a. `if / else` — Making Decisions

```cpp
int temperature = 35;

if (temperature > 40) {
    std::cout << "Extremely hot!\n";
} else if (temperature > 30) {
    std::cout << "It's hot!\n";        // ← This runs (35 > 30)
} else if (temperature > 20) {
    std::cout << "It's warm.\n";
} else {
    std::cout << "It's cold.\n";
}
```

The program checks conditions **top to bottom**. The first `true` condition runs; the rest are skipped. `else` is the catch-all if nothing matched.

### 4b. `switch` — Multiple Choices

```cpp
int day = 3;

switch (day) {
    case 1: std::cout << "Monday\n";    break;
    case 2: std::cout << "Tuesday\n";   break;
    case 3: std::cout << "Wednesday\n"; break;   // ← This runs
    case 6:
    case 7: std::cout << "Weekend!\n";  break;   // 6 OR 7
    default: std::cout << "Invalid\n";
}
```

> ⚠️ **Don't forget `break`!** Without it, execution "falls through" to the next case.

### 4c. `for` Loop — "Do This N Times"

```cpp
for (int i = 0; i < 5; i++) {
    std::cout << i << " ";
}
// Output: 0 1 2 3 4
```

| Part | Meaning |
|------|---------|
| `int i = 0` | **Start:** create a counter at 0 |
| `i < 5` | **Condition:** keep going while i < 5 |
| `i++` | **Step:** add 1 after each iteration |

### 4d. `while` Loop — "Keep Going Until…"

```cpp
int n = 128;
while (n > 1) {
    n /= 2;
    std::cout << n << " ";
}
// Output: 64 32 16 8 4 2 1
```

### 4e. Range-Based `for` — "For Each Item In…"

```cpp
std::vector<int> nums = {10, 20, 30, 40};

for (int val : nums) {
    std::cout << val << " ";
}
// Output: 10 20 30 40
```

This is the cleanest way to iterate over any collection. Read it as: *"for each `val` in `nums`, do…"*

---

## 🔹 5. Functions

> *📄 See full code → `05_functions.cpp`*

### The Concept

A function is a **reusable recipe**. You give it ingredients (parameters), it follows steps (the body), and hands back a result (return value). Instead of writing the same code 10 times, you write it once as a function and **call** it 10 times.

### 5a. Defining & Calling

```cpp
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(3, 4);              // result = 7
    std::cout << add(10, 20) << '\n';    // 30
}
```

### 5b. Pass by Value vs. Reference — ⭐ Critical Concept

```cpp
void byValue(int x) {
    x = x * 2;         // Changes a LOCAL COPY only!
}

void byReference(int& x) {
    x = x * 2;         // Changes the ORIGINAL variable!
}

int main() {
    int num = 10;

    byValue(num);
    std::cout << num << '\n';       // Still 10 — copy was changed

    byReference(num);
    std::cout << num << '\n';       // Now 20  — original was changed
}
```

| Method | Syntax | Effect | Best For |
|--------|:------:|--------|----------|
| **By value** | `f(int x)` | Gets a **copy** | Small types; no side effects |
| **By reference** | `f(int& x)` | Gets the **original** | Need to modify caller's variable |
| **By const ref** | `f(const string& s)` | Reads original, **can't modify** | Large objects; read-only |

### 5c. Default Parameters

```cpp
void greet(const std::string& name, const std::string& greeting = "Hello") {
    std::cout << greeting << ", " << name << "!\n";
}

greet("Alice");            // "Hello, Alice!"
greet("Bob", "Goodbye");   // "Goodbye, Bob!"
```

### 5d. Function Overloading — Same Name, Different Signatures

```cpp
int    add(int a, int b)       { return a + b; }
double add(double a, double b) { return a + b; }

add(3, 4);       // Calls int version → 7
add(3.14, 2.71); // Calls double version → 5.85
```

The compiler picks the right version based on argument types — like the word "open" meaning different things for a door vs. a file.

---

## 🔹 6. Arrays

> *📄 See full code → `06_arrays.cpp`*

### The Concept

An array is a **row of numbered lockers**, all the same size, sitting side by side in memory. Each locker is accessed by its **index** (starting from **0**).

```
  Index:     [0]    [1]    [2]    [3]    [4]
           ┌──────┬──────┬──────┬──────┬──────┐
  Values:  │  10  │  20  │  30  │  40  │  50  │
           └──────┴──────┴──────┴──────┴──────┘
  Address: 0x100  0x104  0x108  0x10C  0x110
           ← each int is 4 bytes apart →
```

### Example

```cpp
int main() {
    int scores[5] = {10, 20, 30, 40, 50};

    // Access by index
    std::cout << scores[0] << '\n';   // 10 (first)
    std::cout << scores[4] << '\n';   // 50 (last)

    // Modify
    scores[2] = 99;

    // Loop through
    for (int i = 0; i < 5; i++) {
        std::cout << scores[i] << " ";
    }
    // Output: 10 20 99 40 50

    // 2D Array (table/grid)
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };
    std::cout << matrix[1][2] << '\n';  // 6 (row 1, col 2)
}
```

### How This Works

- **Index starts at 0** because it represents the *offset* from the start. First element = 0 steps away.
- **Fixed size** — `int arr[5]` can never become size 6. For resizable arrays, use `std::vector` (Chapter 5).
- **`sizeof(arr) / sizeof(arr[0])`** gives the number of elements: total bytes / one element's bytes.

> ⚠️ **Danger:** Accessing `scores[5]` is **undefined behavior** — there is no 6th element. C++ won't warn you. Always check your bounds.

---

## 🔹 7. Pointers

> *📄 See full code → `07_pointers.cpp`*

### The Concept

A **pointer** is a variable that stores the **memory address** of another variable — not the value itself.

> If a variable is a **house**, a pointer is a **piece of paper with the house's address written on it**.

```
      Variable x               Pointer ptr
     ┌──────────┐            ┌──────────────┐
     │    42    │  ◄──────── │  0x7fff5a2c  │
     └──────────┘            └──────────────┘
     Address: 0x7fff5a2c     (stores x's address)

     ptr  == 0x7fff5a2c      (the address itself)
     *ptr == 42              (the value AT that address)
```

### Example

```cpp
int main() {
    int x = 42;
    int* ptr = &x;         // ptr holds the ADDRESS of x

    // Reading
    std::cout << x     << '\n';   // 42
    std::cout << &x    << '\n';   // 0x7fff...  (address of x)
    std::cout << ptr   << '\n';   // same address
    std::cout << *ptr  << '\n';   // 42  (follow the address → get the value)

    // Writing through the pointer
    *ptr = 100;
    std::cout << x << '\n';       // 100 — x changed!

    // Pointer arithmetic (walking through an array)
    int arr[] = {10, 20, 30, 40, 50};
    int* p = arr;
    std::cout << *(p + 0) << '\n';   // 10
    std::cout << *(p + 1) << '\n';   // 20
    std::cout << *(p + 2) << '\n';   // 30

    // Null pointer — points to nothing
    int* safe = nullptr;
    if (safe != nullptr) {
        std::cout << *safe << '\n';
    }
}
```

### Quick-Reference Table

| Symbol | In a Declaration | In an Expression |
|:------:|:----------------:|:----------------:|
| `*` | `int* ptr` → "pointer to int" | `*ptr` → "value at address" (dereference) |
| `&` | `int& ref` → "reference to int" | `&x` → "address of x" |

**Pointer arithmetic:** Adding 1 to an `int*` advances by `sizeof(int)` bytes (4), not 1 byte. So `p + 1` points to the **next element**, not the next byte.

> ⚠️ **Danger — Dangling Pointer:** If you `delete` memory then dereference the pointer, you're visiting a demolished house. Always set deleted pointers to `nullptr`.

---

## 🔹 8. References

> *📄 See full code → `08_references.cpp`*

### The Concept

A **reference** is an **alias** — a second name for an existing variable. If "Robert" has the nickname "Bob," calling either name reaches the **same person**.

```
    int x = 42;
    int& ref = x;      // ref IS x — same box, different label

    ┌──────────┐
    │    42    │  ← Both 'x' and 'ref' point here
    └──────────┘
```

### Example

```cpp
int main() {
    int x = 42;
    int& ref = x;         // ref is an alias for x

    ref = 100;             // Modify through alias
    std::cout << x << '\n';   // 100 — same memory!

    // const reference — read-only alias
    const int& cref = x;
    std::cout << cref << '\n';   // 100
    // cref = 200;               // ❌ ERROR: can't modify through const ref
}
```

### References vs. Pointers

| | Reference `int&` | Pointer `int*` |
|-|:-:|:-:|
| Can be null? | ❌ No | ✅ Yes |
| Can be reassigned? | ❌ Always same target | ✅ Yes |
| Syntax to use | Just `ref` | Need `*ptr` |
| Must initialize? | ✅ Yes | ❌ No |

**Rule of thumb:** Use references by default. Use pointers only when you need `nullptr` or reassignment.

---

## 🔹 9. Strings

> *📄 See full code → `09_strings.cpp`*

### The Concept

C++ has two kinds of strings:

| | C-string (`char[]`) | `std::string` |
|-|:---:|:---:|
| Analogy | Scrabble tiles on a fixed rack | A smart text editor |
| Resize? | ❌ Fixed | ✅ Automatic |
| Knows its length? | ❌ Must walk to `'\0'` | ✅ Instant `.size()` |
| Safe? | ❌ Buffer overflows | ✅ Safe by design |

**Always prefer `std::string`.**

### Example

```cpp
#include <iostream>
#include <string>

int main() {
    std::string greeting = "Hello";
    std::string name("World");

    // Concatenation
    std::string full = greeting + ", " + name + "!";
    std::cout << full << '\n';                // Hello, World!
    std::cout << full.length() << '\n';       // 13

    // Access characters
    std::cout << full[0] << '\n';             // 'H'
    std::cout << full.at(7) << '\n';          // 'W' (throws if out of bounds)

    // Searching
    size_t pos = full.find("World");
    if (pos != std::string::npos) {
        std::cout << "Found at position " << pos << '\n';   // 7
    }

    // Substring & conversion
    std::string sub = full.substr(0, 5);          // "Hello"
    std::string num = std::to_string(42);          // "42"
    int val = std::stoi("123");                    // 123

    // Iterate character-by-character
    for (char c : greeting) std::cout << c << ' ';
    // Output: H e l l o
}
```

### 🖥️ Output

```
Hello, World!
13
H
W
Found at position 7
H e l l o
```

---

## 🔹 10. Input / Output

> *📄 See full code → `10_input_output.cpp`*

### The Concept

I/O is how your program talks to the outside world:

| Stream | Direction | Analogy |
|--------|:---------:|---------|
| `std::cout` | → Screen | 📣 Megaphone |
| `std::cin` | ← Keyboard | 🎤 Microphone |
| `std::ofstream` | → File | ✏️ Writing to a notebook |
| `std::ifstream` | ← File | 📖 Reading from a book |

### Console I/O

```cpp
#include <iostream>
#include <iomanip>

int main() {
    // ── Formatted output ──
    double pi = 3.14159265;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Pi = " << pi << '\n';           // Pi = 3.14

    // ── Reading input ──
    int age;
    std::cout << "Enter age: ";
    std::cin >> age;                    // Reads one number

    std::cin.ignore();                  // Discard leftover '\n'
    std::string name;
    std::cout << "Enter full name: ";
    std::getline(std::cin, name);       // Reads entire line (with spaces)
}
```

### File I/O

```cpp
#include <fstream>
#include <string>

int main() {
    // ── Write ──
    std::ofstream out("data.txt");
    out << "Name: Alice\n";
    out << "Score: 95\n";
    out.close();

    // ── Read ──
    std::ifstream in("data.txt");
    std::string line;
    while (std::getline(in, line)) {
        std::cout << line << '\n';
    }
}
```

### How This Works

| Operation | What It Does |
|-----------|-------------|
| `cin >> age` | Reads until whitespace. `"John Doe"` → only captures `"John"` |
| `getline(cin, line)` | Reads the **entire line**, spaces included |
| `cin.ignore()` | Discards the leftover `'\n'` from a previous `cin >>` — without this, `getline` reads an empty string |
| `ofstream` creates/overwrites | `ifstream` reads |

> 💡 **Tip:** Files auto-close when the stream goes out of scope (RAII). But `.close()` makes intent explicit.

---

## 📚 The Big Picture

### Memory Layout of a Running Program

```
   High Address  ┌─────────────────────────────┐
                 │  STACK                       │ ← Local variables, function calls
                 │  (grows ↓ downward)          │   Fast, automatic, limited (~8MB)
                 ├─────────────────────────────┤
                 │         free space           │
                 ├─────────────────────────────┤
                 │  HEAP                        │ ← Dynamic memory (new / malloc)
                 │  (grows ↑ upward)            │   Must be freed manually
                 ├─────────────────────────────┤
                 │  Global / Static Data        │ ← Variables outside functions
                 ├─────────────────────────────┤
                 │  Code (Text Segment)         │ ← Compiled machine instructions
   Low Address   └─────────────────────────────┘
```

### The Compilation Pipeline

```
   Your Code (.cpp)
       │
       ▼
  ┌───────────────┐
  │  Preprocessor  │ → Expands #include, #define
  └───────┬───────┘
          ▼
  ┌───────────────┐
  │   Compiler     │ → C++ → Assembly → Machine code (.o)
  └───────┬───────┘
          ▼
  ┌───────────────┐
  │    Linker      │ → Combines .o files + libraries → Executable
  └───────┬───────┘
          ▼
      ./program      ← Run it!
```

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 2: Object-Oriented Programming</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
