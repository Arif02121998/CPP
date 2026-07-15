/*******************************************************************************
 * 04_polymorphism.cpp — Virtual Functions, VTable, Override, Final
 *******************************************************************************
 *
 *  Polymorphism = "Many forms" — same interface, different behavior
 *
 *  TWO TYPES:
 *  ┌──────────────────────────┬──────────────────────────────┐
 *  │ COMPILE-TIME (Static)    │ RUNTIME (Dynamic)            │
 *  │ ─────────────────────    │ ──────────────────           │
 *  │ Function overloading     │ Virtual functions             │
 *  │ Operator overloading     │ Base class ptr/ref            │
 *  │ Templates                │ Resolved at RUNTIME           │
 *  │ Resolved at COMPILE time │ Uses vtable mechanism         │
 *  └──────────────────────────┴──────────────────────────────┘
 *
 *  Virtual Function Table (vtable):
 *
 *  ┌─────────────────┐      ┌─────────────────────────────┐
 *  │  Dog object      │      │    Dog's VTable              │
 *  │─────────────────│      │─────────────────────────────│
 *  │ vptr ───────────┼─────►│ speak() → Dog::speak()      │
 *  │ name_           │      │ move()  → Animal::move()    │
 *  │ breed_          │      │ dtor    → Dog::~Dog()       │
 *  └─────────────────┘      └─────────────────────────────┘
 *
 *  ┌─────────────────┐      ┌─────────────────────────────┐
 *  │  Cat object      │      │    Cat's VTable              │
 *  │─────────────────│      │─────────────────────────────│
 *  │ vptr ───────────┼─────►│ speak() → Cat::speak()      │
 *  │ name_           │      │ move()  → Cat::move()       │
 *  │ indoor_         │      │ dtor    → Cat::~Cat()       │
 *  └─────────────────┘      └─────────────────────────────┘
 *
 *  When calling animal_ptr->speak():
 *  1. Look up vptr in the object
 *  2. Go to the vtable
 *  3. Call the function pointer at speak()'s slot
 *  → Correct version called at RUNTIME!
 *
 * COMPILE: g++ -std=c++17 -o polymorphism 04_polymorphism.cpp
 ******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ═════════════════════════════════════════════════════════════════
// 1. BASE CLASS WITH VIRTUAL FUNCTIONS
// ═════════════════════════════════════════════════════════════════

class Shape {
protected:
    std::string name_;

public:
    Shape(const std::string& name) : name_(name) {}

    // Virtual destructor — ALWAYS needed when using polymorphism
    virtual ~Shape() = default;

    // Virtual function — CAN be overridden (has default implementation)
    virtual double area() const {
        return 0.0;
    }

    // Pure virtual function — MUST be overridden (no default)
    // Makes Shape an ABSTRACT class (cannot be instantiated)
    virtual double perimeter() const = 0;

    // Non-virtual — NOT overridable through virtual dispatch
    std::string getName() const { return name_; }

    // Virtual
    virtual void describe() const {
        std::cout << name_ << ": area=" << area()
                  << ", perimeter=" << perimeter() << '\n';
    }
};

// ═════════════════════════════════════════════════════════════════
// 2. DERIVED CLASSES — OVERRIDE VIRTUAL FUNCTIONS
// ═════════════════════════════════════════════════════════════════

class Circle : public Shape {
    double radius_;
public:
    Circle(double r) : Shape("Circle"), radius_(r) {}

    // 'override' keyword — ALWAYS use it! Catches:
    //   - Typos in function name
    //   - Wrong parameter types
    //   - Missing 'const'
    double area() const override {
        return 3.14159265 * radius_ * radius_;
    }

    double perimeter() const override {
        return 2 * 3.14159265 * radius_;
    }
};

class Rectangle : public Shape {
    double width_, height_;
public:
    Rectangle(double w, double h)
        : Shape("Rectangle"), width_(w), height_(h) {}

    double area() const override {
        return width_ * height_;
    }

    double perimeter() const override {
        return 2 * (width_ + height_);
    }
};

class Triangle : public Shape {
    double a_, b_, c_;  // Three sides
public:
    Triangle(double a, double b, double c)
        : Shape("Triangle"), a_(a), b_(b), c_(c) {}

    double area() const override {
        // Heron's formula
        double s = (a_ + b_ + c_) / 2;
        return std::sqrt(s * (s - a_) * (s - b_) * (s - c_));
    }

    double perimeter() const override {
        return a_ + b_ + c_;
    }
};

// ═════════════════════════════════════════════════════════════════
// 3. FINAL KEYWORD (C++11) — prevent further overriding
// ═════════════════════════════════════════════════════════════════

class Square final : public Rectangle {  // 'final' on class = cannot be inherited
    // Note: 'final' can also be on individual methods
public:
    Square(double side) : Rectangle(side, side) {}

    // void area() const override final { }  // Can mark method as final too
};

// class SuperSquare : public Square {};  // ERROR! Square is final

// ═════════════════════════════════════════════════════════════════
// 4. POLYMORPHISM IN ACTION
// ═════════════════════════════════════════════════════════════════

// This function works with ANY Shape — power of polymorphism!
void printShapeInfo(const Shape& shape) {
    shape.describe();  // Calls correct version based on actual type
}

double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) {
    double total = 0;
    for (const auto& shape : shapes) {
        total += shape->area();  // Virtual dispatch — correct area() called
    }
    return total;
}

// ═════════════════════════════════════════════════════════════════
// 5. OBJECT SLICING PROBLEM
// ═════════════════════════════════════════════════════════════════

/*
 *  Object Slicing:
 *
 *  When assigning a derived object to a base VARIABLE (not pointer/ref),
 *  the derived part is "sliced off":
 *
 *  Dog:                    Animal (sliced):
 *  ┌──────────────────┐   ┌──────────────────┐
 *  │ vptr (Dog)       │   │ vptr (Animal)    │  ← Changed!
 *  │ name_            │   │ name_            │
 *  │ age_             │   │ age_             │
 *  │ breed_  ──────── │   └──────────────────┘  ← breed_ LOST!
 *  └──────────────────┘
 *
 *  SOLUTION: Always use pointers or references for polymorphic code
 */

