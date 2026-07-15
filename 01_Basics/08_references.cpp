/*******************************************************************************
 * 08_references.cpp — References: Aliases for Existing Variables
 *******************************************************************************
 *
 * CONCEPT: A reference is an ALIAS (another name) for an existing variable.
 *          Unlike pointers, references cannot be null or reseated.
 *
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │  POINTER vs REFERENCE                                        │
 *  ├────────────────────────────┬─────────────────────────────────┤
 *  │  Pointer (int* p = &x)    │  Reference (int& r = x)        │
 *  ├────────────────────────────┼─────────────────────────────────┤
 *  │  Stores address            │  Alias (no separate storage*)  │
 *  │  Can be nullptr            │  CANNOT be null                │
 *  │  Can be reseated           │  CANNOT be reseated            │
 *  │  Needs * to dereference    │  Used directly (no syntax)     │
 *  │  Can have ptr-to-ptr       │  No ref-to-ref                 │
 *  │  Can point to nothing      │  Must be initialized           │
 *  └────────────────────────────┴─────────────────────────────────┘
 *  * Compiler may implement refs as pointers internally
 *
 * COMPILE: g++ -std=c++17 -o references 08_references.cpp
 ******************************************************************************/

#include <iostream>
#include <string>

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. BASIC REFERENCES
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== BASIC REFERENCES ===\n";

    int x = 42;
    int& ref = x;   // ref is an alias for x

    /*
     *   x and ref refer to the SAME memory location:
     *
     *   Name: x (also: ref)
     *         ┌──────┐
     *         │  42  │   ← Both x and ref access this
     *         └──────┘
     *         0x7fff5a2c
     *
     *  Changing ref changes x, and vice versa.
     */

    std::cout << "x = " << x << ", ref = " << ref << '\n';        // 42, 42
    std::cout << "&x = " << &x << ", &ref = " << &ref << '\n';    // Same address!

    ref = 100;   // Changes x!
    std::cout << "After ref = 100: x = " << x << '\n';  // 100

    x = 200;     // Also visible through ref
    std::cout << "After x = 200: ref = " << ref << '\n';  // 200

    // ═════════════════════════════════════════════════════════════════
    // 2. REFERENCE RULES
    // ═════════════════════════════════════════════════════════════════

    /*
     *  RULES:
     *  ──────
     *  1. Must be initialized at declaration
     *     int& r;          // ERROR! Must bind to something
     *
     *  2. Cannot be reseated (changed to refer to something else)
     *     int a = 10, b = 20;
     *     int& r = a;
     *     r = b;           // This ASSIGNS b's value to a, doesn't reseat!
     *
     *  3. Cannot be null
     *     int& r = nullptr; // ERROR!
     */

    int a = 10, b = 20;
    int& r = a;
    r = b;  // This does NOT make r point to b!
            // It copies b's value (20) into a
    std::cout << "\nAfter r = b: a = " << a << ", b = " << b << '\n';  // a=20, b=20

    // ═════════════════════════════════════════════════════════════════
    // 3. CONST REFERENCES
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== CONST REFERENCES ===\n";

    int val = 42;
    const int& cref = val;   // Read-only reference

    std::cout << "cref = " << cref << '\n';
    // cref = 99;   // ERROR! Cannot modify through const reference

    // SPECIAL: const references can bind to temporaries (rvalues)!
    const int& temp_ref = 42;       // OK! Extends lifetime of temporary
    const std::string& str_ref = "Hello";  // OK!

    // int& bad_ref = 42;           // ERROR! Non-const ref can't bind to rvalue

    std::cout << "temp_ref = " << temp_ref << '\n';
    std::cout << "str_ref = " << str_ref << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 4. REFERENCES IN FUNCTION PARAMETERS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== REFERENCES IN FUNCTIONS ===\n";

    /*
     *  Three ways to pass to functions:
     *
     *  ┌───────────────────────────────────────────────────────────┐
     *  │ BY VALUE:           void f(int x)                        │
     *  │   ► Copies the value                                     │
     *  │   ► Original unchanged                                   │
     *  │   ► Use for small types (int, char, bool, pointers)      │
     *  ├───────────────────────────────────────────────────────────┤
     *  │ BY REFERENCE:       void f(int& x)                       │
     *  │   ► No copy, modifies original                           │
     *  │   ► Use when function needs to change the argument       │
     *  ├───────────────────────────────────────────────────────────┤
     *  │ BY CONST REFERENCE: void f(const int& x)                 │
     *  │   ► No copy, read-only                                   │
     *  │   ► Use for large objects (strings, vectors, classes)     │
     *  │   ► BEST PRACTICE for read-only parameters               │
     *  └───────────────────────────────────────────────────────────┘
     */

    // Swap using references
    auto swap = [](int& a, int& b) {
        int temp = a;
        a = b;
        b = temp;
    };

    int p = 10, q = 20;
    std::cout << "Before swap: p=" << p << ", q=" << q << '\n';
    swap(p, q);
    std::cout << "After swap:  p=" << p << ", q=" << q << '\n';

    // Print using const reference (efficient, no copy)
    auto print = [](const std::string& s) {
        std::cout << "String: " << s << " (length: " << s.size() << ")\n";
    };

    std::string name = "C++ Reference Guide";
    print(name);

    // ═════════════════════════════════════════════════════════════════
    // 5. RETURNING REFERENCES
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== RETURNING REFERENCES ===\n";

    // Safe: returning reference to element of array/container
    int arr[] = {10, 20, 30, 40, 50};
    auto getElement = [&arr](int index) -> int& {
        return arr[index];  // OK: array outlives the function call
    };

    getElement(2) = 99;  // Modify arr[2] through returned reference!
    std::cout << "arr[2] after modification = " << arr[2] << '\n';  // 99

    // DANGER: Never return reference to a local variable!
    // int& bad() {
    //     int local = 42;
    //     return local;  // UNDEFINED BEHAVIOR! local is destroyed
    // }

    // ═════════════════════════════════════════════════════════════════
    // 6. REFERENCE vs POINTER SUMMARY
    // ═════════════════════════════════════════════════════════════════

    /*
     *  When to use which:
     *
     *  ┌──────────────────────────────────────────────────────────┐
     *  │              USE REFERENCE WHEN:                          │
     *  │  ✓ You always have a valid object (never null)           │
     *  │  ✓ You don't need to reseat                              │
     *  │  ✓ Cleaner syntax is desired                             │
     *  │  ✓ Function parameters (especially const T&)             │
     *  │  ✓ Operator overloading                                  │
     *  ├──────────────────────────────────────────────────────────┤
     *  │              USE POINTER WHEN:                            │
     *  │  ✓ Value might be absent (nullptr)                       │
     *  │  ✓ Need to reseat (point to different object)            │
     *  │  ✓ Dynamic memory allocation                             │
     *  │  ✓ Need pointer arithmetic                               │
     *  │  ✓ Working with C APIs                                   │
     *  └──────────────────────────────────────────────────────────┘
     */

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. A reference is an ALIAS — same object, different name
 * 2. Must be initialized, cannot be null, cannot be reseated
 * 3. Use const T& for read-only function parameters (no copies)
 * 4. const references can bind to temporaries (rvalues)
 * 5. Never return a reference to a local variable
 * 6. Prefer references over pointers when possible
 */
