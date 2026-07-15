/*******************************************************************************
 * 05_move_forward_deep.cpp — Universal references, perfect forwarding,
 *                              reference collapsing rules
 *******************************************************************************
 *
 *  This is one of the most confusing parts of C++. Let's demystify it.
 *
 *  ┌──────────────────────────────────────────────────────────────────────┐
 *  │  CONCEPT MAP                                                         │
 *  │                                                                      │
 *  │  Value Categories:                                                   │
 *  │  ┌────────────────┐  ┌────────────────────┐                         │
 *  │  │  lvalue         │  │  rvalue              │                       │
 *  │  │  (has identity) │  │  (temporary/movable) │                       │
 *  │  │  int x = 5;     │  │  42, std::move(x)    │                       │
 *  │  │  &x is valid    │  │  &42 is NOT valid     │                       │
 *  │  └────────────────┘  └────────────────────┘                         │
 *  │                                                                      │
 *  │  Reference Types:                                                    │
 *  │  T&   → lvalue reference (binds to lvalues only)                    │
 *  │  T&&  → rvalue reference (binds to rvalues only)                    │
 *  │  T&&  → UNIVERSAL reference (when T is deduced template param)      │
 *  │         can bind to BOTH lvalues and rvalues!                        │
 *  └──────────────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o move_forward 05_move_forward_deep.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <type_traits>

// ═══════════════════════════════════════════════════════════════
//  1. Lvalue vs Rvalue — the foundation
// ═══════════════════════════════════════════════════════════════

/*
 *   LVALUE: has a name, has an address, persists beyond expression
 *   RVALUE: temporary, no name, about to be destroyed
 *
 *   int x = 5;      // x is lvalue, 5 is rvalue
 *   int y = x + 1;  // x is lvalue, (x+1) is rvalue
 *   foo(x);         // x passed as lvalue
 *   foo(x + 1);     // (x+1) passed as rvalue
 *   foo(std::move(x)); // x CAST to rvalue (but x still exists!)
 */

void takeByValue(std::string s) { std::cout << "  byValue: " << s << '\n'; }
void takeByLRef(std::string& s) { std::cout << "  byLRef: " << s << '\n'; }
void takeByRRef(std::string&& s) { std::cout << "  byRRef: " << s << '\n'; }

void demo_value_categories() {
    std::cout << "═══ LVALUE vs RVALUE ═══\n";

    std::string name = "Alice";

    takeByValue(name);             // copy (lvalue)
    takeByValue(std::move(name));  // move (rvalue) — name is now moved-from!
    name = "Bob";                  // restore

    takeByLRef(name);              // OK: lvalue
    // takeByLRef("temporary");    // ERROR: can't bind rvalue to lvalue ref

    takeByRRef(std::move(name));   // OK: rvalue
    // takeByRRef(name);           // ERROR: can't bind lvalue to rvalue ref
    takeByRRef(std::string("temp")); // OK: temporary is rvalue
}


// ═══════════════════════════════════════════════════════════════
//  2. Reference collapsing rules
// ═══════════════════════════════════════════════════════════════

/*
 *  When references to references form (via templates), they collapse:
 *
 *   ┌──────────────────────────────────────────────────┐
 *   │  T&  &   →  T&    (lvalue ref to lvalue ref)    │
 *   │  T&  &&  →  T&    (rvalue ref to lvalue ref)    │
 *   │  T&& &   →  T&    (lvalue ref to rvalue ref)    │
 *   │  T&& &&  →  T&&   (rvalue ref to rvalue ref)    │
 *   └──────────────────────────────────────────────────┘
 *
 *  RULE: If ANY reference is lvalue (&), result is lvalue (&)
 *        Only && + && gives &&
 */

