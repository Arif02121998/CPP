/*******************************************************************************
 * 02_new_delete.cpp — Dynamic Memory Allocation
 *******************************************************************************
 *
 *  new/delete workflow:
 *
 *   int* p = new int(42);     delete p;
 *        │                         │
 *        ▼                         ▼
 *   1. Allocate memory        1. Call destructor (if object)
 *   2. Call constructor       2. Free memory
 *   3. Return pointer         3. Pointer becomes dangling!
 *
 * COMPILE: g++ -std=c++17 -o newdel 02_new_delete.cpp
 ******************************************************************************/

#include <iostream>
#include <cstdlib>   // malloc, free (C-style — avoid in C++)

class Widget {
    int id_;
public:
    Widget(int id) : id_(id) { std::cout << "  Widget " << id_ << " constructed\n"; }
    ~Widget()                { std::cout << "  Widget " << id_ << " destroyed\n"; }
};

int main() {
    std::cout << "=== SINGLE OBJECT ===\n";
    int* pi = new int(42);
    std::cout << "*pi = " << *pi << '\n';
    delete pi;
    pi = nullptr;  // Good practice!

    std::cout << "\n=== ARRAY ===\n";
    int* arr = new int[5]{10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) std::cout << arr[i] << " ";
    std::cout << '\n';
    delete[] arr;  // MUST use delete[] for arrays!
    arr = nullptr;

    std::cout << "\n=== OBJECTS ===\n";
    Widget* w = new Widget(1);
    delete w;

    std::cout << "\n=== ARRAY OF OBJECTS ===\n";
    Widget* widgets = new Widget[3]{{10}, {20}, {30}};
    delete[] widgets;  // Calls destructor for each, then frees

    std::cout << "\n=== NOTHROW NEW ===\n";
    // Returns nullptr instead of throwing on failure
    int* safe = new(std::nothrow) int[1000000];
    if (safe) {
        std::cout << "Allocation succeeded\n";
        delete[] safe;
    } else {
        std::cout << "Allocation failed!\n";
    }

    /*
     * GOLDEN RULES:
     * 1. Every 'new' needs a matching 'delete'
     * 2. Every 'new[]' needs a matching 'delete[]'
     * 3. Set pointer to nullptr after delete
     * 4. PREFER smart pointers (unique_ptr, shared_ptr) over raw new/delete
     */

    return 0;
}
