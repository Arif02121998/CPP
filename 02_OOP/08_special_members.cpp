/*******************************************************************************
 * 08_special_members.cpp — Rule of 0/3/5, Copy & Move Semantics
 *******************************************************************************
 *
 *  RULE OF THREE (C++98): If you define ANY of these, define ALL THREE:
 *    1. Destructor
 *    2. Copy Constructor
 *    3. Copy Assignment Operator
 *
 *  RULE OF FIVE (C++11): Add move semantics:
 *    4. Move Constructor
 *    5. Move Assignment Operator
 *
 *  RULE OF ZERO (Modern): Prefer using smart pointers and containers
 *    that manage resources, so you need NONE of the special members.
 *
 *  ┌─────────────────────────────────────────────────────────────────┐
 *  │  RULE OF ZERO (Best!)     │  Use smart_ptr, vector, string     │
 *  │  RULE OF FIVE (If needed) │  Manage raw resources yourself      │
 *  │  Rule of THREE (Legacy)   │  Pre-C++11, no move semantics      │
 *  └─────────────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o special 08_special_members.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <memory>
#include <utility>

// ═════════════════════════════════════════════════════════════════
// RULE OF FIVE — class managing raw resource
// ═════════════════════════════════════════════════════════════════

class Buffer {
    int* data_;
    size_t size_;

public:
    explicit Buffer(size_t size)
        : data_(new int[size]{}), size_(size)
    {
        std::cout << "  [Ctor: alloc " << size_ << "]\n";
    }

    // 1. Destructor
    ~Buffer() {
        std::cout << "  [Dtor: free " << size_ << "]\n";
        delete[] data_;
    }

    // 2. Copy Constructor (deep copy)
    Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
        std::cout << "  [Copy Ctor]\n";
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // 3. Copy Assignment
    Buffer& operator=(const Buffer& other) {
        std::cout << "  [Copy Assign]\n";
        if (this != &other) {
            Buffer temp(other);       // Copy-and-swap idiom
            std::swap(data_, temp.data_);
            std::swap(size_, temp.size_);
        }
        return *this;
    }

    // 4. Move Constructor (steal resources)
    Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
        std::cout << "  [Move Ctor]\n";
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // 5. Move Assignment
    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "  [Move Assign]\n";
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    size_t size() const { return size_; }
};

// ═════════════════════════════════════════════════════════════════
// RULE OF ZERO — let containers/smart pointers handle resources
// ═════════════════════════════════════════════════════════════════

class Person {
    std::string name_;     // std::string manages its own memory
    int age_;

public:
    Person(std::string name, int age) : name_(std::move(name)), age_(age) {}
    // No destructor, copy/move constructors, or assignment operators needed!
    // The compiler generates correct versions automatically.
};

// ═════════════════════════════════════════════════════════════════
// DELETED AND DEFAULTED SPECIAL MEMBERS
// ═════════════════════════════════════════════════════════════════

class Singleton {
public:
    Singleton() = default;
    Singleton(const Singleton&) = delete;            // No copying!
    Singleton& operator=(const Singleton&) = delete; // No copy assignment!
    Singleton(Singleton&&) = default;                 // Allow moving
    Singleton& operator=(Singleton&&) = default;      // Allow move assignment
};

int main() {
    std::cout << "=== RULE OF FIVE ===\n";
    {
        Buffer b1(10);
        Buffer b2 = b1;               // Copy ctor
        Buffer b3(5);
        b3 = b1;                      // Copy assignment
        Buffer b4 = std::move(b1);    // Move ctor (b1 is now empty)
        Buffer b5(3);
        b5 = std::move(b2);           // Move assignment
        std::cout << "--- leaving scope ---\n";
    }

    std::cout << "\n=== RULE OF ZERO ===\n";
    Person p1("Alice", 30);
    Person p2 = p1;  // Works automatically!

    std::cout << "\n=== DELETED OPERATIONS ===\n";
    Singleton s1;
    // Singleton s2 = s1;     // ERROR! Copy deleted
    Singleton s3 = std::move(s1);  // OK, move allowed

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Rule of Zero: Use RAII types (string, vector, unique_ptr) → no special members needed
 * 2. Rule of Five: If you manage raw resources, implement all 5 special members
 * 3. Use = default to explicitly request compiler-generated version
 * 4. Use = delete to prevent copying/moving
 * 5. Move operations should be noexcept for STL container optimization
 */
