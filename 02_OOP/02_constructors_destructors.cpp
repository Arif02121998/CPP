/*******************************************************************************
 * 02_constructors_destructors.cpp — Object Lifecycle
 *******************************************************************************
 *
 *  Object Lifecycle:
 *
 *  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐
 *  │ CONSTRUCTION │───►│   USAGE      │───►│ DESTRUCTION  │
 *  │              │    │              │    │              │
 *  │ Constructor  │    │ Methods      │    │ Destructor   │
 *  │ called       │    │ called       │    │ called       │
 *  │ Memory alloc │    │              │    │ Memory freed │
 *  └──────────────┘    └──────────────┘    └──────────────┘
 *
 *  Constructor Types:
 *  ┌─────────────────────────────────────────────────────────┐
 *  │ Default      │ No arguments      │ MyClass()           │
 *  │ Parameterized│ Takes arguments   │ MyClass(int, string)│
 *  │ Copy         │ From same type    │ MyClass(const MC&)  │
 *  │ Move         │ Steals resources  │ MyClass(MC&&)       │
 *  │ Delegating   │ Calls another ctor│ MyClass(): MyClass(0)│
 *  └─────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o constructors 02_constructors_destructors.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <cstring>

// ═════════════════════════════════════════════════════════════════
// 1. ALL CONSTRUCTOR TYPES IN ONE CLASS
// ═════════════════════════════════════════════════════════════════

class MyString {
private:
    char* data_;
    size_t length_;

public:
    // ─── DEFAULT CONSTRUCTOR ───
    // Called when: MyString s;
    MyString() : data_(nullptr), length_(0) {
        std::cout << "  [Default Constructor]\n";
    }

    // ─── PARAMETERIZED CONSTRUCTOR ───
    // Called when: MyString s("Hello");
    MyString(const char* str) {
        std::cout << "  [Parameterized Constructor: \"" << str << "\"]\n";
        length_ = strlen(str);
        data_ = new char[length_ + 1];
        strcpy(data_, str);
    }

    // ─── COPY CONSTRUCTOR ───
    // Called when: MyString s2 = s1;  OR  MyString s2(s1);
    // Creates a DEEP COPY (allocates new memory, copies content)
    MyString(const MyString& other) : length_(other.length_) {
        std::cout << "  [Copy Constructor]\n";
        if (other.data_) {
            data_ = new char[length_ + 1];
            strcpy(data_, other.data_);
        } else {
            data_ = nullptr;
        }
    }

    /*
     *  Shallow Copy vs Deep Copy:
     *
     *  SHALLOW (BAD — default behavior):
     *  s1.data_ ──────► "Hello"
     *  s2.data_ ──────┘          ← Both point to SAME memory!
     *                              Double delete = CRASH!
     *
     *  DEEP (GOOD — what our copy constructor does):
     *  s1.data_ ──────► "Hello"  ← Separate memory
     *  s2.data_ ──────► "Hello"  ← Own copy
     */

    // ─── MOVE CONSTRUCTOR (C++11) ───
    // Called when: MyString s2 = std::move(s1);
    // STEALS resources from a temporary/expiring object
    MyString(MyString&& other) noexcept
        : data_(other.data_), length_(other.length_)
    {
        std::cout << "  [Move Constructor]\n";
        other.data_ = nullptr;   // Leave source in valid but empty state
        other.length_ = 0;
    }

    /*
     *  Move Semantics (steal instead of copy):
     *
     *  BEFORE move:
     *  other.data_ ──────► "Hello"
     *  this->data_  = ???
     *
     *  AFTER move:
     *  other.data_ = nullptr  (emptied)
     *  this->data_ ──────► "Hello"  (stolen!)
     *
     *  Much faster than copy for large objects!
     */

    // ─── COPY ASSIGNMENT OPERATOR ───
    MyString& operator=(const MyString& other) {
        std::cout << "  [Copy Assignment]\n";
        if (this != &other) {  // Self-assignment check!
            delete[] data_;
            length_ = other.length_;
            if (other.data_) {
                data_ = new char[length_ + 1];
                strcpy(data_, other.data_);
            } else {
                data_ = nullptr;
            }
        }
        return *this;
    }

    // ─── MOVE ASSIGNMENT OPERATOR (C++11) ───
    MyString& operator=(MyString&& other) noexcept {
        std::cout << "  [Move Assignment]\n";
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            length_ = other.length_;
            other.data_ = nullptr;
            other.length_ = 0;
        }
        return *this;
    }

    // ─── DESTRUCTOR ───
    // Called when object goes out of scope or is deleted
    ~MyString() {
        std::cout << "  [Destructor";
        if (data_) std::cout << ": \"" << data_ << "\"";
        std::cout << "]\n";
        delete[] data_;
    }

    // Utility
    void print() const {
        if (data_)
            std::cout << "  MyString: \"" << data_ << "\" (length: " << length_ << ")\n";
        else
            std::cout << "  MyString: (empty)\n";
    }
};

