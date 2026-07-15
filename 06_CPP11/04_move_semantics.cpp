/*******************************************************************************
 * 04_move_semantics.cpp — Rvalue References, std::move, Perfect Forwarding
 *******************************************************************************
 *
 *  VALUE CATEGORIES:
 *
 *  ┌──────────────────────────────────────────────────────────────┐
 *  │  LVALUE                     │  RVALUE                        │
 *  │  "Has identity" (address)   │  "Temporary" (no address)      │
 *  │                              │                                │
 *  │  int x = 42;     ← x       │  42                            │
 *  │  string s = "hi"; ← s      │  x + y                         │
 *  │  arr[0]           ← arr[0] │  func_returning_by_value()     │
 *  │  *ptr             ← *ptr   │  std::move(x)                  │
 *  ├──────────────────────────────┼────────────────────────────────┤
 *  │  int& ref = x;    (OK)     │  int&& rref = 42;    (OK)     │
 *  │  int& ref = 42;   (ERROR)  │  int&& rref = x;     (ERROR)  │
 *  └──────────────────────────────┴────────────────────────────────┘
 *
 *  MOVE vs COPY:
 *
 *  COPY:  source───copy───►destination    (source unchanged, expensive)
 *  MOVE:  source═══move═══►destination    (source emptied, cheap!)
 *
 *  Copy a string "Hello World":
 *    - Allocate new memory
 *    - Copy all characters
 *    - O(n) time
 *
 *  Move a string "Hello World":
 *    - Steal the internal pointer
 *    - Set source to empty
 *    - O(1) time!
 *
 * COMPILE: g++ -std=c++17 -o move 04_move_semantics.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <utility>

class HeavyObject {
    std::string name_;
    int* data_;
    size_t size_;

public:
    HeavyObject(const std::string& name, size_t size)
        : name_(name), data_(new int[size]{}), size_(size)
    {
        std::cout << "  [Ctor] " << name_ << " (" << size_ << " ints)\n";
    }

    ~HeavyObject() {
        delete[] data_;
    }

    // Copy constructor (expensive!)
    HeavyObject(const HeavyObject& other)
        : name_(other.name_ + "_copy"), data_(new int[other.size_]), size_(other.size_)
    {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  [COPY] " << name_ << " (expensive: " << size_ << " copies)\n";
    }

    // Move constructor (cheap!)
    HeavyObject(HeavyObject&& other) noexcept
        : name_(std::move(other.name_)), data_(other.data_), size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "  [MOVE] " << name_ << " (cheap: pointer swap)\n";
    }

    // Copy assignment
    HeavyObject& operator=(const HeavyObject& other) {
        if (this != &other) {
            delete[] data_;
            name_ = other.name_ + "_assigned";
            size_ = other.size_;
            data_ = new int[size_];
            std::copy(other.data_, other.data_ + size_, data_);
            std::cout << "  [COPY=] " << name_ << '\n';
        }
        return *this;
    }

    // Move assignment
    HeavyObject& operator=(HeavyObject&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            name_ = std::move(other.name_);
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
            std::cout << "  [MOVE=] " << name_ << '\n';
        }
        return *this;
    }
};

// Function returning by value — move is automatic (RVO/NRVO)
HeavyObject createHeavy() {
    HeavyObject h("Factory", 1000);
    return h;  // Move or RVO (compiler may elide the move entirely)
}

// ═════════════════════════════════════════════════════════════════
// PERFECT FORWARDING
// ═════════════════════════════════════════════════════════════════

template<typename T>
void wrapper(T&& arg) {
    // std::forward preserves the value category (lvalue or rvalue)
    process(std::forward<T>(arg));
}

void process(const std::string& s) {
    std::cout << "  process(lvalue): " << s << '\n';
}

void process(std::string&& s) {
    std::cout << "  process(rvalue): " << s << '\n';
}

int main() {
    std::cout << "=== COPY vs MOVE ===\n";
    HeavyObject a("Alpha", 1000);

    HeavyObject b = a;                 // Copy (a is lvalue)
    HeavyObject c = std::move(a);      // Move (std::move casts to rvalue)

    std::cout << "\n=== MOVE INTO VECTOR ===\n";
    std::vector<HeavyObject> vec;
    vec.reserve(3);  // Pre-allocate to avoid reallocations

    HeavyObject d("Delta", 500);
    vec.push_back(d);              // Copy (d is lvalue)
    vec.push_back(std::move(d));   // Move (d is now empty)
    vec.push_back(HeavyObject("Temp", 100));  // Move (temporary is rvalue)

    std::cout << "\n=== RETURN VALUE OPTIMIZATION ===\n";
    HeavyObject e = createHeavy();  // Usually RVO (no copy or move!)

    std::cout << "\n=== PERFECT FORWARDING ===\n";
    std::string name = "Hello";
    wrapper(name);                    // Forwards as lvalue
    wrapper(std::string("World"));    // Forwards as rvalue
    wrapper(std::move(name));         // Forwards as rvalue

    /*
     * KEY RULES:
     * 1. std::move doesn't move — it CASTS to rvalue reference
     * 2. Move constructors/assignments should be noexcept
     * 3. After move, object is in "valid but unspecified state"
     * 4. Compiler uses RVO/NRVO to elide copies/moves for return values
     * 5. std::forward preserves value category in template forwarding
     */

    return 0;
}
