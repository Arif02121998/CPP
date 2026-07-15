/*******************************************************************************
 * 05_functions.cpp — Functions: Declaration, Definition, Overloading
 *******************************************************************************
 *
 * CONCEPT: Functions are reusable blocks of code that perform a specific task.
 *
 *  ┌────────────────────────────────────────────────────────────┐
 *  │  return_type  function_name(parameter_list) {              │
 *  │      // function body                                      │
 *  │      return value;                                         │
 *  │  }                                                         │
 *  └────────────────────────────────────────────────────────────┘
 *
 *  Function Call Stack:
 *
 *   Stack grows ↓     ┌──────────────┐
 *                     │   main()     │ ← Called first
 *                     ├──────────────┤
 *                     │   func_a()   │ ← main calls func_a
 *                     ├──────────────┤
 *                     │   func_b()   │ ← func_a calls func_b
 *                     └──────────────┘ ← func_b returns first (LIFO)
 *
 * COMPILE: g++ -std=c++17 -o functions 05_functions.cpp
 ******************************************************************************/

#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════
// FUNCTION DECLARATION (Prototype) — tells compiler the signature
// Usually placed in header files (.h/.hpp)
// ═════════════════════════════════════════════════════════════════
void greet(const std::string& name);
int add(int a, int b);
double multiply(double a, double b);

// ═════════════════════════════════════════════════════════════════
// 1. BASIC FUNCTIONS
// ═════════════════════════════════════════════════════════════════

// void function — returns nothing
void greet(const std::string& name) {
    std::cout << "Hello, " << name << "!\n";
}

// Function with return value
int add(int a, int b) {
    return a + b;
}

double multiply(double a, double b) {
    return a * b;
}

// ═════════════════════════════════════════════════════════════════
// 2. PASS BY VALUE vs PASS BY REFERENCE vs PASS BY POINTER
// ═════════════════════════════════════════════════════════════════

/*
 *  Pass by Value          Pass by Reference       Pass by Pointer
 *  ┌──────┐  copy         ┌──────┐  alias        ┌──────┐  address
 *  │ arg  │──────►func    │ arg  │◄─────func     │ arg  │──ptr──►func
 *  └──────┘  (separate)   └──────┘  (same var)   └──────┘  (via addr)
 *
 *  Changes in func        Changes in func         Changes via *ptr
 *  do NOT affect arg      DO affect arg           DO affect arg
 */

// Pass by VALUE (copy — original unchanged)
void doubleByValue(int x) {
    x = x * 2;  // Only changes local copy
    std::cout << "  Inside (value): x = " << x << '\n';
}

// Pass by REFERENCE (alias — modifies original)
void doubleByReference(int& x) {
    x = x * 2;  // Changes the original!
    std::cout << "  Inside (ref):   x = " << x << '\n';
}

// Pass by POINTER (via address — modifies original)
void doubleByPointer(int* x) {
    *x = *x * 2;  // Dereference to modify
    std::cout << "  Inside (ptr):   *x = " << *x << '\n';
}

// Pass by CONST REFERENCE (read-only, no copy — best for large objects)
void printName(const std::string& name) {
    // name += "!";  // ERROR! Cannot modify const reference
    std::cout << "  Name: " << name << '\n';
}

// ═════════════════════════════════════════════════════════════════
// 3. DEFAULT PARAMETERS
// ═════════════════════════════════════════════════════════════════
void printMessage(const std::string& msg, int times = 1, char end = '\n') {
    for (int i = 0; i < times; i++) {
        std::cout << msg << end;
    }
}
// RULE: Default params must be rightmost — no gaps allowed!
// void bad(int a = 1, int b, int c = 3);  // ERROR!

// ═════════════════════════════════════════════════════════════════
// 4. FUNCTION OVERLOADING (same name, different parameters)
// ═════════════════════════════════════════════════════════════════

/*
 *  Overloading Resolution:
 *
 *  print(42)     ──► print(int x)        ← Exact match
 *  print(3.14)   ──► print(double x)     ← Exact match
 *  print("hi")   ──► print(string s)     ← Exact match
 *  print(42, 10) ──► print(int x, int y) ← Different param count
 *
 *  The compiler picks the BEST match based on argument types.
 *  Overloading is resolved at COMPILE TIME (static polymorphism).
 */

int calculate(int a, int b) {
    std::cout << "  int version: ";
    return a + b;
}

double calculate(double a, double b) {
    std::cout << "  double version: ";
    return a + b;
}

int calculate(int a, int b, int c) {
    std::cout << "  3-param version: ";
    return a + b + c;
}

// NOTE: Return type alone is NOT enough to distinguish overloads!
// int  func(int x);
// double func(int x);  // ERROR! Same params, different return type

