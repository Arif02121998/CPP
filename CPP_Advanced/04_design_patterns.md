# Chapter 4 — Design Patterns in Modern C++

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)
> Builds on [C++11 lambdas](../CPP11/03_lambdas.md),
> [C++11 smart pointers](../CPP11/04_smart_pointers.md), and
> [C++17 `variant`/`visit`](../CPP17/03_vocabulary_types.md).

Design patterns are reusable solutions to common design problems. Modern C++
(move semantics, smart pointers, lambdas, `variant`) often makes the *classic*
GoF implementations simpler — or unnecessary.

---

## 4.1 Creational Patterns

### Singleton — One Shared Instance

**Intent:** ensure a class has exactly one instance with global access.

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger inst;        // Meyers' Singleton: thread-safe since C++11
        return inst;
    }
    void log(const std::string& m) { /* ... */ }

    Logger(const Logger&) = delete;            // non-copyable
    Logger& operator=(const Logger&) = delete;
private:
    Logger() = default;
};

Logger::instance().log("hello");
```

**Why the static-local works:** C++11 guarantees **thread-safe initialization**
of function-local statics — no manual locking needed (the "Meyers' Singleton").

> **Caution:** Singletons are often an **anti-pattern** — they're hidden global
> state, hard to test, and create lifetime/ordering issues. Prefer dependency
> injection where practical.

### Factory — Centralized Object Creation

**Intent:** create objects without exposing the concrete type to the caller.

```cpp
class Shape { public: virtual ~Shape() = default; virtual double area() const = 0; };
class Circle : public Shape { /* ... */ };
class Square : public Shape { /* ... */ };

// Factory returns a smart pointer (ownership is explicit)
std::unique_ptr<Shape> makeShape(const std::string& type) {
    if (type == "circle") return std::make_unique<Circle>();
    if (type == "square") return std::make_unique<Square>();
    return nullptr;
}
```

Returning `std::unique_ptr<Base>` is the idiomatic modern factory — ownership is
clear and leak-free.

### Builder — Step-by-Step Construction

**Intent:** construct a complex object incrementally; great for many optional
parameters. Fluent interfaces use method chaining.

```cpp
class HttpRequest {
public:
    HttpRequest& url(std::string u)      { url_ = std::move(u);   return *this; }
    HttpRequest& method(std::string m)   { method_ = std::move(m);return *this; }
    HttpRequest& header(std::string k, std::string v) { headers_[k]=v; return *this; }
    Response send();
private:
    std::string url_, method_ = "GET";
    std::map<std::string,std::string> headers_;
};

auto resp = HttpRequest{}.url("http://x").method("POST").header("A","B").send();
```

---

## 4.2 Structural Patterns

### PIMPL — Pointer to Implementation

**Intent:** hide implementation details behind an opaque pointer to reduce
compile-time coupling and stabilize the ABI.

```cpp
// widget.h — header exposes NOTHING about internals
class Widget {
public:
    Widget();
    ~Widget();                       // must be declared (defined in .cpp)
    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;
    void doWork();
private:
    struct Impl;                     // forward declaration only
    std::unique_ptr<Impl> pImpl;     // opaque pointer to the real data
};

// widget.cpp — the real definition lives here, fully hidden
struct Widget::Impl {
    int internalState;
    std::vector<int> cache;
    void doWorkImpl() { /* ... */ }
};
Widget::Widget() : pImpl(std::make_unique<Impl>()) {}
Widget::~Widget() = default;          // defined where Impl is complete
void Widget::doWork() { pImpl->doWorkImpl(); }
```

**Benefits:**
- **Faster builds** — changing `Impl` doesn't recompile clients of `widget.h`.
- **Stable ABI** — the object's size/layout in the header never changes.
- **True encapsulation** — clients can't see private members.

**Pitfall:** the destructor (and move ops) must be defined in the `.cpp` where
`Impl` is **complete**, or `unique_ptr<Impl>` can't delete an incomplete type.

### Adapter — Make Incompatible Interfaces Work Together

```cpp
struct LegacyPrinter { void print_legacy(const char* s); };

class PrinterAdapter {                 // adapts legacy API to a modern interface
    LegacyPrinter legacy_;
public:
    void print(const std::string& s) { legacy_.print_legacy(s.c_str()); }
};
```

### Decorator — Add Behavior Dynamically

Wrap an object to extend behavior without modifying it (e.g., a buffered stream
wrapping a raw stream). Often expressed with composition + a shared interface.

---

## 4.3 Behavioral Patterns

### Strategy — Interchangeable Algorithms

**Intent:** select an algorithm at runtime. In modern C++, a **`std::function`
or a lambda** usually replaces the classic interface hierarchy.

```cpp
// Classic OOP strategy
struct SortStrategy { virtual void sort(std::vector<int>&) = 0; virtual ~SortStrategy()=default; };

