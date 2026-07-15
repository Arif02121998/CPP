/*******************************************************************************
 * 05_other_cpp20.cpp — std::span, designated initializers, consteval,
 *                       constinit, source_location, and more
 *******************************************************************************
 *
 *  Smaller but impactful C++20 features:
 *
 *   ┌───────────────────────┬──────────────────────────────────────────┐
 *   │ Feature               │ What it does                             │
 *   ├───────────────────────┼──────────────────────────────────────────┤
 *   │ std::span             │ Non-owning view of contiguous memory     │
 *   │ Designated init       │ .field = value syntax for structs        │
 *   │ consteval             │ Function MUST run at compile time        │
 *   │ constinit             │ Variable init MUST be at compile time    │
 *   │ source_location       │ __FILE__, __LINE__ replacement           │
 *   │ using enum            │ Import enum values into scope            │
 *   │ Template lambdas      │ []<typename T>(T x) { ... }             │
 *   │ Bit operations        │ popcount, countl_zero, etc.              │
 *   │ starts_with/ends_with │ String prefix/suffix checking            │
 *   │ contains (maps)       │ map.contains(key) instead of find!=end  │
 *   └───────────────────────┴──────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o other_cpp20 05_other_cpp20.cpp
 ******************************************************************************/

#include <iostream>
#include <span>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <source_location>
#include <bit>
#include <numeric>

// ═══════════════════════════════════════════════════════════════
//  1. std::span — Non-owning view of contiguous data
// ═══════════════════════════════════════════════════════════════

/*
 *  span is to arrays/vectors what string_view is to strings:
 *    - Non-owning, lightweight
 *    - Works with C arrays, std::array, std::vector
 *    - Can be fixed-size or dynamic-size
 *
 *   ┌─vector─────────────────┐
 *   │ 1 │ 2 │ 3 │ 4 │ 5 │   │
 *   └───┴───┴───┴───┴───┴───┘
 *         ▲───────────▲
 *   ┌─span──────────────────┐
 *   │ ptr=&2, size=3        │  ← just a pointer + size
 *   └──────────────────────┘
 */

// Accepts any contiguous container!
void printSpan(std::span<const int> data) {
    std::cout << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i) std::cout << ", ";
        std::cout << data[i];
    }
    std::cout << "] size=" << data.size() << '\n';
}

// Fixed-size span
void printThree(std::span<const int, 3> data) {
    std::cout << "fixed3: " << data[0] << ", " << data[1] << ", " << data[2] << '\n';
}

