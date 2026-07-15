/*******************************************************************************
 * 01_function_templates.cpp — Write Once, Use with Any Type
 *******************************************************************************
 *
 *  Without templates:         With templates:
 *  int    max(int a, int b)   template<typename T>
 *  double max(double, double) T max(T a, T b) {
 *  string max(string, string)     return (a > b) ? a : b;
 *  ... (endless repetition!)  }
 *                              // ONE definition, works for ALL types!
 *
 *  Template Instantiation:
 *
 *  Source Code:  template<typename T> T max(T a, T b);
 *                         │
 *          ┌──────────────┼──────────────┐
 *          ▼              ▼              ▼
 *  max(3, 4)     max(3.14, 2.71)   max("a", "b")
 *          │              │              │
 *          ▼              ▼              ▼
 *  int max(int, int)  double max(...)  const char* max(...)
 *  (generated)         (generated)      (generated)
 *
 * COMPILE: g++ -std=c++17 -o func_templates 01_function_templates.cpp
 ******************************************************************************/

#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════
// 1. BASIC FUNCTION TEMPLATE
// ═════════════════════════════════════════════════════════════════

template<typename T>  // 'typename' and 'class' are interchangeable here
T maximum(T a, T b) {
    return (a > b) ? a : b;
}

// ═════════════════════════════════════════════════════════════════
// 2. MULTIPLE TEMPLATE PARAMETERS
// ═════════════════════════════════════════════════════════════════

template<typename T, typename U>
auto add(T a, U b) {  // Return type deduced (C++14) or use -> decltype(a+b)
    return a + b;
}

// ═════════════════════════════════════════════════════════════════
// 3. NON-TYPE TEMPLATE PARAMETERS
// ═════════════════════════════════════════════════════════════════

template<typename T, int N>
void printArray(const T (&arr)[N]) {  // Array reference — size preserved!
    for (int i = 0; i < N; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << "(size=" << N << ")\n";
}

// ═════════════════════════════════════════════════════════════════
// 4. TEMPLATE SPECIALIZATION
// ═════════════════════════════════════════════════════════════════

// Primary template
template<typename T>
void display(T value) {
    std::cout << "  Generic: " << value << '\n';
}

// Explicit specialization for const char*
template<>
void display<const char*>(const char* value) {
    std::cout << "  String specialization: \"" << value << "\"\n";
}

// Explicit specialization for bool
template<>
void display<bool>(bool value) {
    std::cout << "  Bool specialization: " << (value ? "TRUE" : "FALSE") << '\n';
}

int main() {
    std::cout << "=== BASIC TEMPLATE ===\n";
    std::cout << "max(3, 7) = " << maximum(3, 7) << '\n';
    std::cout << "max(3.14, 2.71) = " << maximum(3.14, 2.71) << '\n';
    std::cout << "max(\"apple\", \"banana\") = " << maximum(std::string("apple"), std::string("banana")) << '\n';

    // Explicit type specification
    std::cout << "max<double>(3, 4.5) = " << maximum<double>(3, 4.5) << '\n';

    std::cout << "\n=== MULTIPLE TYPE PARAMS ===\n";
    std::cout << "add(3, 4.5) = " << add(3, 4.5) << '\n';  // int + double = double

    std::cout << "\n=== NON-TYPE PARAMS ===\n";
    int nums[] = {1, 2, 3, 4, 5};
    double vals[] = {1.1, 2.2, 3.3};
    printArray(nums);    // N deduced as 5
    printArray(vals);    // N deduced as 3

    std::cout << "\n=== SPECIALIZATION ===\n";
    display(42);           // Generic
    display(3.14);         // Generic
    display("hello");      // String specialization
    display(true);         // Bool specialization

    return 0;
}
