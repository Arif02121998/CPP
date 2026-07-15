/*******************************************************************************
 * 02_lambda.cpp — Lambda Expressions: Inline Anonymous Functions
 *******************************************************************************
 *
 *  Lambda Syntax:
 *
 *  [capture](parameters) -> return_type { body }
 *
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │  [captures]  — What external variables to access             │
 *  │  (params)    — Function parameters                           │
 *  │  -> type     — Return type (optional, usually deduced)       │
 *  │  { body }    — Function body                                 │
 *  │                                                              │
 *  │  CAPTURE OPTIONS:                                            │
 *  │  []        — Capture nothing                                 │
 *  │  [x]       — Capture x by value (copy)                      │
 *  │  [&x]      — Capture x by reference                         │
 *  │  [=]       — Capture ALL by value                            │
 *  │  [&]       — Capture ALL by reference                        │
 *  │  [=, &x]   — All by value, except x by reference            │
 *  │  [this]    — Capture 'this' pointer (class member access)   │
 *  │  [*this]   — Capture copy of *this (C++17)                  │
 *  └──────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o lambda 02_lambda.cpp
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>

int main() {

    // ═══════════════ BASIC LAMBDA ═══════════════
    std::cout << "=== BASIC LAMBDA ===\n";

    auto greet = []() {
        std::cout << "Hello from lambda!\n";
    };
    greet();  // Call it like a function

    auto add = [](int a, int b) { return a + b; };
    std::cout << "add(3, 4) = " << add(3, 4) << '\n';

    // With explicit return type
    auto divide = [](double a, double b) -> double {
        if (b == 0) return 0;
        return a / b;
    };
    std::cout << "divide(10, 3) = " << divide(10, 3) << '\n';

    // ═══════════════ CAPTURES ═══════════════
    std::cout << "\n=== CAPTURES ===\n";

    int x = 10, y = 20;

    // Capture by value (copy)
    auto byValue = [x, y]() {
        std::cout << "  By value: x=" << x << ", y=" << y << '\n';
        // x++;  // ERROR! captured by value is const by default
    };

    // Capture by reference (can modify)
    auto byRef = [&x, &y]() {
        x += 5;
        y += 5;
        std::cout << "  By ref: x=" << x << ", y=" << y << '\n';
    };

    // Mutable lambda — allows modifying value captures (local copy only!)
    auto mutableLambda = [x]() mutable {
        x += 100;  // Modifies LOCAL copy, not original
        std::cout << "  Mutable: local x=" << x << '\n';
    };

    byValue();
    byRef();
    std::cout << "  After byRef: x=" << x << ", y=" << y << '\n';
    mutableLambda();
    std::cout << "  After mutable: x=" << x << " (unchanged!)\n";

    // Capture all by value / reference
    auto allByVal = [=]() { std::cout << "  [=] x=" << x << '\n'; };
    auto allByRef = [&]() { x = 99; };
    allByVal();
    allByRef();
    std::cout << "  After [&]: x=" << x << '\n';

    // ═══════════════ LAMBDA WITH STL ═══════════════
    std::cout << "\n=== LAMBDA WITH STL ===\n";

    std::vector<int> nums = {5, 3, 8, 1, 9, 2, 7};

    // Sort with custom comparison
    std::sort(nums.begin(), nums.end(), [](int a, int b) {
        return a > b;  // Descending
    });
    std::cout << "Sorted desc: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << '\n';

    // Count elements matching condition
    int threshold = 5;
    int count = std::count_if(nums.begin(), nums.end(),
        [threshold](int n) { return n > threshold; });
    std::cout << "Count > " << threshold << ": " << count << '\n';

    // Transform
    std::vector<int> squared(nums.size());
    std::transform(nums.begin(), nums.end(), squared.begin(),
        [](int n) { return n * n; });
    std::cout << "Squared: ";
    for (int n : squared) std::cout << n << " ";
    std::cout << '\n';

    // For each
    std::cout << "For each: ";
    std::for_each(nums.begin(), nums.end(), [](int n) {
        std::cout << n * 10 << " ";
    });
    std::cout << '\n';

    // ═══════════════ GENERIC LAMBDA (C++14) ═══════════════
    std::cout << "\n=== GENERIC LAMBDA (C++14) ===\n";

    auto generic_add = [](auto a, auto b) { return a + b; };
    std::cout << "int:    " << generic_add(3, 4) << '\n';
    std::cout << "double: " << generic_add(3.14, 2.71) << '\n';
    std::cout << "string: " << generic_add(std::string("Hello "), std::string("World")) << '\n';

    // ═══════════════ STORING LAMBDAS ═══════════════
    std::cout << "\n=== std::function ===\n";

    // std::function can store any callable (lambda, function pointer, functor)
    std::function<int(int, int)> operation;

    operation = [](int a, int b) { return a + b; };
    std::cout << "Add: " << operation(5, 3) << '\n';

    operation = [](int a, int b) { return a * b; };
    std::cout << "Mul: " << operation(5, 3) << '\n';

    // ═══════════════ IMMEDIATELY INVOKED LAMBDA (IIFE) ═══════════════
    std::cout << "\n=== IIFE ===\n";
    const int result = [](int n) {
        int sum = 0;
        for (int i = 1; i <= n; i++) sum += i;
        return sum;
    }(10);  // Immediately called with argument 10
    std::cout << "Sum 1..10 = " << result << '\n';

    return 0;
}