void demo_span() {
    std::cout << "═══ std::span ═══\n";

    // From various sources
    int carr[] = {1, 2, 3, 4, 5};
    std::vector<int> vec{10, 20, 30, 40, 50};
    std::array<int, 4> arr{100, 200, 300, 400};

    printSpan(carr);
    printSpan(vec);
    printSpan(arr);

    // Subspan
    std::span<int> full(vec);
    auto sub = full.subspan(1, 3);  // elements [20, 30, 40]
    std::cout << "subspan(1,3): ";
    printSpan(sub);

    // first / last
    std::cout << "first(2): ";
    printSpan(full.first(2));
    std::cout << "last(2): ";
    printSpan(full.last(2));

    // Fixed-size span
    int three[] = {7, 8, 9};
    printThree(three);

    // Modifying through span (it's a view, not const!)
    std::span<int> mutable_span(vec);
    mutable_span[0] = 999;
    std::cout << "after modify: vec[0]=" << vec[0] << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  2. Designated initializers
// ═══════════════════════════════════════════════════════════════

struct Config {
    int width = 800;
    int height = 600;
    bool fullscreen = false;
    std::string title = "Untitled";
};

void demo_designated_init() {
    std::cout << "\n═══ DESIGNATED INITIALIZERS ═══\n";

    // Name the fields you're initializing — clear and self-documenting!
    Config cfg{
        .width = 1920,
        .height = 1080,
        .fullscreen = true,
        .title = "My App"
    };

    std::cout << cfg.title << ": " << cfg.width << "x" << cfg.height
              << (cfg.fullscreen ? " fullscreen" : " windowed") << '\n';

    // Skip fields — they get default values
    Config minimal{.title = "Minimal"};
    std::cout << minimal.title << ": " << minimal.width << "x" << minimal.height << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. consteval — MUST run at compile time
// ═══════════════════════════════════════════════════════════════

/*
 *  constexpr:  CAN run at compile time (but also at runtime)
 *  consteval:  MUST run at compile time (error if called at runtime)
 *  constinit:  Variable initialization MUST be at compile time
 */

consteval int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}

consteval int square(int n) {
    return n * n;
}

// constinit: initialization must be compile-time, but variable is mutable
constinit int global_val = factorial(5);  // 120, computed at compile time

void demo_consteval() {
    std::cout << "\n═══ consteval & constinit ═══\n";

    constexpr int f5 = factorial(5);     // OK: compile time
    constexpr int s10 = square(10);      // OK: compile time

    std::cout << "factorial(5) = " << f5 << '\n';
    std::cout << "square(10) = " << s10 << '\n';

    // This would NOT compile:
    // int n = 5;
    // int f = factorial(n);  // ERROR: n is not constexpr

    // constinit: init at compile time, but can modify at runtime
    global_val += 1;
    std::cout << "constinit global_val = " << global_val << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. std::source_location — replaces __FILE__, __LINE__
// ═══════════════════════════════════════════════════════════════

void log_message(const std::string& msg,
                 const std::source_location loc = std::source_location::current()) {
    std::cout << loc.file_name() << ":" << loc.line()
              << " [" << loc.function_name() << "] " << msg << '\n';
}

void demo_source_location() {
    std::cout << "\n═══ source_location ═══\n";
    log_message("Starting demo");
    log_message("This is line info without macros!");
}


// ═══════════════════════════════════════════════════════════════
//  5. using enum — import enum values into scope
// ═══════════════════════════════════════════════════════════════

enum class Color { Red, Green, Blue };

std::string colorName(Color c) {
    using enum Color;  // All values now accessible without Color:: prefix!
    switch (c) {
        case Red:   return "Red";
        case Green: return "Green";
        case Blue:  return "Blue";
    }
    return "Unknown";
}

void demo_using_enum() {
    std::cout << "\n═══ using enum ═══\n";
    std::cout << colorName(Color::Red) << '\n';
    std::cout << colorName(Color::Blue) << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  6. Template lambdas
// ═══════════════════════════════════════════════════════════════

void demo_template_lambda() {
    std::cout << "\n═══ TEMPLATE LAMBDAS ═══\n";

    // C++14 generic: auto
    auto generic = [](auto x) { return x * 2; };

    // C++20 template: explicit type parameter
    auto typed = []<typename T>(std::vector<T>& v) {
        T sum = std::accumulate(v.begin(), v.end(), T{});
        std::cout << "sum = " << sum << " (size=" << v.size() << ")\n";
    };

    std::cout << "generic(5): " << generic(5) << '\n';
    std::cout << "generic(2.5): " << generic(2.5) << '\n';

    std::vector<int> vi{1, 2, 3, 4, 5};
    typed(vi);

    std::vector<double> vd{1.1, 2.2, 3.3};
    typed(vd);
}


// ═══════════════════════════════════════════════════════════════
//  7. String & container improvements
// ═══════════════════════════════════════════════════════════════

void demo_container_improvements() {
    std::cout << "\n═══ CONTAINER IMPROVEMENTS ═══\n";

    // starts_with / ends_with
    std::string filename = "report_2024.pdf";
    std::cout << "starts_with 'report': " << filename.starts_with("report") << '\n';
    std::cout << "ends_with '.pdf': " << filename.ends_with(".pdf") << '\n';

    // map::contains — no more find()!=end()!
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}};

    // Old: if (scores.find("Alice") != scores.end())
    // New:
    if (scores.contains("Alice")) {
        std::cout << "Alice found!\n";
    }
    if (!scores.contains("Charlie")) {
        std::cout << "Charlie not found\n";
    }
}


// ═══════════════════════════════════════════════════════════════
//  8. Bit operations (<bit>)
// ═══════════════════════════════════════════════════════════════

void demo_bit_ops() {
    std::cout << "\n═══ BIT OPERATIONS ═══\n";

    unsigned int val = 0b0011'0100;  // 52
    std::cout << "value: " << val << " (0b00110100)\n";
    std::cout << "popcount:    " << std::popcount(val) << " (set bits)\n";
    std::cout << "has_single_bit(64): " << std::has_single_bit(64u) << " (power of 2?)\n";
    std::cout << "has_single_bit(52): " << std::has_single_bit(52u) << '\n';
    std::cout << "bit_ceil(52):  " << std::bit_ceil(52u) << " (next power of 2)\n";
    std::cout << "bit_floor(52): " << std::bit_floor(52u) << " (prev power of 2)\n";
    std::cout << "countl_zero:   " << std::countl_zero(val) << " (leading zeros)\n";
}


int main() {
    demo_span();
    demo_designated_init();
    demo_consteval();
    demo_source_location();
    demo_using_enum();
    demo_template_lambda();
    demo_container_improvements();
    demo_bit_ops();
    return 0;
}
