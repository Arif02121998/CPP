/*******************************************************************************
 * 03_type_traits_metaprogramming.cpp — Type traits, SFINAE, compile-time logic
 *******************************************************************************
 *
 *  Type traits let you query and transform types AT COMPILE TIME.
 *
 *   ┌────────────────────────────────────────────────────────────────┐
 *   │  Category         │  Examples                                   │
 *   ├───────────────────┼────────────────────────────────────────────┤
 *   │  Type queries      │  is_integral, is_pointer, is_class         │
 *   │  Type relations    │  is_same, is_base_of, is_convertible       │
 *   │  Type transforms   │  remove_const, add_pointer, decay          │
 *   │  Property queries  │  is_nothrow_move_constructible             │
 *   └───────────────────┴────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o type_traits 03_type_traits_metaprogramming.cpp
 ******************************************************************************/

#include <iostream>
#include <type_traits>
#include <string>
#include <vector>
#include <concepts>

// ═══════════════════════════════════════════════════════════════
//  1. Type queries — What IS this type?
// ═══════════════════════════════════════════════════════════════

template <typename T>
void inspectType(const char* name) {
    std::cout << name << ":\n";
    std::cout << "  integral:       " << std::is_integral_v<T> << '\n';
    std::cout << "  floating_point: " << std::is_floating_point_v<T> << '\n';
    std::cout << "  pointer:        " << std::is_pointer_v<T> << '\n';
    std::cout << "  class:          " << std::is_class_v<T> << '\n';
    std::cout << "  const:          " << std::is_const_v<T> << '\n';
    std::cout << "  reference:      " << std::is_reference_v<T> << '\n';
}

void demo_type_queries() {
    std::cout << "═══ TYPE QUERIES ═══\n";
    inspectType<int>("int");
    inspectType<double>("double");
    inspectType<std::string>("string");
    inspectType<int*>("int*");
    inspectType<const int>("const int");
}


// ═══════════════════════════════════════════════════════════════
//  2. Type relations — How do types relate?
// ═══════════════════════════════════════════════════════════════

class Base {};
class Derived : public Base {};

