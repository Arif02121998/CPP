/*******************************************************************************
 * 05_modern_syntax.cpp — nullptr, enum class, constexpr, uniform init
 *******************************************************************************
 * COMPILE: g++ -std=c++17 -o modern 05_modern_syntax.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <array>
#include <initializer_list>

// ═════════════════════════════════════════════════════════════════
// 1. NULLPTR (replaces NULL)
// ═════════════════════════════════════════════════════════════════
void foo(int) { std::cout << "  foo(int)\n"; }
void foo(int*) { std::cout << "  foo(int*)\n"; }

// ═════════════════════════════════════════════════════════════════
// 2. SCOPED ENUM (enum class) — Type-safe enumerations
// ═════════════════════════════════════════════════════════════════

// OLD: unscoped enum (pollutes namespace, implicit conversion)
// enum Color { RED, GREEN, BLUE };  // RED is in global scope!

// NEW: scoped enum (type-safe, no implicit conversion)
enum class Color { Red, Green, Blue };
enum class TrafficLight { Red, Yellow, Green };  // No conflict with Color::Red!

// Can specify underlying type
enum class ErrorCode : uint8_t { OK = 0, NotFound = 1, Timeout = 2 };

// ═════════════════════════════════════════════════════════════════
// 3. CONSTEXPR — Compile-time evaluation
// ═════════════════════════════════════════════════════════════════

// constexpr function — evaluated at compile time if possible
constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// ═════════════════════════════════════════════════════════════════
// 4. INITIALIZER_LIST
// ═════════════════════════════════════════════════════════════════
class IntList {
    std::vector<int> data_;
public:
    IntList(std::initializer_list<int> init) : data_(init) {
        std::cout << "  Created list with " << data_.size() << " elements\n";
    }
    void print() const {
        for (int x : data_) std::cout << x << " ";
        std::cout << '\n';
    }
};

// ═════════════════════════════════════════════════════════════════
// 5. STATIC_ASSERT — Compile-time assertions
// ═════════════════════════════════════════════════════════════════
static_assert(sizeof(int) == 4, "int must be 4 bytes");
static_assert(sizeof(void*) == 8, "Must be 64-bit platform");

int main() {
    // ─── nullptr ───
    std::cout << "=== NULLPTR ===\n";
    int* p = nullptr;  // Type-safe null pointer
    // foo(NULL);  // Ambiguous! NULL is just 0 (int)
    foo(nullptr);  // Calls foo(int*) — correct!

    // ─── enum class ───
    std::cout << "\n=== ENUM CLASS ===\n";
    Color c = Color::Red;
    // int x = c;  // ERROR! No implicit conversion
    int x = static_cast<int>(c);  // Explicit OK
    std::cout << "Color::Red = " << x << '\n';

    if (c == Color::Red) {
        std::cout << "It's red!\n";
    }

    // ─── constexpr ───
    std::cout << "\n=== CONSTEXPR ===\n";
    constexpr int fact5 = factorial(5);    // Computed at COMPILE TIME
    constexpr int fib10 = fibonacci(10);

    std::cout << "5! = " << fact5 << " (compile-time)\n";
    std::cout << "fib(10) = " << fib10 << " (compile-time)\n";

    // Can also use at runtime
    int n = 7;
    int fact_n = factorial(n);  // Computed at runtime
    std::cout << "7! = " << fact_n << " (runtime)\n";

    constexpr double pi = 3.14159265;
    // constexpr replaces many uses of #define

    // ─── Uniform Initialization {} ───
    std::cout << "\n=== UNIFORM INITIALIZATION ===\n";
    int a{42};                     // Direct initialization
    double b{3.14};
    std::string s{"Hello"};
    std::vector<int> v{1, 2, 3, 4, 5};
    std::array<int, 3> arr{10, 20, 30};

    // Prevents narrowing conversions!
    // int bad{3.14};  // ERROR! narrowing double → int
    int ok = 3.14;     // Silently truncates (dangerous!)

    std::cout << "a=" << a << ", b=" << b << ", s=" << s << '\n';

    // ─── Initializer list ───
    std::cout << "\n=== INITIALIZER_LIST ===\n";
    IntList list{10, 20, 30, 40, 50};
    list.print();

    // ─── Type aliases (using instead of typedef) ───
    std::cout << "\n=== TYPE ALIASES ===\n";
    using StringVec = std::vector<std::string>;
    // Same as: typedef std::vector<std::string> StringVec;

    StringVec names{"Alice", "Bob", "Charlie"};
    for (const auto& name : names) std::cout << name << " ";
    std::cout << '\n';

    // ─── Range-based for ───
    std::cout << "\n=== RANGE-BASED FOR ===\n";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << '\n';

    return 0;
}
