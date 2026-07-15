/*******************************************************************************
 * 01_cpp14_features.cpp — All C++14 Features
 *******************************************************************************
 * COMPILE: g++ -std=c++14 -o cpp14 01_cpp14_features.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// ═════════════════════════════════════════════════════════════════
// 1. GENERIC LAMBDAS (auto parameters)
// ═════════════════════════════════════════════════════════════════

// C++11: [](int a, int b) { return a + b; }
// C++14: [](auto a, auto b) { return a + b; }  ← Works with ANY type!

// ═════════════════════════════════════════════════════════════════
// 2. RETURN TYPE DEDUCTION (auto return)
// ═════════════════════════════════════════════════════════════════

// C++11: auto add(int a, int b) -> decltype(a + b) { return a + b; }
// C++14: auto add(int a, int b) { return a + b; }  ← No trailing return needed!

auto multiply(double a, double b) {
    return a * b;  // Compiler deduces return type as double
}

// ═════════════════════════════════════════════════════════════════
// 3. VARIABLE TEMPLATES
// ═════════════════════════════════════════════════════════════════

template<typename T>
constexpr T pi = T(3.14159265358979323846);

// ═════════════════════════════════════════════════════════════════
// 4. RELAXED CONSTEXPR
// ═════════════════════════════════════════════════════════════════

// C++11 constexpr: only return statement allowed
// C++14 constexpr: loops, if/else, local variables allowed!

constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {  // Loops OK in C++14!
        result *= i;
    }
    return result;
}

constexpr int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;

    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// ═════════════════════════════════════════════════════════════════
// 5. [[deprecated]] ATTRIBUTE
// ═════════════════════════════════════════════════════════════════

[[deprecated("Use newFunction() instead")]]
void oldFunction() {
    std::cout << "  Old function (generates compiler warning)\n";
}

void newFunction() {
    std::cout << "  New function\n";
}

// ═════════════════════════════════════════════════════════════════
// 6. std::make_unique (was missing from C++11!)
// ═════════════════════════════════════════════════════════════════

class Widget {
public:
    int value;
    Widget(int v) : value(v) { std::cout << "  Widget(" << v << ")\n"; }
    ~Widget() { std::cout << "  ~Widget(" << value << ")\n"; }
};

int main() {
    std::cout << "=== GENERIC LAMBDAS ===\n";
    auto add = [](auto a, auto b) { return a + b; };

    std::cout << "int:    " << add(3, 4) << '\n';
    std::cout << "double: " << add(3.14, 2.71) << '\n';
    std::cout << "string: " << add(std::string("Hello "), std::string("World")) << '\n';

    // Generic lambda for sorting
    std::vector<int> v = {5, 2, 8, 1, 9};
    std::sort(v.begin(), v.end(), [](auto a, auto b) { return a > b; });
    std::cout << "Sorted: ";
    for (auto x : v) std::cout << x << " ";
    std::cout << '\n';

    std::cout << "\n=== RETURN TYPE DEDUCTION ===\n";
    std::cout << "multiply(3.0, 4.0) = " << multiply(3.0, 4.0) << '\n';

    std::cout << "\n=== VARIABLE TEMPLATES ===\n";
    std::cout << "pi<float>:  " << pi<float> << '\n';
    std::cout << "pi<double>: " << pi<double> << '\n';

    std::cout << "\n=== RELAXED CONSTEXPR ===\n";
    constexpr int fact10 = factorial(10);
    constexpr int fib15  = fibonacci(15);
    std::cout << "10! = " << fact10 << " (compile-time)\n";
    std::cout << "fib(15) = " << fib15 << " (compile-time)\n";

    std::cout << "\n=== BINARY LITERALS & DIGIT SEPARATORS ===\n";
    int binary = 0b1010'1100;     // Binary literal with separator
    int million = 1'000'000;       // Much more readable!
    double sci = 1'234.567'89;

    std::cout << "0b1010'1100 = " << binary << '\n';
    std::cout << "1'000'000 = " << million << '\n';

    std::cout << "\n=== std::make_unique ===\n";
    {
        auto w = std::make_unique<Widget>(42);
        std::cout << "  Value: " << w->value << '\n';
    }

    // oldFunction();  // Would generate deprecation warning
    newFunction();

    return 0;
}