void demo_type_relations() {
    std::cout << "\n═══ TYPE RELATIONS ═══\n";

    std::cout << "is_same<int, int>:        " << std::is_same_v<int, int> << '\n';
    std::cout << "is_same<int, long>:       " << std::is_same_v<int, long> << '\n';
    std::cout << "is_base_of<Base, Derived>: " << std::is_base_of_v<Base, Derived> << '\n';
    std::cout << "is_convertible<int, double>:" << std::is_convertible_v<int, double> << '\n';
    std::cout << "is_convertible<int*, double*>:" << std::is_convertible_v<int*, double*> << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Type transforms — Modify types at compile time
// ═══════════════════════════════════════════════════════════════

/*
 *  remove_const_t<const int>     → int
 *  remove_reference_t<int&>      → int
 *  add_pointer_t<int>            → int*
 *  decay_t<const int&>           → int
 *  make_signed_t<unsigned int>   → int
 *  conditional_t<true, A, B>     → A
 */

void demo_type_transforms() {
    std::cout << "\n═══ TYPE TRANSFORMS ═══\n";

    using T1 = std::remove_const_t<const int>;
    using T2 = std::remove_reference_t<int&>;
    using T3 = std::add_pointer_t<double>;
    using T4 = std::decay_t<const int(&)[5]>;  // array decays to pointer

    std::cout << "remove_const<const int>:     is int? " << std::is_same_v<T1, int> << '\n';
    std::cout << "remove_reference<int&>:      is int? " << std::is_same_v<T2, int> << '\n';
    std::cout << "add_pointer<double>:         is double*? " << std::is_same_v<T3, double*> << '\n';
    std::cout << "decay<const int(&)[5]>:      is const int*? " << std::is_same_v<T4, const int*> << '\n';

    // conditional: compile-time ternary for types
    using Big = std::conditional_t<(sizeof(int) > 4), int, long>;
    std::cout << "conditional: chose type with size " << sizeof(Big) << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. SFINAE — Substitution Failure Is Not An Error
// ═══════════════════════════════════════════════════════════════

/*
 *  SFINAE: If substituting template args produces an invalid type,
 *          the overload is REMOVED (not an error). Other overloads tried.
 *
 *  enable_if<condition, Type>: Only participates if condition is true.
 *
 *  Note: C++20 concepts are the MODERN replacement for SFINAE!
 */

// enable_if approach (pre-C++20)
template <typename T>
std::enable_if_t<std::is_integral_v<T>, std::string>
classifyOld(T) {
    return "integer";
}

template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::string>
classifyOld(T) {
    return "floating point";
}

// C++20 approach (MUCH cleaner!)
template <std::integral T>
std::string classifyNew(T) { return "integer"; }

template <std::floating_point T>
std::string classifyNew(T) { return "floating point"; }

void demo_sfinae() {
    std::cout << "\n═══ SFINAE vs CONCEPTS ═══\n";

    // SFINAE (old way)
    std::cout << "SFINAE   classifyOld(42):   " << classifyOld(42) << '\n';
    std::cout << "SFINAE   classifyOld(3.14): " << classifyOld(3.14) << '\n';

    // Concepts (new way — same result, cleaner code)
    std::cout << "Concept  classifyNew(42):   " << classifyNew(42) << '\n';
    std::cout << "Concept  classifyNew(3.14): " << classifyNew(3.14) << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  5. Tag dispatch — Compile-time function selection
// ═══════════════════════════════════════════════════════════════

/*
 *  Tag dispatch uses empty structs as "tags" to select overloads.
 *  The iterator category tags in <iterator> are a classic example.
 */

namespace detail {
    template <typename Iter>
    void advanceImpl(Iter& it, int n, std::random_access_iterator_tag) {
        std::cout << "  Random access: it += n (O(1))\n";
        it += n;
    }

    template <typename Iter>
    void advanceImpl(Iter& it, int n, std::input_iterator_tag) {
        std::cout << "  Input iterator: ++it n times (O(n))\n";
        for (int i = 0; i < n; ++i) ++it;
    }
}

template <typename Iter>
void myAdvance(Iter& it, int n) {
    detail::advanceImpl(it, n, typename std::iterator_traits<Iter>::iterator_category{});
}

void demo_tag_dispatch() {
    std::cout << "\n═══ TAG DISPATCH ═══\n";

    std::vector<int> v{1, 2, 3, 4, 5};
    auto it = v.begin();
    myAdvance(it, 3);
    std::cout << "  Value after advance: " << *it << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  6. Compile-time computation with constexpr
// ═══════════════════════════════════════════════════════════════

// constexpr: compute at compile time when possible
constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// Compile-time string length
constexpr size_t strLen(const char* s) {
    size_t len = 0;
    while (s[len]) ++len;
    return len;
}

void demo_constexpr_computation() {
    std::cout << "\n═══ COMPILE-TIME COMPUTATION ═══\n";

    // All computed at compile time
    constexpr int fib10 = fibonacci(10);
    constexpr size_t len = strLen("Hello, World!");

    static_assert(fib10 == 55);
    static_assert(len == 13);

    std::cout << "fibonacci(10) = " << fib10 << " (compile-time)\n";
    std::cout << "strLen(\"Hello, World!\") = " << len << " (compile-time)\n";

    // static_assert: compile-time assertion
    static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");
    static_assert(std::is_trivially_copyable_v<int>);
    std::cout << "All static_asserts passed!\n";
}


int main() {
    demo_type_queries();
    demo_type_relations();
    demo_type_transforms();
    demo_sfinae();
    demo_tag_dispatch();
    demo_constexpr_computation();
    return 0;
}
