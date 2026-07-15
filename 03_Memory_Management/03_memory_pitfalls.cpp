/*******************************************************************************
 * 03_memory_pitfalls.cpp — Common Memory Bugs
 *******************************************************************************
 *
 *  ┌─────────────────────────────────────────────────────────────────┐
 *  │  BUG                 │ CAUSE                  │ PREVENTION      │
 *  ├──────────────────────┼────────────────────────┼─────────────────┤
 *  │ Memory Leak          │ new without delete     │ Smart pointers  │
 *  │ Dangling Pointer     │ Use after delete       │ Set to nullptr  │
 *  │ Double Free          │ delete same ptr twice  │ Set to nullptr  │
 *  │ Buffer Overflow      │ Write past array end   │ Use vector      │
 *  │ Uninitialized Ptr    │ Deref uninitialized    │ Init to nullptr │
 *  │ Stack Overflow       │ Deep/infinite recursion│ Limit depth     │
 *  └──────────────────────┴────────────────────────┴─────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o pitfalls 03_memory_pitfalls.cpp
 ******************************************************************************/

#include <iostream>
#include <memory>

// Each function demonstrates a pitfall (in comments) and the fix

void memoryLeak() {
    std::cout << "=== MEMORY LEAK ===\n";
    // BAD:
    // int* p = new int(42);
    // return;  // p is lost! Memory never freed!

    // GOOD: Use smart pointer
    auto p = std::make_unique<int>(42);
    std::cout << "  Value: " << *p << " (auto-freed)\n";
    // Automatically deleted when p goes out of scope
}

void danglingPointer() {
    std::cout << "\n=== DANGLING POINTER ===\n";
    int* p = new int(42);
    delete p;
    // BAD: *p is now undefined behavior (dangling!)
    // std::cout << *p;  // CRASH or garbage

    // GOOD: Set to nullptr after delete
    p = nullptr;
    if (p) {
        std::cout << *p;
    } else {
        std::cout << "  Pointer is null — safe!\n";
    }
}

void doubleFree() {
    std::cout << "\n=== DOUBLE FREE ===\n";
    int* p = new int(42);
    delete p;
    // BAD: delete p;  // CRASH! Already freed!

    // GOOD: nullptr
    p = nullptr;
    delete p;  // Deleting nullptr is safe (no-op)
    std::cout << "  Double delete avoided with nullptr\n";
}

int main() {
    memoryLeak();
    danglingPointer();
    doubleFree();

    std::cout << "\n=== SOLUTION: SMART POINTERS ===\n";
    std::cout << "  Use unique_ptr, shared_ptr, weak_ptr\n";
    std::cout << "  See 06_CPP11/04_smart_pointers.cpp for details\n";

    return 0;
}