// ═════════════════════════════════════════════════════════════════
// 5. INLINE FUNCTIONS
// ═════════════════════════════════════════════════════════════════

/*
 *  Normal Call:                    Inline:
 *  ┌────────┐                     ┌────────┐
 *  │ caller │──call──►function    │ caller │
 *  │        │◄─return─┘           │  [code │ ← Function body inserted
 *  │        │                     │  here] │    directly (no call overhead)
 *  └────────┘                     └────────┘
 *
 *  'inline' is a HINT to the compiler — it may ignore it.
 *  Modern compilers decide inlining automatically.
 */

inline int square(int x) {
    return x * x;
}

// ═════════════════════════════════════════════════════════════════
// 6. STATIC LOCAL VARIABLES
// ═════════════════════════════════════════════════════════════════
int countCalls() {
    static int count = 0;  // Initialized ONCE, persists across calls
    return ++count;
}

// ═════════════════════════════════════════════════════════════════
// 7. RECURSION
// ═════════════════════════════════════════════════════════════════

/*
 *  factorial(5)
 *  │
 *  └─► 5 * factorial(4)
 *         │
 *         └─► 4 * factorial(3)
 *                │
 *                └─► 3 * factorial(2)
 *                       │
 *                       └─► 2 * factorial(1)
 *                              │
 *                              └─► return 1  (base case)
 *                           return 2
 *                       return 6
 *                  return 24
 *             return 120
 */

int factorial(int n) {
    if (n <= 1) return 1;        // Base case — stops recursion
    return n * factorial(n - 1); // Recursive case
}

int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// ═════════════════════════════════════════════════════════════════
// 8. FUNCTION POINTERS
// ═════════════════════════════════════════════════════════════════
int operate(int a, int b, int (*operation)(int, int)) {
    return operation(a, b);
}

int addOp(int a, int b) { return a + b; }
int subOp(int a, int b) { return a - b; }

// ═════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════
int main() {

    // --- Basic functions ---
    std::cout << "=== BASIC FUNCTIONS ===\n";
    greet("World");
    std::cout << "add(3, 4) = " << add(3, 4) << '\n';
    std::cout << "multiply(2.5, 4.0) = " << multiply(2.5, 4.0) << '\n';

    // --- Pass by value/reference/pointer ---
    std::cout << "\n=== PASS BY VALUE vs REFERENCE vs POINTER ===\n";
    int val = 10;

    std::cout << "Original: val = " << val << '\n';
    doubleByValue(val);
    std::cout << "After byValue: val = " << val << " (unchanged)\n";

    doubleByReference(val);
    std::cout << "After byReference: val = " << val << " (changed!)\n";

    doubleByPointer(&val);
    std::cout << "After byPointer: val = " << val << " (changed!)\n";

    printName("Alice");

    // --- Default parameters ---
    std::cout << "\n=== DEFAULT PARAMETERS ===\n";
    printMessage("Hello");           // times=1, end='\n'
    printMessage("Repeat", 3);       // end='\n'
    printMessage("Star", 5, '*');    // Custom end char
    std::cout << '\n';

    // --- Function overloading ---
    std::cout << "\n=== FUNCTION OVERLOADING ===\n";
    std::cout << calculate(3, 4) << '\n';          // int version
    std::cout << calculate(3.14, 2.71) << '\n';    // double version
    std::cout << calculate(1, 2, 3) << '\n';       // 3-param version

    // --- Inline ---
    std::cout << "\n=== INLINE ===\n";
    std::cout << "square(7) = " << square(7) << '\n';

    // --- Static local ---
    std::cout << "\n=== STATIC LOCAL ===\n";
    std::cout << "Call count: " << countCalls() << '\n';  // 1
    std::cout << "Call count: " << countCalls() << '\n';  // 2
    std::cout << "Call count: " << countCalls() << '\n';  // 3

    // --- Recursion ---
    std::cout << "\n=== RECURSION ===\n";
    std::cout << "factorial(5) = " << factorial(5) << '\n';     // 120
    std::cout << "fibonacci(10) = " << fibonacci(10) << '\n';   // 55

    // --- Function pointers ---
    std::cout << "\n=== FUNCTION POINTERS ===\n";
    std::cout << "operate(10, 3, add) = " << operate(10, 3, addOp) << '\n';
    std::cout << "operate(10, 3, sub) = " << operate(10, 3, subOp) << '\n';

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Declare functions before calling them (or use prototypes)
 * 2. Pass large objects by const reference (const T&) to avoid copies
 * 3. Use pass-by-reference (&) when you need to modify the argument
 * 4. Function overloading: same name, different parameter types/count
 * 5. Default parameters must be rightmost
 * 6. static local variables persist across function calls
 * 7. Every recursion needs a base case to prevent infinite loops
 */
