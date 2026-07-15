/*******************************************************************************
 * 01_auto_decltype.cpp — Type Inference (C++11)
 *******************************************************************************
 *
 *  auto:     Compiler deduces type from initializer
 *  decltype: Get the type of an expression without evaluating it
 *
 *  ┌────────────────────────────────────────────────────────┐
 *  │  auto x = 42;         // x is int                      │
 *  │  auto y = 3.14;       // y is double                   │
 *  │  auto z = "hello";    // z is const char*              │
 *  │  auto s = std::string("hi"); // s is std::string       │
 *  │                                                         │
 *  │  decltype(x) a = 10;  // a is int (same type as x)    │
 *  │  decltype(x + y) b;   // b is double (type of x + y)  │
 *  └────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++14 -o auto_decl 01_auto_decltype.cpp
 ******************************************************************************/

#include <iostream>
#include <vector>
#include <map>
#include <string>

// ─── Trailing return type ───
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {  // Return type is type of (a+b)
    return a + b;
}

// C++14: can just use auto without trailing return type
// auto add(T a, U b) { return a + b; }

int main() {
    // ═══════════════ AUTO ═══════════════
    std::cout << "=== AUTO ===\n";

    auto i = 42;           // int
    auto d = 3.14;         // double
    auto c = 'A';          // char
    auto b = true;         // bool
    auto s = std::string("Hello");  // std::string
    auto p = &i;           // int*
    const auto ci = 100;   // const int

    std::cout << "i: " << i << " (int)\n";
    std::cout << "d: " << d << " (double)\n";
    std::cout << "s: " << s << " (string)\n";

    // Auto with containers — avoids verbose types!
    std::map<std::string, std::vector<int>> data;
    data["scores"] = {90, 85, 92};

    // WITHOUT auto (verbose!):
    // std::map<std::string, std::vector<int>>::iterator it = data.begin();

    // WITH auto (clean!):
    auto it = data.begin();
    std::cout << "\nMap key: " << it->first << '\n';

    // Auto with range-for
    std::cout << "Scores: ";
    for (const auto& [key, vals] : data) {  // Structured bindings (C++17)
        for (auto v : vals) std::cout << v << " ";
    }
    std::cout << '\n';

    // ═══════════════ DECLTYPE ═══════════════
    std::cout << "\n=== DECLTYPE ===\n";

    int x = 10;
    decltype(x) y = 20;       // y is int (same type as x)
    decltype(x + d) z = 1.5;  // z is double (int + double = double)

    std::cout << "y (decltype(x)):     " << y << '\n';
    std::cout << "z (decltype(x+d)):   " << z << '\n';

    // ═══════════════ TRAILING RETURN TYPE ═══════════════
    std::cout << "\n=== TRAILING RETURN ===\n";
    std::cout << "add(3, 4.5) = " << add(3, 4.5) << '\n';

    /*
     * WHEN TO USE AUTO:
     * ✓ Iterator types (auto it = map.begin())
     * ✓ Lambda return types
     * ✓ Complex template types
     * ✓ Range-based for loops (const auto&)
     *
     * WHEN NOT TO USE:
     * ✗ When type isn't obvious from context
     * ✗ When you want to be explicit about type
     * ✗ auto x = getValue(); — reader can't tell the type
     */

    return 0;
}
