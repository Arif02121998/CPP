/*******************************************************************************
 * 02_design_patterns.cpp — Common C++ design patterns and idioms
 *******************************************************************************
 *
 *  Patterns covered:
 *
 *   ┌─────────────────────────────────────────────────────────────────┐
 *   │  Pattern        │  Purpose                                      │
 *   ├─────────────────┼───────────────────────────────────────────────┤
 *   │  Singleton       │  Exactly one instance, global access          │
 *   │  Observer        │  Publish/subscribe notifications              │
 *   │  Strategy        │  Swap algorithms at runtime                   │
 *   │  Factory         │  Create objects without exposing concrete type│
 *   │  CRTP            │  Static polymorphism (no vtable overhead)     │
 *   │  Pimpl           │  Hide implementation, reduce compile deps     │
 *   └─────────────────┴───────────────────────────────────────────────┘
 *
 * COMPILE: g++ -std=c++20 -o patterns 02_design_patterns.cpp
 ******************************************************************************/

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_map>

// ═══════════════════════════════════════════════════════════════
//  1. Singleton — One and only one instance
// ═══════════════════════════════════════════════════════════════

/*
 *  C++11 guarantees thread-safe initialization of local statics.
 *  This is the "Meyer's Singleton" — simplest and safest approach.
 */

class Logger {
public:
    static Logger& instance() {
        static Logger instance;  // Thread-safe since C++11
        return instance;
    }

    void log(const std::string& msg) {
        std::cout << "[LOG] " << msg << '\n';
    }

    // Delete copy/move to prevent copies
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() { std::cout << "[LOG] Logger created\n"; }  // Private constructor
    ~Logger() = default;
};

void demo_singleton() {
    std::cout << "═══ SINGLETON ═══\n";
    Logger::instance().log("First message");
    Logger::instance().log("Second message");
    // Same instance both times
}


// ═══════════════════════════════════════════════════════════════
//  2. Observer — Publish/Subscribe
// ═══════════════════════════════════════════════════════════════

/*
 *   ┌──────────┐      notify()     ┌──────────┐
 *   │ Subject  │ ──────────────► │ Observer1 │
 *   │          │ ──────────────► │ Observer2 │
 *   │          │ ──────────────► │ Observer3 │
 *   └──────────┘                  └──────────┘
 */

class EventBus {
    std::unordered_map<std::string, std::vector<std::function<void(const std::string&)>>> listeners_;
public:
    void subscribe(const std::string& event, std::function<void(const std::string&)> callback) {
        listeners_[event].push_back(std::move(callback));
    }

    void publish(const std::string& event, const std::string& data) {
        if (auto it = listeners_.find(event); it != listeners_.end()) {
            for (auto& cb : it->second) {
                cb(data);
            }
        }
    }
};

void demo_observer() {
    std::cout << "\n═══ OBSERVER ═══\n";

    EventBus bus;

    // Subscribe
    bus.subscribe("user.login", [](const std::string& data) {
        std::cout << "  [Auth] User logged in: " << data << '\n';
    });
    bus.subscribe("user.login", [](const std::string& data) {
        std::cout << "  [Analytics] Track login: " << data << '\n';
    });
    bus.subscribe("order.placed", [](const std::string& data) {
        std::cout << "  [Email] Order confirmation: " << data << '\n';
    });

    // Publish
    bus.publish("user.login", "alice@example.com");
    bus.publish("order.placed", "Order #12345");
}


// ═══════════════════════════════════════════════════════════════
//  3. Strategy — Swap algorithms at runtime
// ═══════════════════════════════════════════════════════════════

/*
 *   ┌─────────────────────────────────────┐
 *   │  Sorter                             │
 *   │  ┌───────────────────────────────┐  │
 *   │  │ strategy: SortStrategy        │  │  ← swap at runtime
 *   │  └───────────────────────────────┘  │
 *   └─────────────────────────────────────┘
 */

// Modern C++ strategy: just use std::function (no need for inheritance!)
class DataProcessor {
    std::function<std::vector<int>(std::vector<int>)> strategy_;
    std::string name_;
public:
    DataProcessor(std::string name, std::function<std::vector<int>(std::vector<int>)> strategy)
        : strategy_(std::move(strategy)), name_(std::move(name)) {}

    void setStrategy(std::function<std::vector<int>(std::vector<int>)> s) {
        strategy_ = std::move(s);
    }

    void process(std::vector<int> data) {
        auto result = strategy_(std::move(data));
        std::cout << "  " << name_ << ": ";
        for (auto x : result) std::cout << x << " ";
        std::cout << '\n';
    }
};

void demo_strategy() {
    std::cout << "\n═══ STRATEGY ═══\n";

    auto ascending = [](std::vector<int> v) {
        std::sort(v.begin(), v.end());
        return v;
    };
    auto descending = [](std::vector<int> v) {
        std::sort(v.begin(), v.end(), std::greater<>{});
        return v;
    };
    auto evensFirst = [](std::vector<int> v) {
        std::stable_partition(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
        return v;
    };

    DataProcessor proc("Processor", ascending);
    proc.process({5, 3, 8, 1, 9, 2});

    proc.setStrategy(descending);
    proc.process({5, 3, 8, 1, 9, 2});

    proc.setStrategy(evensFirst);
    proc.process({5, 3, 8, 1, 9, 2});
}


// ═══════════════════════════════════════════════════════════════
//  4. Factory — Create objects without exposing concrete types
// ═══════════════════════════════════════════════════════════════

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual double area() const = 0;
};

