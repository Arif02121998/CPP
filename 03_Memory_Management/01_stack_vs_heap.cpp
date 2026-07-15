/*******************************************************************************
 * 01_stack_vs_heap.cpp — Stack vs Heap Memory
 *******************************************************************************
 *
 *  ┌──────────────────────────┬──────────────────────────────────┐
 *  │         STACK            │            HEAP                  │
 *  ├──────────────────────────┼──────────────────────────────────┤
 *  │ Automatic allocation     │ Manual allocation (new/malloc)   │
 *  │ Very fast (pointer bump) │ Slower (memory manager)          │
 *  │ Fixed size (1-8 MB)     │ Large (limited by RAM)           │
 *  │ LIFO order              │ Any order alloc/dealloc          │
 *  │ Freed when scope ends   │ Must be explicitly freed         │
 *  │ Contiguous              │ Fragmented over time             │
 *  │ Local vars, params      │ Dynamic objects, large arrays    │
 *  └──────────────────────────┴──────────────────────────────────┘
 *
 *  Stack Frame per Function Call:
 *
 *    main()        ┌──────────────────┐  High addr
 *                  │ local vars       │
 *                  │ return address   │
 *    ──────────────├──────────────────┤
 *    funcA()       │ local vars       │
 *                  │ parameters       │
 *                  │ return address   │
 *    ──────────────├──────────────────┤
 *    funcB()       │ local vars       │  ← Stack pointer (SP)
 *                  └──────────────────┘  Low addr
 *
 * COMPILE: g++ -std=c++17 -o stack_heap 01_stack_vs_heap.cpp
 ******************************************************************************/

#include <iostream>

void stackExample() {
    // All on the STACK — automatically freed when function returns
    int x = 42;                  // 4 bytes on stack
    double arr[100];             // 800 bytes on stack
    char name[] = "Hello";       // 6 bytes on stack

    std::cout << "Stack variable x at: " << &x << '\n';
    std::cout << "Stack array at:      " << arr << '\n';
}

void heapExample() {
    // On the HEAP — manually managed
    int* p = new int(42);            // 4 bytes on heap
    double* arr = new double[100];   // 800 bytes on heap

    std::cout << "Heap variable at:  " << p << '\n';
    std::cout << "Heap array at:     " << arr << '\n';

    // MUST free heap memory!
    delete p;
    delete[] arr;
}

int main() {
    std::cout << "=== STACK ALLOCATION ===\n";
    stackExample();

    std::cout << "\n=== HEAP ALLOCATION ===\n";
    heapExample();

    /*
     * When to use Stack vs Heap:
     *
     * USE STACK when:
     *   ✓ Size known at compile time
     *   ✓ Small objects (< few KB)
     *   ✓ Short-lived variables
     *
     * USE HEAP when:
     *   ✓ Size determined at runtime
     *   ✓ Large objects (> few KB)
     *   ✓ Object must outlive current scope
     *   ✓ Polymorphic objects (base class pointer)
     *
     * BEST: Use smart pointers for heap allocation (C++11)
     */

    return 0;
}
