/*******************************************************************************
 * 01_concepts.cpp — Concepts & Constraints: Readable template requirements
 *******************************************************************************
 *
 *  Before C++20: Template errors were HORRIBLE. Hundreds of lines of
 *  gibberish when you passed the wrong type.
 *
 *  C++20 Concepts: Name your requirements, get clear error messages!
 *
 *   ┌─────────────────────────────────────────────────────────────────┐
 *   │  template <typename T>              Before C++20               │
 *   │  requires std::integral<T>          (SFINAE / enable_if)       │
 *   │  T add(T a, T b) { return a+b; }                              │
 *   │                                                                 │
 *   │  add(1, 2);     → OK                                           │
 *   │  add(1.5, 2.5); → ERROR: "constraint not satisfied"            │
 *   │                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^              │
 *   │                    CLEAR message vs. pages of template errors!   │
 *   └─────────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o concepts 01_concepts.cpp
 ******************************************************************************/

#include <iostream>
#include <concepts>
#include <string>
#include <vector>
#include <type_traits>

// ═══════════════════════════════════════════════════════════════
//  1. Using standard library concepts
// ═══════════════════════════════════════════════════════════════

/*
 *  Standard concepts in <concepts>:
 *    std::integral        — int, long, char, bool, etc.
 *    std::floating_point  — float, double
 *    std::signed_integral — int, long (not unsigned)
 *    std::same_as<T, U>   — T and U are the same type
 *    std::convertible_to  — T can convert to U
 *    std::derived_from    — T derives from U
 *    std::copyable, std::movable, std::regular, etc.
 */

// SYNTAX 1: requires clause
template <typename T>
requires std::integral<T>
T gcd(T a, T b) {
    while (b != 0) {
        T temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// SYNTAX 2: Trailing requires
template <typename T>
T absolute(T val) requires std::signed_integral<T> {
    return val < 0 ? -val : val;
}

// SYNTAX 3: Constrained auto (shortest!)
void printIntegral(std::integral auto val) {
    std::cout << "Integral: " << val << '\n';
}

// SYNTAX 4: Concept in template parameter
template <std::floating_point T>
T divide(T a, T b) {
    return a / b;
}

void demo_standard_concepts() {
    std::cout << "═══ STANDARD CONCEPTS ═══\n";

    std::cout << "gcd(12, 8): " << gcd(12, 8) << '\n';
    std::cout << "absolute(-42): " << absolute(-42) << '\n';
    printIntegral(100);
    std::cout << "divide(10.0, 3.0): " << divide(10.0, 3.0) << '\n';

    // These would NOT compile (clear error messages!):
    // gcd(1.5, 2.5);       // error: constraints not satisfied (not integral)
    // absolute(3u);         // error: not signed_integral
    // printIntegral(3.14);  // error: not integral
}


// ═══════════════════════════════════════════════════════════════
//  2. Defining custom concepts
// ═══════════════════════════════════════════════════════════════

/*
 *  concept Name = <requires expression>;
 *
 *  requires { ... } lets you check:
 *    - Existence of operations
 *    - Return type of operations
 *    - Nested type requirements
 */

// Simple concept: type must support + and have a size() method
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;  // a+b must return T
};

// Container concept
template <typename C>
concept Container = requires(C c) {
    { c.size() } -> std::convertible_to<std::size_t>;
    { c.begin() };
    { c.end() };
    typename C::value_type;     // must have a value_type
};

// Printable concept
template <typename T>
concept Printable = requires(std::ostream& os, T val) {
    { os << val };  // must support << with ostream
};

// Use custom concepts
template <Addable T>
T add(T a, T b) {
    return a + b;
}

template <Container C>
void printContainer(const C& c) {
    std::cout << "[";
    bool first = true;
    for (const auto& elem : c) {
        if (!first) std::cout << ", ";
        std::cout << elem;
        first = false;
    }
    std::cout << "] (size=" << c.size() << ")\n";
}

void demo_custom_concepts() {
    std::cout << "\n═══ CUSTOM CONCEPTS ═══\n";

    std::cout << "add(3, 4): " << add(3, 4) << '\n';
    std::cout << "add(1.5, 2.5): " << add(1.5, 2.5) << '\n';

    std::vector<int> v{1, 2, 3, 4, 5};
    std::cout << "container: ";
    printContainer(v);
}


// ═══════════════════════════════════════════════════════════════
//  3. Compound requirements & concept composition
// ═══════════════════════════════════════════════════════════════

// Compose concepts with &&, ||
template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

// Multi-requirement concept
template <typename T>
concept Sortable = requires(T a, T b) {
    { a < b }  -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
    requires std::copyable<T>;
};

template <Number T>
T square(T val) {
    return val * val;
}

void demo_composition() {
    std::cout << "\n═══ CONCEPT COMPOSITION ═══\n";
    std::cout << "square(5): " << square(5) << '\n';
    std::cout << "square(2.5): " << square(2.5) << '\n';

    // Check concepts at compile time
    static_assert(Number<int>);
    static_assert(Number<double>);
    static_assert(!Number<std::string>);
    static_assert(Hashable<int>);
    static_assert(Hashable<std::string>);
    static_assert(Sortable<int>);

    std::cout << "All static_asserts passed!\n";
}


// ═══════════════════════════════════════════════════════════════
//  4. Concept overloading — best match wins
// ═══════════════════════════════════════════════════════════════

template <typename T>
std::string describe_type(T) { return "unknown"; }

template <std::integral T>
std::string describe_type(T) { return "integral"; }

template <std::signed_integral T>
std::string describe_type(T) { return "signed integral"; }  // More specific → wins

void demo_overloading() {
    std::cout << "\n═══ CONCEPT OVERLOADING ═══\n";
    std::cout << "int:      " << describe_type(42) << '\n';        // "signed integral"
    std::cout << "unsigned: " << describe_type(42u) << '\n';       // "integral"
    std::cout << "double:   " << describe_type(3.14) << '\n';      // "unknown"
    std::cout << "string:   " << describe_type(std::string("hi")) << '\n';  // "unknown"
}


int main() {
    demo_standard_concepts();
    demo_custom_concepts();
    demo_composition();
    demo_overloading();
    return 0;
}