class Circle : public Shape {
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    void draw() const override { std::cout << "  Drawing Circle (r=" << radius_ << ")\n"; }
    double area() const override { return 3.14159 * radius_ * radius_; }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    void draw() const override { std::cout << "  Drawing Rectangle (" << w_ << "x" << h_ << ")\n"; }
    double area() const override { return w_ * h_; }
};

// Factory function
std::unique_ptr<Shape> createShape(const std::string& type, double a, double b = 0) {
    if (type == "circle")    return std::make_unique<Circle>(a);
    if (type == "rectangle") return std::make_unique<Rectangle>(a, b);
    return nullptr;
}

void demo_factory() {
    std::cout << "\n═══ FACTORY ═══\n";

    auto shapes = {
        createShape("circle", 5.0),
        createShape("rectangle", 4.0, 6.0),
        createShape("circle", 3.0)
    };

    for (const auto& s : shapes) {
        if (s) {
            s->draw();
            std::cout << "    area = " << s->area() << '\n';
        }
    }
}


// ═══════════════════════════════════════════════════════════════
//  5. CRTP — Curiously Recurring Template Pattern
// ═══════════════════════════════════════════════════════════════

/*
 *  Static polymorphism — No virtual functions, no vtable, no overhead!
 *
 *   template <typename Derived>
 *   class Base {
 *       void interface() {
 *           static_cast<Derived*>(this)->implementation();
 *       }
 *   };
 *   class Concrete : public Base<Concrete> { ... };
 */

template <typename Derived>
class Printable {
public:
    void print() const {
        // Call derived class's method — resolved at COMPILE TIME
        const auto& self = static_cast<const Derived&>(*this);
        std::cout << "  " << self.toString() << '\n';
    }
};

class Dog : public Printable<Dog> {
public:
    std::string name;
    int age;
    Dog(std::string n, int a) : name(std::move(n)), age(a) {}

    std::string toString() const {
        return "Dog{" + name + ", age=" + std::to_string(age) + "}";
    }
};

class Cat : public Printable<Cat> {
public:
    std::string name;
    Cat(std::string n) : name(std::move(n)) {}

    std::string toString() const {
        return "Cat{" + name + "}";
    }
};

void demo_crtp() {
    std::cout << "\n═══ CRTP (Static Polymorphism) ═══\n";

    Dog dog("Rex", 5);
    Cat cat("Whiskers");

    dog.print();   // No virtual call!
    cat.print();   // Resolved at compile time!
}


// ═══════════════════════════════════════════════════════════════
//  6. Pimpl — Pointer to Implementation
// ═══════════════════════════════════════════════════════════════

/*
 *  Pimpl hides implementation details behind a pointer.
 *  Benefits:
 *    - Reduces header dependencies (faster compilation)
 *    - ABI stability (implementation can change without recompiling users)
 *    - Clean public API
 *
 *  (Normally split across .h and .cpp — combined here for demo)
 *
 *   ┌──────────── Widget.h ──────────────┐
 *   │ class Widget {                      │
 *   │   struct Impl;                      │ ← forward declaration only
 *   │   unique_ptr<Impl> pImpl_;          │
 *   │ public:                             │
 *   │   void doWork();                    │
 *   │ };                                  │
 *   └────────────────────────────────────┘
 *
 *   ┌──────────── Widget.cpp ────────────┐
 *   │ struct Widget::Impl {               │ ← full definition here
 *   │   // all private data and methods   │
 *   │ };                                  │
 *   └────────────────────────────────────┘
 */

class Widget {
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
public:
    Widget(const std::string& name);
    ~Widget();                          // Must be defined where Impl is complete
    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;

    void doWork();
    std::string name() const;
};

// Normally in .cpp file:
struct Widget::Impl {
    std::string name;
    int internalCounter = 0;

    void doWorkImpl() {
        ++internalCounter;
        std::cout << "  Widget '" << name << "' working (count=" << internalCounter << ")\n";
    }
};

Widget::Widget(const std::string& name) : pImpl_(std::make_unique<Impl>()) {
    pImpl_->name = name;
}
Widget::~Widget() = default;
Widget::Widget(Widget&&) noexcept = default;
Widget& Widget::operator=(Widget&&) noexcept = default;

void Widget::doWork() { pImpl_->doWorkImpl(); }
std::string Widget::name() const { return pImpl_->name; }

void demo_pimpl() {
    std::cout << "\n═══ PIMPL (Compilation Firewall) ═══\n";

    Widget w("MyWidget");
    w.doWork();
    w.doWork();
    std::cout << "  Widget name: " << w.name() << '\n';
}


int main() {
    demo_singleton();
    demo_observer();
    demo_strategy();
    demo_factory();
    demo_crtp();
    demo_pimpl();
    return 0;
}
