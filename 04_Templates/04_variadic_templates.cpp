/*******************************************************************************
 * 04_variadic_templates.cpp — Templates with Variable Number of Arguments
 *******************************************************************************
 *
 *  Variadic templates (C++11) accept any number of template arguments.
 *
 *  template<typename... Args>     // Args is a "parameter pack"
 *  void print(Args... args);      // Expand with ...
 *
 *  print(1, "hello", 3.14);  → Args = {int, const char*, double}
 *
 * COMPILE: g++ -std=c++17 -o variadic 04_variadic_templates.cpp
 ******************************************************************************/

#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════
// 1. RECURSIVE VARIADIC TEMPLATE (C++11 way)
// ═════════════════════════════════════════════════════════════════

// Base case (no more arguments)
void print() {
    std::cout << '\n';
}

// Recursive case: process first arg, recurse for rest
template<typename T, typename... Args>
void print(T first, Args... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ", ";
    }
    print(rest...);  // Expand remaining args
}

// ═════════════════════════════════════════════════════════════════
// 2. FOLD EXPRESSIONS (C++17) — Much cleaner!
// ═════════════════════════════════════════════════════════════════

template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // Unary right fold: a1 + (a2 + (a3 + a4))
}

template<typename... Args>
void printAll(Args... args) {
    ((std::cout << args << " "), ...);  // Comma fold
    std::cout << '\n';
}

// ═════════════════════════════════════════════════════════════════
// 3. SIZEOF... — Count arguments
// ═════════════════════════════════════════════════════════════════

template<typename... Args>
void countArgs(Args... args) {
    std::cout << "  Number of arguments: " << sizeof...(args) << '\n';
}

int main() {
    std::cout << "=== RECURSIVE PRINT ===\n";
    print(1, "hello", 3.14, true, 'X');

    std::cout << "\n=== FOLD EXPRESSIONS (C++17) ===\n";
    std::cout << "sum(1,2,3,4,5) = " << sum(1, 2, 3, 4, 5) << '\n';
    std::cout << "printAll: ";
    printAll(1, "hello", 3.14, true);

    std::cout << "\n=== SIZEOF... ===\n";
    countArgs(1, 2, 3);
    countArgs("a", "b", "c", "d", "e");

    return 0;
}