class Base {
public:
    virtual void who() const { std::cout << "I am Base\n"; }
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void who() const override { std::cout << "I am Derived\n"; }
};

void demonstrateSlicing() {
    Derived d;

    // Polymorphic (correct):
    Base& ref = d;
    ref.who();       // "I am Derived" ✓

    Base* ptr = &d;
    ptr->who();      // "I am Derived" ✓

    // Object slicing (BAD):
    Base sliced = d;  // Copies only Base part!
    sliced.who();     // "I am Base" ✗ — Derived part LOST!
}

// ═════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════
int main() {

    std::cout << "=== 1. POLYMORPHISM WITH SHAPES ===\n";

    Circle circle(5.0);
    Rectangle rect(4.0, 6.0);
    Triangle tri(3.0, 4.0, 5.0);
    Square sq(4.0);

    // Using base class reference — polymorphism!
    printShapeInfo(circle);      // Circle's version
    printShapeInfo(rect);        // Rectangle's version
    printShapeInfo(tri);         // Triangle's version
    printShapeInfo(sq);          // Square(→Rectangle)'s version

    std::cout << "\n=== 2. COLLECTION OF SHAPES ===\n";
    {
        std::vector<std::unique_ptr<Shape>> shapes;
        shapes.push_back(std::make_unique<Circle>(3.0));
        shapes.push_back(std::make_unique<Rectangle>(5.0, 2.0));
        shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));
        shapes.push_back(std::make_unique<Square>(4.0));

        for (const auto& s : shapes) {
            s->describe();
        }
        std::cout << "Total area: " << totalArea(shapes) << '\n';
    }

    std::cout << "\n=== 3. OBJECT SLICING ===\n";
    demonstrateSlicing();

    std::cout << "\n=== 4. DYNAMIC_CAST (Safe Downcasting) ===\n";
    {
        std::unique_ptr<Shape> shape = std::make_unique<Circle>(7.0);

        // dynamic_cast safely checks type at runtime
        if (Circle* c = dynamic_cast<Circle*>(shape.get())) {
            std::cout << "It's a circle! Area = " << c->area() << '\n';
        }

        if (Rectangle* r = dynamic_cast<Rectangle*>(shape.get())) {
            std::cout << "It's a rectangle!\n";
        } else {
            std::cout << "Not a rectangle (dynamic_cast returned nullptr)\n";
        }
    }

    return 0;
}

/*
 * KEY TAKEAWAYS:
 * ═══════════════
 * 1. virtual functions enable runtime polymorphism via vtable
 * 2. ALWAYS use 'override' when overriding virtual functions
 * 3. Pure virtual (= 0) makes class abstract — cannot instantiate
 * 4. ALWAYS use virtual destructor in base classes used polymorphically
 * 5. 'final' prevents further overriding or inheritance
 * 6. Use pointers/references (not values) for polymorphic behavior
 * 7. Object slicing occurs when copying derived → base by value
 * 8. dynamic_cast performs safe runtime type checking (returns nullptr on failure)
 */