void demo_reference_collapsing() {
    std::cout << "\n═══ REFERENCE COLLAPSING ═══\n";

    // When T&& receives an lvalue of type int:
    //   T = int&
    //   T&& = int& && = int&  (collapsed!)
    //
    // When T&& receives an rvalue of type int:
    //   T = int
    //   T&& = int&&           (no collapse needed)

    using T1 = int&;
    using T2 = T1&&;  // int& && → int&
    std::cout << "int& && is lvalue ref? " << std::is_lvalue_reference_v<T2> << '\n';

    using T3 = int&&;
    using T4 = T3&&;  // int&& && → int&&
    std::cout << "int&& && is rvalue ref? " << std::is_rvalue_reference_v<T4> << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Universal references (forwarding references)
// ═══════════════════════════════════════════════════════════════

/*
 *  T&& is a UNIVERSAL reference ONLY when T is a deduced template parameter:
 *
 *   template <typename T>
 *   void foo(T&& arg);      ← UNIVERSAL reference (T is deduced)
 *
 *   void bar(int&& arg);    ← rvalue reference (no deduction)
 *   void baz(Widget&& arg); ← rvalue reference (Widget is concrete)
 *   auto&& x = expr;        ← UNIVERSAL reference (auto deduction)
 */

void isLvalue(const std::string&)  { std::cout << "  → lvalue\n"; }
void isLvalue(const std::string&&) { std::cout << "  → rvalue\n"; } // Note: const&& is unusual but valid

template <typename T>
void universalRef(T&& arg) {
    std::cout << "  T deduced as: ";
    if constexpr (std::is_lvalue_reference_v<T>) {
        std::cout << "lvalue reference (T = string&)\n";
    } else {
        std::cout << "non-reference (T = string)\n";
    }
}

void demo_universal_ref() {
    std::cout << "\n═══ UNIVERSAL REFERENCES ═══\n";

    std::string s = "hello";

    std::cout << "universalRef(s):\n";
    universalRef(s);           // T = string&, arg = string& && = string&

    std::cout << "universalRef(move(s)):\n";
    universalRef(std::move(s)); // T = string, arg = string&&

    std::cout << "universalRef(\"temp\"):\n";
    universalRef(std::string("temp")); // T = string, arg = string&&
}


// ═══════════════════════════════════════════════════════════════
//  4. std::forward — Perfect forwarding
// ═══════════════════════════════════════════════════════════════

/*
 *  Problem: Inside a function, a named parameter is ALWAYS an lvalue
 *  (even if it was passed as an rvalue!)
 *
 *  void wrapper(string&& s) {
 *      inner(s);        // s is lvalue here (it has a name!)
 *      inner(move(s));  // Forces rvalue — but what if caller passed lvalue?
 *  }
 *
 *  Solution: std::forward preserves the original value category:
 *
 *  template <typename T>
 *  void wrapper(T&& arg) {
 *      inner(std::forward<T>(arg));  // forwards as lvalue or rvalue
 *  }
 */

class Heavy {
    std::string data_;
public:
    Heavy(const std::string& s) : data_(s) { std::cout << "  Heavy COPIED\n"; }
    Heavy(std::string&& s) : data_(std::move(s)) { std::cout << "  Heavy MOVED\n"; }
};

// WITHOUT forwarding — always copies
template <typename T>
Heavy makeHeavyBad(T&& arg) {
    return Heavy(arg);  // arg is always lvalue! Always copies!
}

// WITH forwarding — preserves value category
template <typename T>
Heavy makeHeavyGood(T&& arg) {
    return Heavy(std::forward<T>(arg));  // forwards as lvalue OR rvalue
}

void demo_perfect_forwarding() {
    std::cout << "\n═══ PERFECT FORWARDING ═══\n";

    std::string s = "data";

    std::cout << "WITHOUT forward (always copies):\n";
    std::cout << "  lvalue: ";
    auto h1 = makeHeavyBad(s);
    std::cout << "  rvalue: ";
    auto h2 = makeHeavyBad(std::move(s));  // Still copies! BAD!

    s = "data";  // restore
    std::cout << "\nWITH forward (preserves category):\n";
    std::cout << "  lvalue: ";
    auto h3 = makeHeavyGood(s);              // Copies (correct!)
    std::cout << "  rvalue: ";
    auto h4 = makeHeavyGood(std::move(s));   // Moves (correct!)
}


// ═══════════════════════════════════════════════════════════════
//  5. Practical: emplace with perfect forwarding
// ═══════════════════════════════════════════════════════════════

/*
 *  This is exactly how std::vector::emplace_back works!
 */

template <typename T>
class SimpleVector {
    std::vector<T> data_;
public:
    // Variadic perfect forwarding — forwards ALL args to T's constructor
    template <typename... Args>
    void emplace_back(Args&&... args) {
        data_.emplace_back(std::forward<Args>(args)...);
    }

    size_t size() const { return data_.size(); }
    const T& back() const { return data_.back(); }
};

void demo_emplace() {
    std::cout << "\n═══ EMPLACE (Variadic Forwarding) ═══\n";

    SimpleVector<std::string> vec;

    std::string s = "lvalue string";
    vec.emplace_back(s);                    // Copy
    vec.emplace_back(std::move(s));         // Move
    vec.emplace_back("literal");            // Construct in-place from const char*
    vec.emplace_back(5, 'x');              // Construct "xxxxx" in-place

    std::cout << "  Vector has " << vec.size() << " strings\n";
    std::cout << "  Last: " << vec.back() << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  6. Summary: When to use what
// ═══════════════════════════════════════════════════════════════

/*
 *  ┌────────────────────────────────────────────────────────────┐
 *  │  Situation                    │  Use                       │
 *  ├───────────────────────────────┼────────────────────────────┤
 *  │  "I'm done with this object" │  std::move(obj)            │
 *  │  "Forward as received"       │  std::forward<T>(arg)      │
 *  │  "Accept lvalue or rvalue"   │  template<T> f(T&& arg)    │
 *  │  Move constructor            │  Type(Type&& other)        │
 *  │  Sink parameter (take owned) │  void f(std::string s)     │
 *  │  Read-only access            │  void f(const string& s)   │
 *  └───────────────────────────────┴────────────────────────────┘
 *
 *  GOLDEN RULES:
 *  1. std::move on rvalue references (T&&)
 *  2. std::forward on universal references (template T&&)
 *  3. Never std::forward on rvalue references
 *  4. Never std::move on universal references
 */


int main() {
    demo_value_categories();
    demo_reference_collapsing();
    demo_universal_ref();
    demo_perfect_forwarding();
    demo_emplace();
    return 0;
}
