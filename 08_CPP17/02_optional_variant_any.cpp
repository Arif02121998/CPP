/*******************************************************************************
 * 02_optional_variant_any.cpp — Type-safe alternatives to null / union / void*
 *******************************************************************************
 *
 *  ┌──────────────┬────────────────────┬────────────────────────────────────┐
 *  │   Type       │   Replaces         │   Purpose                          │
 *  ├──────────────┼────────────────────┼────────────────────────────────────┤
 *  │ optional<T>  │ T* / sentinel      │ "may or may not hold a value"      │
 *  │ variant<Ts>  │ union              │ "holds one of these types, safely" │
 *  │ any          │ void*              │ "holds any type, type-erased"      │
 *  └──────────────┴────────────────────┴────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o opt_var_any 02_optional_variant_any.cpp
 ******************************************************************************/

#include <iostream>
#include <optional>
#include <variant>
#include <any>
#include <string>
#include <vector>
#include <map>

// ═══════════════════════════════════════════════════════════════
//  1. std::optional — Nullable value without pointers
// ═══════════════════════════════════════════════════════════════

/*
 *   ┌─────────────────┐
 *   │  optional<int>   │
 *   ├─────────────────┤
 *   │ has_value: true  │   ← Engaged / Disengaged
 *   │ value:     42    │
 *   └─────────────────┘
 */

std::optional<int> findIndex(const std::vector<int>& v, int target) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == target) return static_cast<int>(i);
    }
    return std::nullopt;  // "no value"
}

void demo_optional() {
    std::cout << "═══ std::optional ═══\n";

    // Creating
    std::optional<int> a;               // empty
    std::optional<int> b = 42;          // engaged
    std::optional<std::string> c = "hello";

    // Checking
    if (a) std::cout << "a has value\n";
    if (b.has_value()) std::cout << "b = " << *b << '\n';

    // value_or — default if empty
    std::cout << "a value_or 0 = " << a.value_or(0) << '\n';

    // Practical use: find in vector
    std::vector<int> nums{10, 20, 30, 40};

    if (auto idx = findIndex(nums, 30); idx) {
        std::cout << "Found 30 at index " << *idx << '\n';
    }
    if (auto idx = findIndex(nums, 99); !idx) {
        std::cout << "99 not found (std::nullopt)\n";
    }

    // Reset
    b.reset();
    std::cout << "b after reset: " << (b ? "has value" : "empty") << '\n';

    // emplace — construct in-place
    b.emplace(100);
    std::cout << "b after emplace: " << *b << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  2. std::variant — Type-safe union
// ═══════════════════════════════════════════════════════════════

/*
 *  variant<int, double, string>
 *   ┌────────────────────────────┐
 *   │  index: 1                  │  ← which alternative is active
 *   │  storage: 3.14 (double)    │
 *   └────────────────────────────┘
 *
 *   Old C union:  UB if you read wrong member
 *   std::variant: throws bad_variant_access! Safe!
 */

// Visitor pattern for variant
struct PrintVisitor {
    void operator()(int i)                const { std::cout << "int: " << i << '\n'; }
    void operator()(double d)             const { std::cout << "double: " << d << '\n'; }
    void operator()(const std::string& s) const { std::cout << "string: " << s << '\n'; }
};

void demo_variant() {
    std::cout << "\n═══ std::variant ═══\n";

    std::variant<int, double, std::string> v;

    // Default: first type (int)
    std::cout << "default index: " << v.index() << '\n';

    v = 3.14;
    std::cout << "after = 3.14, index: " << v.index() << '\n';

    // get<T> — throws bad_variant_access if wrong type
    std::cout << "get<double>: " << std::get<double>(v) << '\n';

    // get_if<T> — returns pointer (nullptr if wrong type, SAFE)
    if (auto* p = std::get_if<double>(&v)) {
        std::cout << "get_if<double>: " << *p << '\n';
    }

    // holds_alternative<T>
    std::cout << "holds double? " << std::holds_alternative<double>(v) << '\n';
    std::cout << "holds int?    " << std::holds_alternative<int>(v) << '\n';

    // Visit — call the right overload automatically
    v = 42;
    std::visit(PrintVisitor{}, v);
    v = std::string("hello");
    std::visit(PrintVisitor{}, v);

    // Visit with generic lambda
    v = 3.14;
    std::visit([](const auto& val) {
        std::cout << "generic visit: " << val << '\n';
    }, v);

    // Practical: heterogeneous list
    std::cout << "\n--- Heterogeneous list ---\n";
    using Value = std::variant<int, double, std::string>;
    std::vector<Value> data = {10, 2.5, std::string("text"), 42};
    for (const auto& item : data) {
        std::visit([](const auto& val) { std::cout << val << " "; }, item);
    }
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. std::any — Type-erased container (replaces void*)
// ═══════════════════════════════════════════════════════════════

void demo_any() {
    std::cout << "\n═══ std::any ═══\n";

    std::any a;
    std::cout << "empty: " << (a.has_value() ? "no" : "yes") << '\n';

    a = 42;
    std::cout << "type: " << a.type().name() << ", value: " << std::any_cast<int>(a) << '\n';

    a = std::string("hello");
    std::cout << "type: " << a.type().name() << ", value: " << std::any_cast<std::string>(a) << '\n';

    a = 3.14;
    std::cout << "type: " << a.type().name() << ", value: " << std::any_cast<double>(a) << '\n';

    // Safe cast — returns nullptr for pointer overload
    if (auto* p = std::any_cast<int>(&a)) {
        std::cout << "cast to int: " << *p << '\n';
    } else {
        std::cout << "Not an int\n";
    }

    // Reset
    a.reset();
    std::cout << "after reset has_value: " << a.has_value() << '\n';

    // Practical: property map
    std::cout << "\n--- Property Map ---\n";
    std::map<std::string, std::any> config;
    config["width"]  = 1920;
    config["height"] = 1080;
    config["title"]  = std::string("My App");
    config["ratio"]  = 16.0 / 9.0;

    std::cout << "width:  " << std::any_cast<int>(config["width"]) << '\n';
    std::cout << "title:  " << std::any_cast<std::string>(config["title"]) << '\n';
    std::cout << "ratio:  " << std::any_cast<double>(config["ratio"]) << '\n';
}


int main() {
    demo_optional();
    demo_variant();
    demo_any();
    return 0;
}
