/*******************************************************************************
 * 03_inheritance.cpp — Inheritance: Code Reuse and IS-A Relationships
 *******************************************************************************
 *
 *  Inheritance Hierarchy:
 *
 *             ┌──────────┐
 *             │  Animal  │  ← Base class (Parent/Superclass)
 *             │──────────│
 *             │ name_    │
 *             │ speak()  │
 *             └────┬─────┘
 *                  │ IS-A
 *          ┌───────┴───────┐
 *          ▼               ▼
 *    ┌──────────┐   ┌──────────┐
 *    │   Dog    │   │   Cat    │  ← Derived classes (Children/Subclasses)
 *    │──────────│   │──────────│
 *    │ breed_   │   │ indoor_  │
 *    │ fetch()  │   │ purr()   │
 *    └──────────┘   └──────────┘
 *
 *  Access Inheritance:
 *  ┌────────────────┬──────────┬───────────┬──────────┐
 *  │ Base Member    │ public:  │ protected:│ private: │
 *  │ Access         │ inherit  │ inherit   │ inherit  │
 *  ├────────────────┼──────────┼───────────┼──────────┤
 *  │ public         │ public   │ protected │ private  │
 *  │ protected      │ protected│ protected │ private  │
 *  │ private        │ NO ACCESS│ NO ACCESS │ NO ACCESS│
 *  └────────────────┴──────────┴───────────┴──────────┘
 *
 * COMPILE: g++ -std=c++17 -o inheritance 03_inheritance.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <cmath>

// ═════════════════════════════════════════════════════════════════
// 1. BASIC INHERITANCE
// ═════════════════════════════════════════════════════════════════

class Animal {
protected:       // Accessible in derived classes
    std::string name_;
    int age_;

public:
    Animal(const std::string& name, int age) : name_(name), age_(age) {
        std::cout << "  [Animal constructor: " << name_ << "]\n";
    }

    virtual ~Animal() {  // Virtual destructor — ESSENTIAL for polymorphism
        std::cout << "  [Animal destructor: " << name_ << "]\n";
    }

    void eat() const {
        std::cout << name_ << " is eating.\n";
    }

    virtual void speak() const {  // Virtual — can be overridden
        std::cout << name_ << " makes a sound.\n";
    }

    std::string getName() const { return name_; }
};

// Dog IS-A Animal
class Dog : public Animal {
    std::string breed_;

public:
    // Call base constructor in initializer list
    Dog(const std::string& name, int age, const std::string& breed)
        : Animal(name, age), breed_(breed)
    {
        std::cout << "  [Dog constructor: " << name_ << ", breed: " << breed_ << "]\n";
    }

    ~Dog() override {
        std::cout << "  [Dog destructor: " << name_ << "]\n";
    }

    void speak() const override {  // Override base class method
        std::cout << name_ << " barks! Woof!\n";
    }

    void fetch() const {  // Dog-specific method
        std::cout << name_ << " fetches the ball!\n";
    }
};

// Cat IS-A Animal
class Cat : public Animal {
public:
    Cat(const std::string& name, int age) : Animal(name, age) {
        std::cout << "  [Cat constructor: " << name_ << "]\n";
    }

    void speak() const override {
        std::cout << name_ << " meows! Meow!\n";
    }

    void purr() const {
        std::cout << name_ << " purrs...\n";
    }
};

// ═════════════════════════════════════════════════════════════════
// 2. MULTILEVEL INHERITANCE
// ═════════════════════════════════════════════════════════════════

/*
 *    ┌─────────┐
 *    │ Vehicle │
 *    └────┬────┘
 *         │
 *    ┌────▼────┐
 *    │   Car   │
 *    └────┬────┘
 *         │
 *    ┌────▼──────────┐
 *    │ ElectricCar   │
 *    └───────────────┘
 */

class Vehicle {
protected:
    std::string make_;
    int year_;
public:
    Vehicle(const std::string& make, int year) : make_(make), year_(year) {}
    virtual void info() const {
        std::cout << year_ << " " << make_;
    }
};

class Car : public Vehicle {
protected:
    int doors_;
public:
    Car(const std::string& make, int year, int doors)
        : Vehicle(make, year), doors_(doors) {}
    void info() const override {
        Vehicle::info();  // Call base version
        std::cout << " (" << doors_ << " doors)";
    }
};

class ElectricCar : public Car {
    int range_km_;
public:
    ElectricCar(const std::string& make, int year, int doors, int range)
        : Car(make, year, doors), range_km_(range) {}
    void info() const override {
        Car::info();  // Call parent version
        std::cout << " [Electric, range: " << range_km_ << "km]";
    }
};

// ═════════════════════════════════════════════════════════════════
// 3. MULTIPLE INHERITANCE
// ═════════════════════════════════════════════════════════════════

/*
 *  ┌──────────────┐     ┌──────────────┐
 *  │  Printable   │     │  Serializable│
 *  │──────────────│     │──────────────│
 *  │ print()      │     │ serialize()  │
 *  └──────┬───────┘     └───────┬──────┘
 *         │                     │
 *         └────────┬────────────┘
 *                  ▼
 *         ┌──────────────┐
 *         │   Document   │
 *         │──────────────│
 *         │ content_     │
 *         └──────────────┘
 */

