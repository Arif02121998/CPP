/*******************************************************************************
 * 01_classes_and_objects.cpp — Classes, Objects, and Access Specifiers
 *******************************************************************************
 *
 *  ┌──────────────────────────────────────────────────────────┐
 *  │  CLASS = Blueprint    │    OBJECT = Instance             │
 *  │                       │                                  │
 *  │  class Car {          │    Car toyota;                   │
 *  │    string brand;      │    Car honda;                    │
 *  │    int speed;         │    Car bmw;                      │
 *  │    void drive();      │                                  │
 *  │  };                   │    (3 separate objects, same     │
 *  │                       │     class/blueprint)             │
 *  └──────────────────────────────────────────────────────────┘
 *
 *  Access Specifiers:
 *  ┌───────────────────────────────────────────────────────────┐
 *  │ public:    Accessible from anywhere                       │
 *  │ private:   Accessible only within the class (DEFAULT)     │
 *  │ protected: Accessible within class + derived classes      │
 *  └───────────────────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++17 -o classes 01_classes_and_objects.cpp
 ******************************************************************************/

#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════
// 1. BASIC CLASS DEFINITION
// ═════════════════════════════════════════════════════════════════

/*
 *  Class Layout:
 *
 *  ┌──────────────────────────────────────┐
 *  │           class Person               │
 *  ├──────────── private ─────────────────┤
 *  │  std::string name_;                  │  ← Data members
 *  │  int age_;                           │     (attributes/fields)
 *  ├──────────── public ──────────────────┤
 *  │  Person(string, int)                 │  ← Constructor
 *  │  void introduce()                    │  ← Member functions
 *  │  string getName()                    │     (methods)
 *  │  int getAge()                        │
 *  └──────────────────────────────────────┘
 */

class Person {
private:  // Only accessible inside the class
    std::string name_;
    int age_;

public:   // Accessible from anywhere
    // Constructor
    Person(const std::string& name, int age)
        : name_(name), age_(age)     // Member initializer list (preferred!)
    {
        std::cout << "  [Person created: " << name_ << "]\n";
    }

    // Member functions (methods)
    void introduce() const {  // 'const' = this method doesn't modify the object
        std::cout << "Hi, I'm " << name_ << " and I'm " << age_ << " years old.\n";
    }

    // Getters
    std::string getName() const { return name_; }
    int getAge() const { return age_; }

    // Setters
    void setAge(int age) {
        if (age >= 0 && age <= 150) {  // Validation!
            age_ = age;
        }
    }
};

// ═════════════════════════════════════════════════════════════════
// 2. STRUCT VS CLASS
// ═════════════════════════════════════════════════════════════════

/*
 *  The ONLY difference between struct and class in C++:
 *
 *  struct → members are PUBLIC by default
 *  class  → members are PRIVATE by default
 *
 *  Convention:
 *  - Use struct for simple data holders (PODs)
 *  - Use class for objects with behavior/invariants
 */

struct Point {
    double x;  // public by default
    double y;

    // Structs can have constructors and methods too!
    Point(double x = 0, double y = 0) : x(x), y(y) {}

    double distanceTo(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// ═════════════════════════════════════════════════════════════════
// 3. CLASS WITH MULTIPLE FEATURES
// ═════════════════════════════════════════════════════════════════

class BankAccount {
private:
    std::string owner_;
    double balance_;
    static int account_count_;  // Shared across ALL instances

public:
    // Constructor
    BankAccount(const std::string& owner, double initial_balance = 0.0)
        : owner_(owner), balance_(initial_balance)
    {
        account_count_++;
    }

    // Methods
    void deposit(double amount) {
        if (amount > 0) {
            balance_ += amount;
            std::cout << "  Deposited $" << amount << " → Balance: $" << balance_ << '\n';
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance_) {
            balance_ -= amount;
            std::cout << "  Withdrew $" << amount << " → Balance: $" << balance_ << '\n';
            return true;
        }
        std::cout << "  Insufficient funds!\n";
        return false;
    }

    void display() const {
        std::cout << "  Account[" << owner_ << "]: $" << balance_ << '\n';
    }

    // Static method — called on the CLASS, not an object
    static int getAccountCount() {
        return account_count_;
    }

    // 'this' pointer — points to current object
    BankAccount& addBonus(double bonus) {
        this->balance_ += bonus;       // 'this' is explicit here
        return *this;                  // Return reference for chaining
    }
};

// Static member must be DEFINED outside the class
int BankAccount::account_count_ = 0;

// ═════════════════════════════════════════════════════════════════
// 4. CONST OBJECTS AND CONST METHODS
// ═════════════════════════════════════════════════════════════════

class Rectangle {
    double width_, height_;
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}

    // const method — can be called on const objects
    double area() const { return width_ * height_; }
    double perimeter() const { return 2 * (width_ + height_); }

    // non-const method — cannot be called on const objects
    void scale(double factor) {
        width_ *= factor;
        height_ *= factor;
    }
};

// ═════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════
int main() {

    std::cout << "=== 1. CREATING OBJECTS ===\n";
    Person alice("Alice", 30);
    Person bob("Bob", 25);
    alice.introduce();
    bob.introduce();

    // alice.name_;  // ERROR! private member
    std::cout << "Alice's name: " << alice.getName() << '\n';
    alice.setAge(31);
    std::cout << "Alice's new age: " << alice.getAge() << '\n';

    std::cout << "\n=== 2. STRUCT ===\n";
    Point p1(3.0, 4.0);
    Point p2(6.0, 8.0);
    std::cout << "Distance: " << p1.distanceTo(p2) << '\n';

    std::cout << "\n=== 3. BANK ACCOUNT ===\n";
    BankAccount acc1("Alice", 1000.0);
    BankAccount acc2("Bob", 500.0);
    acc1.deposit(250);
    acc1.withdraw(100);
    acc2.withdraw(1000);
    acc1.display();

    // Method chaining with 'this'
    acc1.addBonus(50).addBonus(25);
    acc1.display();

    // Static method
    std::cout << "Total accounts: " << BankAccount::getAccountCount() << '\n';

    std::cout << "\n=== 4. CONST OBJECTS ===\n";
    const Rectangle r(5.0, 3.0);
    std::cout << "Area: " << r.area() << '\n';
    std::cout << "Perimeter: " << r.perimeter() << '\n';
    // r.scale(2.0);  // ERROR! scale() is non-const, r is const

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. class = blueprint, object = instance
 * 2. Use member initializer lists in constructors (: member(value))
 * 3. Mark methods 'const' if they don't modify the object
 * 4. Struct members are public by default; class members are private
 * 5. Use static members for data shared across all instances
 * 6. 'this' is a pointer to the current object
 * 7. Convention: private data members end with _ (name_)
 */
