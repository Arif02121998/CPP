/*******************************************************************************
 * 03_string_view.cpp — Non-owning, read-only reference to a string
 *******************************************************************************
 *
 *  Problem: Passing std::string everywhere causes unnecessary copying.
 *
 *   ┌──────────────────────────────────────────────────┐
 *   │  std::string  "Hello, World!"                     │  ← owns the data
 *   └──────────┬───────────────────────────────────────┘
 *              │
 *   ┌──────────▼────────────┐
 *   │  string_view           │  ← just a pointer + size
 *   │  ptr ──► 'H'           │     (NO copy, NO allocation)
 *   │  size = 13             │
 *   └───────────────────────┘
 *
 *  ┌───────────────┬─────────────┬────────────────────────┐
 *  │               │ std::string │ std::string_view        │
 *  ├───────────────┼─────────────┼────────────────────────┤
 *  │ Owns data?    │    YES      │   NO                    │
 *  │ Allocates?    │    YES      │   NO                    │
 *  │ Modifiable?   │    YES      │   NO (read-only)        │
 *  │ Null-term?    │    YES      │   Not guaranteed        │
 *  │ From const c* │  Copy made  │   Zero-cost view        │
 *  └───────────────┴─────────────┴────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o string_view 03_string_view.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// ═══════════════════════════════════════════════════════════════
//  1. Basic usage — zero-cost abstraction over strings
// ═══════════════════════════════════════════════════════════════

// Accepts const char*, std::string, or string_view — no copy!
void printName(std::string_view name) {
    std::cout << "Name: " << name << " (length=" << name.size() << ")\n";
}

void demo_basic() {
    std::cout << "═══ BASIC USAGE ═══\n";

    // From different sources — all zero-copy
    std::string str = "Alice";
    const char* cstr = "Bob";

    printName(str);             // from std::string
    printName(cstr);            // from const char*
    printName("Charlie");       // from string literal
    printName({"DataXYZ", 4});  // from pointer + length → "Data"
}


// ═══════════════════════════════════════════════════════════════
//  2. Substrings without allocation
// ═══════════════════════════════════════════════════════════════

void demo_substr() {
    std::cout << "\n═══ SUBSTRING (No Allocation!) ═══\n";

    std::string_view sv = "Hello, Beautiful World!";

    // substr returns another string_view — NO heap allocation
    auto word = sv.substr(7, 9);  // "Beautiful"
    std::cout << "substr: " << word << '\n';

    // remove_prefix / remove_suffix — narrow the view
    auto trimmed = sv;
    trimmed.remove_prefix(7);   // "Beautiful World!"
    trimmed.remove_suffix(1);   // "Beautiful World"
    std::cout << "trimmed: " << trimmed << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Search and comparison
// ═══════════════════════════════════════════════════════════════

void demo_search() {
    std::cout << "\n═══ SEARCH & COMPARE ═══\n";

    std::string_view sv = "The quick brown fox jumps";

    std::cout << "find 'brown': " << sv.find("brown") << '\n';
    std::cout << "starts_with 'The': " << sv.starts_with("The") << '\n';   // C++20 but widely available
    std::cout << "ends_with 'jumps': " << sv.ends_with("jumps") << '\n';

    // Character access
    std::cout << "front: " << sv.front() << ", back: " << sv.back() << '\n';
    std::cout << "sv[4]: " << sv[4] << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. Practical: split a string without allocations
// ═══════════════════════════════════════════════════════════════

std::vector<std::string_view> split(std::string_view sv, char delim) {
    std::vector<std::string_view> result;
    while (!sv.empty()) {
        auto pos = sv.find(delim);
        if (pos == std::string_view::npos) {
            result.push_back(sv);
            break;
        }
        result.push_back(sv.substr(0, pos));
        sv.remove_prefix(pos + 1);
    }
    return result;
}

void demo_split() {
    std::cout << "\n═══ SPLIT (Zero-Allocation) ═══\n";

    std::string data = "apple,banana,cherry,date,elderberry";
    auto parts = split(data, ',');

    for (auto part : parts) {
        std::cout << "[" << part << "] ";
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  5. DANGER: Lifetime issues
// ═══════════════════════════════════════════════════════════════

/*
 *  RULE: string_view does NOT own the data.
 *        The underlying string must outlive the view!
 *
 *   DANGEROUS:
 *     std::string_view dangling() {
 *         std::string temp = "hello";
 *         return temp;  // ❌ temp destroyed, view dangles!
 *     }
 *
 *   SAFE:
 *     std::string_view safe(const std::string& s) {
 *         return s;  // ✓ caller owns the string
 *     }
 */

void demo_lifetime_warning() {
    std::cout << "\n═══ LIFETIME WARNING ═══\n";

    // SAFE: literal has static duration
    std::string_view sv1 = "I live forever";
    std::cout << "literal view: " << sv1 << '\n';

    // SAFE: string outlives the view
    std::string str = "I live long enough";
    std::string_view sv2 = str;
    std::cout << "string view: " << sv2 << '\n';

    // DANGER: temporary
    // std::string_view sv3 = std::string("DANGER!");  // ❌ UB after this line
    std::cout << "(Commented out dangerous example — see source)\n";
}


int main() {
    demo_basic();
    demo_substr();
    demo_search();
    demo_split();
    demo_lifetime_warning();
    return 0;
}