// ═════════════════════════════════════════════════════════════════
// 2. MEMBER INITIALIZER LIST
// ═════════════════════════════════════════════════════════════════

class Example {
    int x_;
    double y_;
    const int id_;      // MUST be initialized via initializer list
    int& ref_;          // MUST be initialized via initializer list

public:
    /*
     *  Why use member initializer list?
     *
     *  BAD (assignment in body):
     *    Example(int x) {
     *      x_ = x;  // x_ is DEFAULT-constructed, then ASSIGNED
     *    }           // Two steps! Wasteful for complex types
     *
     *  GOOD (initializer list):
     *    Example(int x) : x_(x) {}  // x_ is DIRECTLY constructed
     *                                // One step! Required for const & ref
     *
     *  REQUIRED for: const members, reference members, and
     *  members without default constructors
     */

    Example(int x, double y, int id, int& ref)
        : x_(x), y_(y), id_(id), ref_(ref)  // Initialize in declaration order!
    {
        std::cout << "  Example created: x=" << x_ << " y=" << y_
                  << " id=" << id_ << '\n';
    }
};

// ═════════════════════════════════════════════════════════════════
// 3. DELEGATING CONSTRUCTORS (C++11)
// ═════════════════════════════════════════════════════════════════

class Widget {
    int x_, y_;
    std::string name_;

public:
    // Primary constructor
    Widget(int x, int y, const std::string& name)
        : x_(x), y_(y), name_(name)
    {
        std::cout << "  Widget(" << x_ << ", " << y_ << ", " << name_ << ")\n";
    }

    // Delegates to primary constructor
    Widget() : Widget(0, 0, "default") {
        std::cout << "  (via delegating default ctor)\n";
    }

    Widget(int x, int y) : Widget(x, y, "unnamed") {
        std::cout << "  (via delegating 2-param ctor)\n";
    }
};

// ═════════════════════════════════════════════════════════════════
// 4. EXPLICIT CONSTRUCTOR (prevent implicit conversions)
// ═════════════════════════════════════════════════════════════════

class Distance {
    double meters_;
public:
    // Without 'explicit': Distance d = 5.0; would work (implicit conversion)
    // With 'explicit':    Distance d = 5.0; is ERROR; must use Distance d(5.0);
    explicit Distance(double m) : meters_(m) {}

    double getMeters() const { return meters_; }
};

// ═════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════
int main() {

    std::cout << "=== 1. CONSTRUCTORS ===\n";
    {
        std::cout << "\n--- Default constructor ---\n";
        MyString s1;
        s1.print();

        std::cout << "\n--- Parameterized constructor ---\n";
        MyString s2("Hello World");
        s2.print();

        std::cout << "\n--- Copy constructor ---\n";
        MyString s3 = s2;  // Copy constructor (NOT assignment!)
        s3.print();

        std::cout << "\n--- Move constructor ---\n";
        MyString s4 = std::move(s2);  // Move: s2 is now empty
        s4.print();
        s2.print();  // s2 is empty after move

        std::cout << "\n--- Copy assignment ---\n";
        s1 = s3;  // Copy assignment (s1 already exists)
        s1.print();

        std::cout << "\n--- Move assignment ---\n";
        s1 = std::move(s4);  // Move assignment
        s1.print();

        std::cout << "\n--- Destructors (leaving scope) ---\n";
    }
    // All destructors called here (reverse order of construction)

    std::cout << "\n=== 2. MEMBER INITIALIZER LIST ===\n";
    int ref_target = 99;
    Example ex(10, 3.14, 42, ref_target);

    std::cout << "\n=== 3. DELEGATING CONSTRUCTORS ===\n";
    Widget w1;             // Calls Widget() → Widget(0,0,"default")
    Widget w2(5, 10);      // Calls Widget(5,10) → Widget(5,10,"unnamed")
    Widget w3(1, 2, "ok"); // Calls primary directly

    std::cout << "\n=== 4. EXPLICIT CONSTRUCTOR ===\n";
    Distance d1(5.0);       // OK
    // Distance d2 = 5.0;   // ERROR! Implicit conversion blocked by 'explicit'
    std::cout << "Distance: " << d1.getMeters() << "m\n";

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Default constructor: no args. Compiler generates one IF you define no constructors
 * 2. Copy constructor: deep copy for classes with dynamic memory
 * 3. Move constructor (C++11): steals resources, much faster than copy
 * 4. Always use member initializer lists (: member(val)) over body assignment
 * 5. Delegating constructors reduce code duplication
 * 6. Use 'explicit' to prevent unwanted implicit conversions
 * 7. Destructor is called automatically when object leaves scope
 * 8. Rule of 5: If you define any of destructor/copy-ctor/copy-assign/
 *    move-ctor/move-assign, define ALL of them
 */
