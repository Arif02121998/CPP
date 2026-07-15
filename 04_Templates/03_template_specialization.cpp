/*******************************************************************************
 * 03_template_specialization.cpp — Full and Partial Specialization
 *******************************************************************************
 *
 *  Specialization = Provide a custom implementation for specific types
 *
 *  Primary:     template<typename T> class Printer { ... };
 *  Full spec:   template<> class Printer<bool> { ... };       ← Specific type
 *  Partial spec: template<typename T> class Printer<T*> { ... }; ← Pattern
 *
 * COMPILE: g++ -std=c++17 -o specialization 03_template_specialization.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <cstring>

// ═════════════════════════════════════════════════════════════════
// PRIMARY TEMPLATE
// ═════════════════════════════════════════════════════════════════
template<typename T>
class TypeInfo {
public:
    static void describe() {
        std::cout << "  Generic type, size = " << sizeof(T) << " bytes\n";
    }
};

// ═════════════════════════════════════════════════════════════════
// FULL SPECIALIZATION
// ═════════════════════════════════════════════════════════════════
template<>
class TypeInfo<int> {
public:
    static void describe() {
        std::cout << "  Integer type (32-bit signed)\n";
    }
};

template<>
class TypeInfo<std::string> {
public:
    static void describe() {
        std::cout << "  String type (dynamic, heap-allocated)\n";
    }
};

// ═════════════════════════════════════════════════════════════════
// PARTIAL SPECIALIZATION (for pointer types)
// ═════════════════════════════════════════════════════════════════
template<typename T>
class TypeInfo<T*> {
public:
    static void describe() {
        std::cout << "  Pointer to type, size = " << sizeof(T*) << " bytes\n";
    }
};

// ═════════════════════════════════════════════════════════════════
// SFINAE Example: "Substitution Failure Is Not An Error"
// ═════════════════════════════════════════════════════════════════

// Only enabled for arithmetic types
template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
safeDivide(T a, T b) {
    if (b == 0) {
        std::cout << "  Division by zero!\n";
        return 0;
    }
    return a / b;
}

int main() {
    std::cout << "=== FULL SPECIALIZATION ===\n";
    TypeInfo<double>::describe();       // Generic
    TypeInfo<int>::describe();          // Specialized for int
    TypeInfo<std::string>::describe();  // Specialized for string

    std::cout << "\n=== PARTIAL SPECIALIZATION ===\n";
    TypeInfo<int*>::describe();         // Pointer specialization
    TypeInfo<double*>::describe();      // Pointer specialization

    std::cout << "\n=== SFINAE ===\n";
    std::cout << "  10 / 3 = " << safeDivide(10, 3) << '\n';
    std::cout << "  10.0 / 0 = ";
    safeDivide(10.0, 0.0);
    // safeDivide(std::string("a"), std::string("b")); // Won't compile — SFINAE!

    return 0;
}