class Printable {
public:
    virtual void print() const = 0;         // Pure virtual
    virtual ~Printable() = default;
};

class Serializable {
public:
    virtual std::string serialize() const = 0;  // Pure virtual
    virtual ~Serializable() = default;
};

class Document : public Printable, public Serializable {
    std::string content_;
public:
    Document(const std::string& content) : content_(content) {}

    void print() const override {
        std::cout << "Document: " << content_ << '\n';
    }

    std::string serialize() const override {
        return "{\"content\": \"" + content_ + "\"}";
    }
};

// ═════════════════════════════════════════════════════════════════
// 4. THE DIAMOND PROBLEM & VIRTUAL INHERITANCE
// ═════════════════════════════════════════════════════════════════

/*
 *  Diamond Problem:
 *
 *       ┌─────────┐                          ┌─────────┐
 *       │  Base   │                          │  Base   │  ← Only ONE copy
 *       └────┬────┘                          └────┬────┘    with virtual
 *        ┌───┴────┐                          ┌────┴────┐    inheritance
 *        ▼        ▼                          ▼         ▼
 *   ┌────────┐ ┌────────┐             ┌────────┐ ┌────────┐
 *   │  Left  │ │ Right  │             │  Left  │ │ Right  │
 *   └────┬───┘ └────┬───┘             └────┬───┘ └────┬───┘
 *        └────┬─────┘                      └────┬─────┘
 *             ▼                                 ▼
 *       ┌──────────┐                      ┌──────────┐
 *       │ Diamond  │                      │ Diamond  │
 *       └──────────┘                      └──────────┘
 *   WITHOUT virtual: TWO              WITH virtual: ONE
 *   copies of Base!                   copy of Base ✓
 */

class Base {
public:
    int value_ = 0;
    void hello() const { std::cout << "Hello from Base! value=" << value_ << '\n'; }
};

class Left : virtual public Base {     // virtual inheritance
public:
    void leftMethod() const { std::cout << "Left method\n"; }
};

class Right : virtual public Base {    // virtual inheritance
public:
    void rightMethod() const { std::cout << "Right method\n"; }
};

class Diamond : public Left, public Right {
public:
    void allMethods() const {
        hello();          // Unambiguous — only one Base!
        leftMethod();
        rightMethod();
    }
};

// ═════════════════════════════════════════════════════════════════
// 5. CONSTRUCTOR / DESTRUCTOR ORDER
// ═════════════════════════════════════════════════════════════════

/*
 *  Construction order:  Base → Derived (top to bottom)
 *  Destruction order:   Derived → Base (bottom to top, reverse!)
 *
 *  For: class Derived : public Base
 *
 *  CREATE:  Base()    → Derived()
 *  DESTROY: ~Derived() → ~Base()
 */

// ═════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════
int main() {

    std::cout << "=== 1. BASIC INHERITANCE ===\n";
    {
        Dog rex("Rex", 5, "German Shepherd");
        rex.eat();       // Inherited from Animal
        rex.speak();     // Overridden in Dog
        rex.fetch();     // Dog-specific

        std::cout << '\n';
        Cat whiskers("Whiskers", 3);
        whiskers.eat();
        whiskers.speak();
        whiskers.purr();

        std::cout << "\n--- Destructors ---\n";
    }

    std::cout << "\n=== 2. MULTILEVEL INHERITANCE ===\n";
    ElectricCar tesla("Tesla Model 3", 2024, 4, 580);
    tesla.info();
    std::cout << '\n';

    std::cout << "\n=== 3. MULTIPLE INHERITANCE ===\n";
    Document doc("Hello World");
    doc.print();
    std::cout << "Serialized: " << doc.serialize() << '\n';

    std::cout << "\n=== 4. DIAMOND PROBLEM (Virtual Inheritance) ===\n";
    Diamond d;
    d.value_ = 42;       // Unambiguous with virtual inheritance
    d.allMethods();

    std::cout << "\n=== 5. POLYMORPHISM PREVIEW ===\n";
    // Base class pointer can point to derived objects!
    Animal* animals[] = {
        new Dog("Buddy", 3, "Labrador"),
        new Cat("Luna", 2)
    };
    std::cout << '\n';

    for (auto* a : animals) {
        a->speak();   // Calls correct version at RUNTIME (virtual dispatch)
        // a->fetch(); // ERROR! base class doesn't have fetch()
    }

    std::cout << "\n--- Cleanup ---\n";
    for (auto* a : animals) {
        delete a;  // Virtual destructor ensures proper cleanup!
    }

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. Inheritance models IS-A relationships (Dog IS-A Animal)
 * 2. Derived class inherits all members of base (access rules apply)
 * 3. Virtual destructor is ESSENTIAL when using base class pointers
 * 4. 'override' keyword ensures you're actually overriding (catches typos)
 * 5. Use virtual inheritance to solve the diamond problem
 * 6. Construction: base → derived; Destruction: derived → base
 * 7. Prefer composition over inheritance when IS-A doesn't fit
 */
