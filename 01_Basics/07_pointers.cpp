/*******************************************************************************
 * 07_pointers.cpp — Pointers: Memory Addresses and Indirection
 *******************************************************************************
 *
 * CONCEPT: A pointer is a variable that stores the MEMORY ADDRESS of another
 *          variable. Pointers enable dynamic memory, data structures, and
 *          efficient passing of large objects.
 *
 *  ┌─────────────────────────────────────────────────────────────────┐
 *  │                                                                 │
 *  │   Variable x:              Pointer p:                          │
 *  │   ┌─────────┐             ┌─────────────┐                     │
 *  │   │   42    │             │  0x7fff5a2c │ ── points to ──► x  │
 *  │   └─────────┘             └─────────────┘                     │
 *  │   Address: 0x7fff5a2c     Address: 0x7fff5a30                 │
 *  │                                                                 │
 *  │   int x = 42;                                                  │
 *  │   int* p = &x;   // p stores address of x                     │
 *  │   *p == 42        // dereferencing p gives value of x          │
 *  │                                                                 │
 *  └─────────────────────────────────────────────────────────────────┘
 *
 * KEY OPERATORS:
 *   &  (address-of)   — Gets the memory address of a variable
 *   *  (dereference)  — Access the value stored AT that address
 *
 * COMPILE: g++ -std=c++17 -o pointers 07_pointers.cpp
 ******************************************************************************/

#include <iostream>

