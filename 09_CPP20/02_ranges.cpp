/*******************************************************************************
 * 02_ranges.cpp — Ranges & Views: Composable, lazy pipelines for sequences
 *******************************************************************************
 *
 *  Before C++20:  std::sort(v.begin(), v.end());    // always begin/end pairs
 *  C++20:         std::ranges::sort(v);             // pass the range directly!
 *
 *   ┌─────────────────────────────────────────────────────────────────┐
 *   │             Ranges Pipeline (lazy, composable)                  │
 *   │                                                                 │
 *   │  data | filter(pred) | transform(func) | take(n)               │
 *   │    ↓        ↓              ↓                ↓                   │
 *   │  [1..10] → [even] → [squared] → [first 3]                      │
 *   │                                                                 │
 *   │  Nothing is computed until you iterate — LAZY!                  │
 *   └─────────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o ranges 02_ranges.cpp
 ******************************************************************************/

#include <iostream>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>

namespace views = std::views;
namespace ranges = std::ranges;

// ═══════════════════════════════════════════════════════════════
//  1. Range-based algorithms (no more begin/end pairs!)
// ═══════════════════════════════════════════════════════════════

void demo_range_algorithms() {
    std::cout << "═══ RANGE ALGORITHMS ═══\n";

    std::vector<int> v{5, 3, 1, 4, 2};

    // OLD: std::sort(v.begin(), v.end());
    // NEW:
    ranges::sort(v);
    std::cout << "sorted: ";
    for (auto x : v) std::cout << x << " ";
    std::cout << '\n';

    // find
    auto it = ranges::find(v, 3);
    if (it != v.end()) std::cout << "found: " << *it << '\n';

    // min/max
    std::cout << "min: " << *ranges::min_element(v) << '\n';
    std::cout << "max: " << *ranges::max_element(v) << '\n';

    // count_if
    auto even_count = ranges::count_if(v, [](int x) { return x % 2 == 0; });
    std::cout << "even count: " << even_count << '\n';

    // all_of, any_of, none_of
    std::cout << "all positive? " << ranges::all_of(v, [](int x){ return x > 0; }) << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  2. Views — lazy, non-owning transformations
// ═══════════════════════════════════════════════════════════════

/*
 *  KEY INSIGHT: Views are LAZY.
 *  They don't copy data or compute anything until iterated.
 *
 *   auto view = v | views::filter(even) | views::transform(square);
 *   // Nothing happened yet!
 *   for (auto x : view) { ... }  // NOW it computes, element by element
 */

void demo_views() {
    std::cout << "\n═══ VIEWS (Lazy Pipelines) ═══\n";

    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // filter — keep only matching elements
    std::cout << "even: ";
    for (auto x : v | views::filter([](int x){ return x % 2 == 0; })) {
        std::cout << x << " ";
    }
    std::cout << '\n';

    // transform — map each element
    std::cout << "squared: ";
    for (auto x : v | views::transform([](int x){ return x * x; })) {
        std::cout << x << " ";
    }
    std::cout << '\n';

    // take — first N elements
    std::cout << "first 3: ";
    for (auto x : v | views::take(3)) {
        std::cout << x << " ";
    }
    std::cout << '\n';

    // drop — skip first N elements
    std::cout << "skip 7: ";
    for (auto x : v | views::drop(7)) {
        std::cout << x << " ";
    }
    std::cout << '\n';

    // reverse
    std::cout << "reverse: ";
    for (auto x : v | views::reverse) {
        std::cout << x << " ";
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Chaining views — the POWER of ranges
// ═══════════════════════════════════════════════════════════════

void demo_chaining() {
    std::cout << "\n═══ VIEW CHAINING ═══\n";

    std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    // Pipeline: even numbers → squared → first 4
    auto pipeline = data
        | views::filter([](int x) { return x % 2 == 0; })
        | views::transform([](int x) { return x * x; })
        | views::take(4);

    std::cout << "even → squared → first 4: ";
    for (auto x : pipeline) {
        std::cout << x << " ";  // 4 16 36 64
    }
    std::cout << '\n';

    // Stored pipeline can be reused
    auto evens = views::filter([](int x) { return x % 2 == 0; });
    auto squares = views::transform([](int x) { return x * x; });

    std::cout << "reusable pipeline: ";
    for (auto x : data | evens | squares | views::take(3)) {
        std::cout << x << " ";
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. views::iota — generate sequences
// ═══════════════════════════════════════════════════════════════

void demo_iota() {
    std::cout << "\n═══ views::iota (Sequence Generator) ═══\n";

    // Bounded: iota(start, end) → [start, end)
    std::cout << "1..10: ";
    for (auto x : views::iota(1, 11)) {
        std::cout << x << " ";
    }
    std::cout << '\n';

    // With pipeline: first 5 squares
    std::cout << "first 5 squares: ";
    for (auto x : views::iota(1)
                  | views::transform([](int x){ return x * x; })
                  | views::take(5)) {
        std::cout << x << " ";
    }
    std::cout << '\n';

    // FizzBuzz with ranges!
    std::cout << "FizzBuzz 1-15:\n  ";
    for (auto i : views::iota(1, 16)) {
        if (i % 15 == 0) std::cout << "FizzBuzz ";
        else if (i % 3 == 0) std::cout << "Fizz ";
        else if (i % 5 == 0) std::cout << "Buzz ";
        else std::cout << i << " ";
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  5. Projections — sort/search by a member
// ═══════════════════════════════════════════════════════════════

struct Student {
    std::string name;
    int grade;
};

void demo_projections() {
    std::cout << "\n═══ PROJECTIONS ═══\n";

    std::vector<Student> students = {
        {"Alice", 92}, {"Bob", 85}, {"Charlie", 98}, {"Diana", 88}
    };

    // Sort by grade (projection!)
    // OLD: ranges::sort(students, [](auto& a, auto& b){ return a.grade < b.grade; });
    // NEW: projection does it cleanly
    ranges::sort(students, {}, &Student::grade);

    std::cout << "Sorted by grade:\n";
    for (const auto& s : students) {
        std::cout << "  " << s.name << ": " << s.grade << '\n';
    }

    // Sort by name
    ranges::sort(students, {}, &Student::name);
    std::cout << "Sorted by name:\n";
    for (const auto& s : students) {
        std::cout << "  " << s.name << ": " << s.grade << '\n';
    }

    // Find by projection
    auto it = ranges::find(students, 92, &Student::grade);
    if (it != students.end()) {
        std::cout << "Grade 92: " << it->name << '\n';
    }
}


// ═══════════════════════════════════════════════════════════════
//  6. keys / values views for maps
// ═══════════════════════════════════════════════════════════════

void demo_keys_values() {
    std::cout << "\n═══ keys/values VIEWS ═══\n";

    std::vector<std::pair<std::string, int>> data = {
        {"Alice", 95}, {"Bob", 87}, {"Charlie", 92}
    };

    std::cout << "keys: ";
    for (auto& k : data | views::keys) {
        std::cout << k << " ";
    }
    std::cout << '\n';

    std::cout << "values: ";
    for (auto v : data | views::values) {
        std::cout << v << " ";
    }
    std::cout << '\n';
}


int main() {
    demo_range_algorithms();
    demo_views();
    demo_chaining();
    demo_iota();
    demo_projections();
    demo_keys_values();
    return 0;
}
