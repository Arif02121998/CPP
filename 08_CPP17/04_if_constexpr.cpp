/*******************************************************************************
 * 04_if_constexpr.cpp — Compile-time if, nested namespaces, attributes, CTAD
 *******************************************************************************
 *
 *  C++17 language features that make code cleaner and more expressive:
 *
 *   ┌─────────────────────────────┬────────────────────────────────────────┐
 *   │ Feature                     │ What it does                           │
 *   ├─────────────────────────────┼────────────────────────────────────────┤
 *   │ if constexpr                │ Compile-time branch elimination        │
 *   │ Nested namespaces           │ namespace A::B::C { }                  │
 *   │ [[nodiscard]]               │ Warn if return value ignored           │
 *   │ [[maybe_unused]]            │ Suppress unused-variable warnings      │
 *   │ [[fallthrough]]             │ Intentional switch fallthrough         │
 *   │ Fold expressions            │ Expand parameter packs with operators  │
 *   │ CTAD                        │ Deduce template args from constructor  │
 *   │ Inline variables            │ Define variables in headers            │
 *   └─────────────────────────────┴────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o if_constexpr 04_if_constexpr.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
#include <tuple>
#include <mutex>

// ═══════════════════════════════════════════════════════════════
//  1. if constexpr — Compile-time branching
// ═══════════════════════════════════════════════════════════════

/*
 *  Before C++17: SFINAE, enable_if, tag dispatch → complicated!
 *  C++17: if constexpr — dead branches are DISCARDED at compile time,
 *         so they don't even need to be valid for the given type.
 *
 *   ┌──────────────────────────────────┐
 *   │ if constexpr (is_integral<T>)    │
 *   │   → compiles ONLY this branch    │
 *   │ else                             │
 *   │   → this branch is DISCARDED     │
 *   └──────────────────────────────────┘
 */

template <typename T>
std::string describe(T value) {
    if constexpr (std::is_integral_v<T>) {
        return "Integer: " + std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "Float: " + std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "String: " + value;
    } else {
        return "Unknown type";
    }
}

// Practical: serialize different types
template <typename T>
void process(T val) {
    if constexpr (std::is_pointer_v<T>) {
        if (val) std::cout << "Pointer to: " << *val << '\n';
        else     std::cout << "Null pointer\n";
    } else {
        std::cout << "Value: " << val << '\n';
    }
}

void demo_if_constexpr() {
    std::cout << "═══ if constexpr ═══\n";
    std::cout << describe(42) << '\n';
    std::cout << describe(3.14) << '\n';
    std::cout << describe(std::string("hello")) << '\n';

    int x = 10;
    process(42);
    process(&x);
    process(3.14);
}


// ═══════════════════════════════════════════════════════════════
//  2. Nested namespaces
// ═══════════════════════════════════════════════════════════════

// Before C++17:
// namespace A { namespace B { namespace C { ... } } }

// C++17:
namespace App::Math::Geometry {
    double circleArea(double r) { return 3.14159 * r * r; }
}

void demo_nested_namespace() {
    std::cout << "\n═══ NESTED NAMESPACES ═══\n";
    std::cout << "Circle area (r=5): " << App::Math::Geometry::circleArea(5) << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Attributes: [[nodiscard]], [[maybe_unused]], [[fallthrough]]
// ═══════════════════════════════════════════════════════════════

[[nodiscard]] int computeImportant() {
    return 42;
}

void demo_attributes() {
    std::cout << "\n═══ ATTRIBUTES ═══\n";

    // [[nodiscard]] — compiler warns if you ignore the return value
    int result = computeImportant();  // OK
    // computeImportant();            // WARNING: discarding return value!
    std::cout << "[[nodiscard]] result: " << result << '\n';

    // [[maybe_unused]] — suppress "unused variable" warning
    [[maybe_unused]] int debugFlag = 1;

    // [[fallthrough]] — intentional switch fallthrough
    int level = 2;
    switch (level) {
        case 3:
            std::cout << "Premium features\n";
            [[fallthrough]];
        case 2:
            std::cout << "Standard features\n";
            [[fallthrough]];
        case 1:
            std::cout << "Basic features\n";
            break;
        default:
            std::cout << "No features\n";
    }
}


// ═══════════════════════════════════════════════════════════════
//  4. Fold expressions (C++17 — extends variadic templates)
// ═══════════════════════════════════════════════════════════════

/*
 *   (args op ...)     → Right fold: a1 op (a2 op (a3 op a4))
 *   (... op args)     → Left fold:  ((a1 op a2) op a3) op a4
 *   (args op ... op init)  → Right fold with init
 *   (init op ... op args)  → Left fold with init
 */

template <typename... Args>
auto sum(Args... args) {
    return (args + ...);  // Right fold: a + (b + (c + d))
}

template <typename... Args>
void printAll(Args... args) {
    ((std::cout << args << " "), ...);  // Left fold with comma operator
    std::cout << '\n';
}

template <typename... Args>
bool allTrue(Args... args) {
    return (args && ...);  // Logical AND fold
}

void demo_fold() {
    std::cout << "\n═══ FOLD EXPRESSIONS ═══\n";
    std::cout << "sum(1,2,3,4): " << sum(1, 2, 3, 4) << '\n';
    std::cout << "printAll: ";
    printAll(1, "hello", 3.14, 'x');
    std::cout << "allTrue(1,1,1): " << allTrue(true, true, true) << '\n';
    std::cout << "allTrue(1,0,1): " << allTrue(true, false, true) << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  5. Class Template Argument Deduction (CTAD)
// ═══════════════════════════════════════════════════════════════

void demo_ctad() {
    std::cout << "\n═══ CTAD ═══\n";

    // Before C++17:   std::pair<int, double> p(1, 2.5);
    // C++17:          std::pair p(1, 2.5);  — deduced!

    std::pair p(1, 2.5);             // pair<int, double>
    std::tuple t(1, "hello", 3.14); // tuple<int, const char*, double>
    std::vector v{1, 2, 3, 4};      // vector<int>

    std::cout << "pair: " << p.first << ", " << p.second << '\n';
    std::cout << "vector size: " << v.size() << '\n';

    // lock_guard deduction
    std::mutex mtx;
    {
        std::lock_guard lock(mtx);  // deduces std::lock_guard<std::mutex>
        std::cout << "Lock acquired with CTAD\n";
    }
}


// ═══════════════════════════════════════════════════════════════
//  6. Inline variables (define in header, one definition rule OK)
// ═══════════════════════════════════════════════════════════════

/*
 *  Before C++17: constexpr static members needed out-of-class definition
 *  C++17: inline variables — define once in headers
 *
 *  // In a header file:
 *  inline constexpr int MAX_SIZE = 1024;
 *  inline const std::string APP_NAME = "MyApp";
 *
 *  struct Config {
 *      static inline int timeout = 30;  // No .cpp definition needed!
 *  };
 */

struct Config {
    static inline int timeout = 30;
    static inline std::string name = "MyApp";
};

void demo_inline_var() {
    std::cout << "\n═══ INLINE VARIABLES ═══\n";
    std::cout << "Config::timeout = " << Config::timeout << '\n';
    std::cout << "Config::name    = " << Config::name << '\n';
}


int main() {
    demo_if_constexpr();
    demo_nested_namespace();
    demo_attributes();
    demo_fold();
    demo_ctad();
    demo_inline_var();
    return 0;
}
