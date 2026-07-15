/*******************************************************************************
 * 04_spaceship_operator.cpp — Three-way comparison <=> (spaceship operator)
 *******************************************************************************
 *
 *  Before C++20: You had to define ==, !=, <, >, <=, >= separately (6 ops!)
 *  C++20: Define ONE operator (<=>), get ALL SIX for free!
 *
 *   ┌──────────────────────────────────────────────────────────────┐
 *   │  (a <=> b)  returns one of:                                  │
 *   │                                                              │
 *   │    std::strong_ordering::less       (a < b)                  │
 *   │    std::strong_ordering::equal      (a == b)                 │
 *   │    std::strong_ordering::greater    (a > b)                  │
 *   │                                                              │
 *   │  Ordering categories:                                        │
 *   │  ┌───────────────────┬───────────────────────────────────┐   │
 *   │  │ strong_ordering   │ a==b means identical (int, string)│   │
 *   │  │ weak_ordering     │ a==b means equivalent (case-ins.) │   │
 *   │  │ partial_ordering  │ some values uncomparable (NaN)    │   │
 *   │  └───────────────────┴───────────────────────────────────┘   │
 *   └──────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o spaceship 04_spaceship_operator.cpp
 ******************************************************************************/

#include <iostream>
#include <compare>
#include <string>
#include <vector>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════
//  1. Basic spaceship operator
// ═══════════════════════════════════════════════════════════════

void demo_basic() {
    std::cout << "═══ BASIC <=> ═══\n";

    int a = 5, b = 10;
    auto result = (a <=> b);

    if (result < 0)  std::cout << "5 < 10\n";
    if (result == 0) std::cout << "5 == 10\n";
    if (result > 0)  std::cout << "5 > 10\n";

    // Check the type
    if (std::is_same_v<decltype(result), std::strong_ordering>) {
        std::cout << "int <=> int → strong_ordering\n";
    }

    // With doubles → partial_ordering (because NaN)
    double x = 3.14, y = 2.71;
    auto dresult = (x <=> y);
    if (dresult > 0) std::cout << "3.14 > 2.71\n";

    // String comparison
    std::string s1 = "apple", s2 = "banana";
    auto sresult = (s1 <=> s2);
    if (sresult < 0) std::cout << "\"apple\" < \"banana\"\n";
}


// ═══════════════════════════════════════════════════════════════
//  2. Defaulted <=> in classes — get ALL 6 operators free!
// ═══════════════════════════════════════════════════════════════

struct Point {
    int x, y;

    // ONE line generates ==, !=, <, >, <=, >= for free!
    auto operator<=>(const Point&) const = default;
};

void demo_defaulted() {
    std::cout << "\n═══ DEFAULTED <=> ═══\n";

    Point a{1, 2}, b{1, 3}, c{1, 2};

    std::cout << "a{1,2} == c{1,2}: " << (a == c) << '\n';
    std::cout << "a{1,2} != b{1,3}: " << (a != b) << '\n';
    std::cout << "a{1,2} <  b{1,3}: " << (a < b)  << '\n';
    std::cout << "b{1,3} >  a{1,2}: " << (b > a)  << '\n';

    // Sort works automatically!
    std::vector<Point> points = {{3,1}, {1,2}, {2,3}, {1,1}};
    std::sort(points.begin(), points.end());
    std::cout << "sorted: ";
    for (auto& p : points) std::cout << "(" << p.x << "," << p.y << ") ";
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  3. Custom <=> — control comparison logic
// ═══════════════════════════════════════════════════════════════

class Version {
    int major_, minor_, patch_;
public:
    Version(int maj, int min, int pat) : major_(maj), minor_(min), patch_(pat) {}

    // Custom: compare major → minor → patch
    std::strong_ordering operator<=>(const Version& other) const {
        if (auto cmp = major_ <=> other.major_; cmp != 0) return cmp;
        if (auto cmp = minor_ <=> other.minor_; cmp != 0) return cmp;
        return patch_ <=> other.patch_;
    }

    // == is NOT auto-generated from custom <=>, must define separately
    bool operator==(const Version& other) const {
        return major_ == other.major_ && minor_ == other.minor_ && patch_ == other.patch_;
    }

    friend std::ostream& operator<<(std::ostream& os, const Version& v) {
        return os << v.major_ << "." << v.minor_ << "." << v.patch_;
    }
};

void demo_custom() {
    std::cout << "\n═══ CUSTOM <=> ═══\n";

    Version v1{1, 2, 3}, v2{1, 3, 0}, v3{2, 0, 0}, v4{1, 2, 3};

    std::cout << v1 << " < " << v2 << " ? " << (v1 < v2) << '\n';
    std::cout << v3 << " > " << v2 << " ? " << (v3 > v2) << '\n';
    std::cout << v1 << " == " << v4 << " ? " << (v1 == v4) << '\n';

    // Sort versions
    std::vector<Version> versions = {v3, v1, v2, v4};
    std::sort(versions.begin(), versions.end());
    std::cout << "sorted: ";
    for (auto& v : versions) std::cout << v << " ";
    std::cout << '\n';
}


// ═══════════════════════════════════════════════════════════════
//  4. Weak ordering — case-insensitive string
// ═══════════════════════════════════════════════════════════════

class CiString {
    std::string data_;
public:
    CiString(std::string s) : data_(std::move(s)) {}

    // Weak ordering: "Hello" and "hello" are equivalent but not identical
    std::weak_ordering operator<=>(const CiString& other) const {
        auto toLower = [](unsigned char c) -> unsigned char { return std::tolower(c); };
        auto it1 = data_.begin(), it2 = other.data_.begin();
        for (; it1 != data_.end() && it2 != other.data_.end(); ++it1, ++it2) {
            auto c1 = toLower(*it1), c2 = toLower(*it2);
            if (c1 < c2) return std::weak_ordering::less;
            if (c1 > c2) return std::weak_ordering::greater;
        }
        if (data_.size() < other.data_.size()) return std::weak_ordering::less;
        if (data_.size() > other.data_.size()) return std::weak_ordering::greater;
        return std::weak_ordering::equivalent;
    }

    bool operator==(const CiString& other) const {
        return (*this <=> other) == 0;
    }

    friend std::ostream& operator<<(std::ostream& os, const CiString& s) {
        return os << s.data_;
    }
};

void demo_weak_ordering() {
    std::cout << "\n═══ WEAK ORDERING ═══\n";

    CiString a("Hello"), b("hello"), c("World");
    std::cout << a << " == " << b << " ? " << (a == b) << '\n';   // true (equivalent)
    std::cout << a << " <  " << c << " ? " << (a < c) << '\n';    // true
}


// ═══════════════════════════════════════════════════════════════
//  5. Before vs. After comparison
// ═══════════════════════════════════════════════════════════════

/*
 *  ┌───────────────────────────────────────────────────────────┐
 *  │           Before C++20              After C++20           │
 *  │                                                           │
 *  │  bool operator==(const T&) const;                         │
 *  │  bool operator!=(const T&) const;   auto operator<=>(    │
 *  │  bool operator< (const T&) const;       const T&) const  │
 *  │  bool operator> (const T&) const;       = default;       │
 *  │  bool operator<=(const T&) const;                         │
 *  │  bool operator>=(const T&) const;   // ONE LINE!          │
 *  │                                                           │
 *  │  (6 functions, ~30 lines)           (1 line)             │
 *  └───────────────────────────────────────────────────────────┘
 */


int main() {
    demo_basic();
    demo_defaulted();
    demo_custom();
    demo_weak_ordering();
    return 0;
}