int main() {

    // ═════════════════════════════════════════════════════════════════
    // 1. DECLARING AND USING POINTERS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "=== BASIC POINTERS ===\n";

    int x = 42;
    int* ptr = &x;   // ptr stores address of x

    /*
     *  Visual:
     *
     *    ptr ──────────────────────────────► x
     *   ┌────────────┐                    ┌──────┐
     *   │ 0x7fff5a2c │                    │  42  │
     *   └────────────┘                    └──────┘
     *   (stores address)                  (actual value)
     *
     *   ptr   == 0x7fff5a2c  (address of x)
     *   *ptr  == 42          (value at that address)
     *   &x    == 0x7fff5a2c  (address of x)
     */

    std::cout << "x       = " << x     << '\n';        // 42
    std::cout << "&x      = " << &x    << '\n';        // Address of x
    std::cout << "ptr     = " << ptr   << '\n';        // Same address
    std::cout << "*ptr    = " << *ptr  << '\n';        // 42 (dereference)

    // Modify value through pointer
    *ptr = 100;
    std::cout << "After *ptr = 100: x = " << x << '\n';  // 100

    // ═════════════════════════════════════════════════════════════════
    // 2. POINTER TYPES AND SIZES
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== POINTER SIZES ===\n";

    /*
     *  All pointers are the same size (8 bytes on 64-bit systems)
     *  The TYPE determines how many bytes are read when dereferencing.
     *
     *  int*    → reads 4 bytes at address
     *  char*   → reads 1 byte at address
     *  double* → reads 8 bytes at address
     */

    std::cout << "sizeof(int*)    = " << sizeof(int*)    << " bytes\n";
    std::cout << "sizeof(char*)   = " << sizeof(char*)   << " bytes\n";
    std::cout << "sizeof(double*) = " << sizeof(double*) << " bytes\n";

    // ═════════════════════════════════════════════════════════════════
    // 3. NULL AND NULLPTR
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== NULLPTR ===\n";

    // Old C-style (AVOID):
    // int* p1 = NULL;    // NULL is just 0, can cause ambiguity
    // int* p2 = 0;       // Same problem

    // Modern C++ (ALWAYS USE):
    int* p3 = nullptr;    // Type-safe null pointer (C++11)

    if (p3 == nullptr) {
        std::cout << "p3 is null — not pointing to anything\n";
    }

    // ALWAYS check for nullptr before dereferencing!
    // *p3;  // UNDEFINED BEHAVIOR! Crash (segfault)

    // ═════════════════════════════════════════════════════════════════
    // 4. POINTER ARITHMETIC
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== POINTER ARITHMETIC ===\n";

    int arr[] = {10, 20, 30, 40, 50};
    int* p = arr;  // arr decays to pointer to first element

    /*
     *  Pointer Arithmetic:
     *
     *    p         p+1       p+2       p+3       p+4
     *    │         │         │         │         │
     *    ▼         ▼         ▼         ▼         ▼
     *  ┌─────────┬─────────┬─────────┬─────────┬─────────┐
     *  │   10    │   20    │   30    │   40    │   50    │
     *  └─────────┴─────────┴─────────┴─────────┴─────────┘
     *  0x100     0x104     0x108     0x10C     0x110
     *       +4bytes   +4bytes   +4bytes   +4bytes
     *
     *  (p + i) advances by i * sizeof(int) bytes
     */

    std::cout << "*p       = " << *p       << '\n';  // 10
    std::cout << "*(p + 1) = " << *(p + 1) << '\n';  // 20
    std::cout << "*(p + 2) = " << *(p + 2) << '\n';  // 30
    std::cout << "*(p + 4) = " << *(p + 4) << '\n';  // 50

    // Pointer increment
    std::cout << "Iterating with pointer: ";
    for (int* it = arr; it != arr + 5; it++) {
        std::cout << *it << " ";
    }
    std::cout << '\n';

    // Array indexing IS pointer arithmetic:
    // arr[i] is exactly the same as *(arr + i)
    std::cout << "arr[3] = " << arr[3] << " == *(arr+3) = " << *(arr + 3) << '\n';

    // ═════════════════════════════════════════════════════════════════
    // 5. POINTER TO POINTER
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== POINTER TO POINTER ===\n";

    int value = 42;
    int* ptr1 = &value;
    int** ptr2 = &ptr1;   // Pointer to pointer

    /*
     *   ptr2 ────────► ptr1 ────────► value
     *  ┌──────────┐  ┌──────────┐  ┌──────┐
     *  │ &ptr1    │  │ &value   │  │  42  │
     *  └──────────┘  └──────────┘  └──────┘
     *  (int**)        (int*)        (int)
     *
     *  *ptr2  == ptr1   == &value
     *  **ptr2 == *ptr1  == value  == 42
     */

    std::cout << "value  = " << value   << '\n';   // 42
    std::cout << "*ptr1  = " << *ptr1   << '\n';   // 42
    std::cout << "**ptr2 = " << **ptr2  << '\n';   // 42

    // ═════════════════════════════════════════════════════════════════
    // 6. CONST WITH POINTERS
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== CONST WITH POINTERS ===\n";

    int a = 10, b = 20;

    /*
     *  Read right-to-left:
     *
     *  const int* p      = "p is a pointer to a const int"
     *                       → Cannot change VALUE through p
     *                       → CAN change where p POINTS
     *
     *  int* const p      = "p is a const pointer to an int"
     *                       → CAN change value through p
     *                       → Cannot change where p POINTS
     *
     *  const int* const p = "p is a const pointer to a const int"
     *                       → Cannot change ANYTHING
     */

    // Pointer to const — can't change the VALUE
    const int* pc = &a;
    // *pc = 99;    // ERROR! Can't modify value
    pc = &b;        // OK — can change where it points

    // Const pointer — can't change the POINTER itself
    int* const cp = &a;
    *cp = 99;       // OK — can modify value
    // cp = &b;     // ERROR! Can't change where it points

    // Const pointer to const — both locked
    const int* const cpc = &a;
    // *cpc = 99;   // ERROR!
    // cpc = &b;    // ERROR!

    std::cout << "const pointer tricks demonstrated (see code comments)\n";

    // ═════════════════════════════════════════════════════════════════
    // 7. DYNAMIC MEMORY (new / delete)
    // ═════════════════════════════════════════════════════════════════
    std::cout << "\n=== DYNAMIC MEMORY ===\n";

    /*
     *       Stack                    Heap
     *  ┌──────────────┐        ┌──────────────┐
     *  │ int* dyn_ptr │───────►│    42        │  ← allocated with 'new'
     *  │ (8 bytes)    │        │ (4 bytes)    │
     *  └──────────────┘        └──────────────┘
     *   Auto-freed              Must be freed
     *   when scope ends         with 'delete'!
     */

    // Single value
    int* dynVal = new int(42);
    std::cout << "Dynamic value: " << *dynVal << '\n';
    delete dynVal;       // MUST free! Otherwise memory leak
    dynVal = nullptr;    // Good practice: set to null after delete

    // Array
    int* dynArr = new int[5]{10, 20, 30, 40, 50};
    std::cout << "Dynamic array: ";
    for (int i = 0; i < 5; i++) {
        std::cout << dynArr[i] << " ";
    }
    std::cout << '\n';
    delete[] dynArr;     // Use delete[] for arrays!
    dynArr = nullptr;

    // WARNING: Prefer smart pointers (C++11) over raw new/delete!
    // See 06_CPP11/04_smart_pointers.cpp

    // ═════════════════════════════════════════════════════════════════
    // 8. COMMON POINTER PITFALLS
    // ═════════════════════════════════════════════════════════════════

    /*
     *  ┌─────────────────────────────────────────────────────────────┐
     *  │ PITFALL                │ CAUSE               │ PREVENTION  │
     *  ├────────────────────────┼──────────────────────┼─────────────┤
     *  │ Dangling pointer       │ Using after delete   │ Set nullptr │
     *  │ Memory leak            │ Forgot delete        │ Smart ptrs  │
     *  │ Double delete          │ delete same memory   │ Set nullptr │
     *  │ Wild pointer           │ Uninitialized ptr    │ Init nullptr│
     *  │  Null dereference       │ *nullptr             │ Check first │
     *  │ Buffer overflow        │ Out of bounds        │ Use vector  │
     *  └─────────────────────────────────────────────────────────────┘
     */

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. & gets address, * dereferences (accesses value at address)
 * 2. All pointers are same size (8 bytes on 64-bit) regardless of type
 * 3. Always use nullptr (not NULL or 0) for null pointers
 * 4. Pointer arithmetic: p+1 advances by sizeof(*p) bytes
 * 5. arr[i] == *(arr + i) — array indexing IS pointer arithmetic
 * 6. Use const correctly: "read right to left" rule
 * 7. Always delete what you new; prefer smart pointers
 * 8. Set pointers to nullptr after delete
 */