// Modern: just store a callable
class Sorter {
    std::function<void(std::vector<int>&)> strategy_;
public:
    explicit Sorter(std::function<void(std::vector<int>&)> s) : strategy_(std::move(s)) {}
    void run(std::vector<int>& v) { strategy_(v); }
};

Sorter quick([](auto& v){ std::sort(v.begin(), v.end()); });
Sorter stable([](auto& v){ std::stable_sort(v.begin(), v.end()); });
```

### Observer — Publish/Subscribe

**Intent:** notify dependents when a subject changes.

```cpp
class Subject {
    std::vector<std::function<void(int)>> observers_;
public:
    void subscribe(std::function<void(int)> cb) { observers_.push_back(std::move(cb)); }
    void notify(int value) { for (auto& o : observers_) o(value); }
};

Subject s;
s.subscribe([](int v){ std::cout << "observer A: " << v << '\n'; });
s.notify(42);
```

> Lifetime caution: if observers capture references, ensure they outlive the
> subject — or use `weak_ptr` to auto-expire dead observers.

### Visitor — Operations Over a Type Hierarchy

**Intent:** add operations to a set of types without modifying them. Modern C++
expresses this elegantly with **`std::variant` + `std::visit`** (no virtuals,
no double dispatch boilerplate).

```cpp
using Shape = std::variant<Circle, Square, Triangle>;

struct AreaVisitor {
    double operator()(const Circle& c)   const { return 3.14159 * c.r * c.r; }
    double operator()(const Square& s)   const { return s.side * s.side; }
    double operator()(const Triangle& t) const { return 0.5 * t.base * t.height; }
};

Shape shape = Circle{2.0};
double a = std::visit(AreaVisitor{}, shape);    // dispatches to the right overload
```

See [C++17 vocabulary types](../CPP17/03_vocabulary_types.md) for the `overload`
helper that builds a visitor from lambdas.

---

## 4.4 Modern C++ Idioms (Patterns You Should Default To)

### RAII — The Master Pattern

Every resource (memory, file, lock, socket) should be owned by an object whose
destructor releases it. This is the foundation of exception safety. See
[Chapter 3](03_exceptions_and_error_models.md) and
[C++11 smart pointers](../CPP11/04_smart_pointers.md).

### Rule of Zero

Design classes so the compiler-generated special members are correct — by using
RAII members (`vector`, `unique_ptr`, ...) instead of raw resources. Then you
write **none** of the five special members.

```cpp
class Good {
    std::vector<int> data_;          // manages its own memory
    std::unique_ptr<Impl> impl_;     // manages its own resource
    // No destructor, copy, or move needed — all correct automatically
};
```

### Type Erasure

Provide a single runtime type that can hold **any** type satisfying an interface —
how `std::function`, `std::any`, and `std::shared_ptr`'s deleter work.

```cpp
// Sketch of type erasure (the std::function technique)
class Drawable {
    struct Concept { virtual ~Concept()=default; virtual void draw() const = 0; };
    template <class T> struct Model : Concept {
        T obj;
        Model(T o) : obj(std::move(o)) {}
        void draw() const override { obj.draw(); }   // calls T::draw()
    };
    std::unique_ptr<Concept> self_;
public:
    template <class T> Drawable(T x) : self_(std::make_unique<Model<T>>(std::move(x))) {}
    void draw() const { self_->draw(); }
};
// Now any type with a draw() method can be stored in a Drawable — no inheritance.
```

---

## Chapter 4 Cheat Sheet

```cpp
// Singleton (Meyers' — thread-safe since C++11)
static T& instance() { static T inst; return inst; }

// Factory → return unique_ptr<Base>
std::unique_ptr<Base> make(...) { return std::make_unique<Derived>(); }

// PIMPL: struct Impl; std::unique_ptr<Impl> pImpl;  (dtor in .cpp)

// Strategy / Observer → store std::function / lambdas
std::function<void(int)> cb;

// Visitor → std::variant + std::visit
std::visit(visitor, variantValue);

// Default idioms: RAII, Rule of Zero, type erasure
```

**Remember:**
- Prefer **lambdas/`std::function`** over class hierarchies for Strategy/Observer.
- Prefer **`variant` + `visit`** over the classic Visitor double-dispatch.
- **PIMPL** decouples compilation and stabilizes ABI (define dtor in `.cpp`).
- **Rule of Zero** beats hand-writing special members.
- Singletons are usually an anti-pattern — prefer dependency injection.

---

## Common Mistakes

1. PIMPL with the destructor defined in the header → incomplete-type error.
2. Overusing Singletons (hidden global state, untestable).
3. Observers capturing references that dangle when the subject outlives them.
4. Writing classic GoF boilerplate where a lambda/`variant` is simpler.

## When NOT to Use

- **Singleton** when you can pass the dependency explicitly (almost always).
- **Heavy pattern hierarchies** when a function/lambda expresses the variation.
- **PIMPL** for small, header-only, or performance-critical types (extra
  indirection + allocation).

---

*Next: [Chapter 5 — Advanced Concurrency Patterns](05_concurrency_patterns.md)*
