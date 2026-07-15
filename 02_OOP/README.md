<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C H A P T E R &nbsp; 2</samp>
</p>

<h1 align="center">📘 Object-Oriented Programming</h1>

<p align="center"><em>"Model the real world inside your code."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Classes & Objects | [§1](#-1-classes--objects) |
| 2 | Constructors & Destructors | [§2](#-2-constructors--destructors) |
| 3 | Inheritance | [§3](#-3-inheritance) |
| 4 | Polymorphism | [§4](#-4-polymorphism) |
| 5 | Encapsulation | [§5](#-5-encapsulation) |
| 6 | Abstraction | [§6](#-6-abstraction) |
| 7 | Operator Overloading | [§7](#-7-operator-overloading) |
| 8 | Special Members & Rule of Five | [§8](#-8-special-members--the-rule-of-five) |

> ```bash
> g++ -std=c++17 -o output filename.cpp && ./output
> ```

---

## 🔹 1. Classes & Objects

> *📄 See full code → `01_classes_and_objects.cpp`*

### The Concept

A **class** is a **blueprint**; an **object** is a **thing built from that blueprint**.

Think of a house blueprint. The blueprint itself isn't a house — it *describes* what a house looks like: how many rooms, where the doors go, what color the walls are. When you actually *build* a house from the blueprint, that house is an **object** (an *instance* of the blueprint).

A class bundles together:
- **Data** (what the object *knows*) — called **member variables** or **attributes**
- **Behavior** (what the object *can do*) — called **member functions** or **methods**

```
  ┌─────────── Class (Blueprint) ──────────┐
  │                                         │
  │  private:          ← Hidden internals   │
  │    std::string name_;                   │
  │    int age_;                            │
  │                                         │
  │  public:           ← Visible interface  │
  │    void introduce();                    │
  │    std::string getName();               │
  │                                         │
  └─────────────────────────────────────────┘
```

### Example

```cpp
#include <iostream>
#include <string>

class Person {
private:                              // Only accessible INSIDE the class
    std::string name_;
    int age_;

public:                               // Accessible from ANYWHERE
    // Constructor — called when creating an object
    Person(const std::string& name, int age)
        : name_(name), age_(age) {}   // Member initializer list

    void introduce() const {
        std::cout << "Hi, I'm " << name_ << ", age " << age_ << '\n';
    }

    std::string getName() const { return name_; }
    void setAge(int age) { age_ = age; }
};

int main() {
    Person alice("Alice", 30);     // Create an object from the blueprint
    Person bob("Bob", 25);

    alice.introduce();             // Hi, I'm Alice, age 30
    bob.introduce();               // Hi, I'm Bob, age 25

    bob.setAge(26);
    std::cout << bob.getName() << '\n';   // Bob
}
```

### How This Works

| Part | Purpose |
|------|---------|
| `private:` | Data is hidden — outsiders can't touch `name_` or `age_` directly. This prevents corruption. |
| `public:` | The "buttons" the outside world can press: `introduce()`, `getName()`, `setAge()`. |
| `: name_(name), age_(age)` | **Member initializer list** — sets fields before the constructor body runs. More efficient than assigning inside `{}`. |
| `const` after `introduce()` | Promises this method won't modify the object. Good practice for read-only methods. |

---

## 🔹 2. Constructors & Destructors

> *📄 See full code → `02_constructors_destructors.cpp`*

### The Concept

A **constructor** is the birth of an object — it runs automatically when the object is created. A **destructor** is the death — it runs when the object is destroyed, cleaning up any resources.

```
  Object Lifecycle:

  ┌─────────────┐      ┌───────────┐      ┌──────────────┐
  │ Constructor  │ ───► │   Usage   │ ───► │  Destructor  │
  │ (allocate)   │      │ (methods) │      │ (clean up)   │
  └─────────────┘      └───────────┘      └──────────────┘
       Birth               Life                Death
```

### Example

```cpp
#include <iostream>
#include <cstring>

class MyString {
    char* data_;
    size_t length_;

public:
    // Default constructor
    MyString() : data_(nullptr), length_(0) {
        std::cout << "  Default constructed\n";
    }

    // Parameterized constructor
    MyString(const char* str) {
        length_ = strlen(str);
        data_ = new char[length_ + 1];   // Allocate memory
        strcpy(data_, str);
        std::cout << "  Constructed: \"" << data_ << "\"\n";
    }

    // Copy constructor — makes a DEEP copy
    MyString(const MyString& other) : length_(other.length_) {
        data_ = new char[length_ + 1];
        strcpy(data_, other.data_);
        std::cout << "  Copy constructed: \"" << data_ << "\"\n";
    }

    // Move constructor — STEALS resources (fast!)
    MyString(MyString&& other) noexcept
        : data_(other.data_), length_(other.length_) {
        other.data_ = nullptr;           // Leave source in valid empty state
        other.length_ = 0;
        std::cout << "  Move constructed\n";
    }

    // Destructor — cleanup
    ~MyString() {
        std::cout << "  Destroyed";
        if (data_) std::cout << ": \"" << data_ << "\"";
        std::cout << '\n';
        delete[] data_;                  // Free memory
    }
};

int main() {
    MyString a("Hello");               // Parameterized constructor
    MyString b = a;                    // Copy constructor (deep copy)
    MyString c = std::move(a);         // Move constructor (steal from a)
}   // ← Destructors called here for c, b, a (reverse order)
```

### How This Works

- **Copy constructor** `MyString(const MyString& other)` — Allocates new memory and copies the string character by character. Like **photocopying** a document — you get an independent copy.
- **Move constructor** `MyString(MyString&& other)` — Steals the pointer from `other` and sets `other`'s pointer to `nullptr`. Like **handing over** a document instead of photocopying. Much faster — no allocation, no copying.
- **Destructor** `~MyString()` — Frees the dynamically allocated `data_`. Without it, the memory would **leak** every time a `MyString` is destroyed.
- Objects are destroyed in **reverse order** of creation (LIFO — like a stack of plates).

---

## 🔹 3. Inheritance

> *📄 See full code → `03_inheritance.cpp`*

### The Concept

**Inheritance** lets you create a new class based on an existing one. The new class **inherits** all the data and behavior of the parent, so you don't have to rewrite it. It's the "is-a" relationship:

```
          ┌──────────┐
          │  Animal   │ ← Base class (parent)
          │  name_    │
          │  speak()  │
          └─────┬────┘
                │ IS-A
         ┌──────┴──────┐
         ▼             ▼
    ┌──────────┐  ┌──────────┐
    │   Dog    │  │   Cat    │ ← Derived classes (children)
    │  breed_  │  │  indoor_ │
    │  speak() │  │  speak() │
    └──────────┘  └──────────┘

    A Dog IS AN Animal.
    A Cat IS AN Animal.
```

### Example

```cpp
#include <iostream>
#include <string>

class Animal {
protected:                        // Accessible by this class AND derived classes
    std::string name_;

public:
    Animal(const std::string& name) : name_(name) {}

    virtual void speak() const {
        std::cout << name_ << " makes a generic sound\n";
    }

    virtual ~Animal() = default;  // Virtual destructor — ALWAYS do this for base classes
};

class Dog : public Animal {       // Dog inherits from Animal
public:
    Dog(const std::string& name) : Animal(name) {}

    void speak() const override {
        std::cout << name_ << " says: Woof!\n";
    }
};

class Cat : public Animal {
public:
    Cat(const std::string& name) : Animal(name) {}

    void speak() const override {
        std::cout << name_ << " says: Meow!\n";
    }
};

int main() {
    Dog rex("Rex");
    Cat whiskers("Whiskers");

    rex.speak();          // Rex says: Woof!
    whiskers.speak();     // Whiskers says: Meow!

    // The magic: treat different animals the same way
    Animal* pet = &rex;
    pet->speak();         // Rex says: Woof!  (calls Dog::speak, not Animal::speak)
}
```

### How This Works

| Keyword | Meaning |
|---------|---------|
| `class Dog : public Animal` | Dog inherits everything from Animal. `public` means Animal's public members stay public in Dog. |
| `protected:` | Like `private`, but derived classes can access it too. `name_` is visible to Dog and Cat. |
| `virtual` | "This function can be overridden by derived classes." Without `virtual`, the base version always runs. |
| `override` | "I'm intentionally overriding a base class function." Catches typos at compile time. |
| `virtual ~Animal()` | **Always make base class destructors virtual.** Without this, `delete basePtr` wouldn't call the derived destructor → memory leak. |

---

## 🔹 4. Polymorphism

> *📄 See full code → `04_polymorphism.cpp`*

### The Concept

**Polymorphism** means "many forms." You write code that works with a **base class**, and it automatically does the right thing for **any derived class** — without knowing or caring which specific class it is.

> Think of a universal remote. You press "Play" and it works correctly whether the TV, DVD player, or sound system receives the signal. Same button, different behavior.

### Example

```cpp
#include <iostream>
#include <vector>
#include <memory>

class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;        // = 0 makes this PURE VIRTUAL
    virtual void draw() const = 0;          // The class is now ABSTRACT
};

class Circle : public Shape {
    double radius_;
public:
    Circle(double r) : radius_(r) {}

    double area() const override {
        return 3.14159 * radius_ * radius_;
    }

    void draw() const override {
        std::cout << "Drawing circle (r=" << radius_ << ", area=" << area() << ")\n";
    }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}

    double area() const override { return w_ * h_; }

    void draw() const override {
        std::cout << "Drawing rectangle (" << w_ << "x" << h_
                  << ", area=" << area() << ")\n";
    }
};

int main() {
    // A collection of DIFFERENT shapes — treated uniformly
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Circle>(3.0));

    for (const auto& shape : shapes) {
        shape->draw();    // Each shape draws itself correctly!
    }
}
```

### 🖥️ Output

```
Drawing circle (r=5, area=78.5398)
Drawing rectangle (4x6, area=24)
Drawing circle (r=3, area=28.2743)
```

### How This Works — The Virtual Table (vtable)

When you declare a function `virtual`, the compiler creates a hidden lookup table for each class:

```
  Circle object in memory:        Circle's vtable (lookup table):
  ┌──────────────────┐           ┌──────────────────────────┐
  │  vptr  ──────────┼──────────►│ area()  → Circle::area() │
  │  radius_ = 5.0   │           │ draw()  → Circle::draw() │
  └──────────────────┘           └──────────────────────────┘

  When you call shape->draw():
  1. Follow the vptr to the vtable
  2. Look up "draw" in the table
  3. Jump to Circle::draw()
```

- **`= 0` (pure virtual)** makes a class **abstract** — you cannot create instances of `Shape` directly. It's a contract: *"Any class inheriting from me MUST implement these methods."*
- The `unique_ptr<Shape>` vector holds pointers to the **base** type but the actual objects are circles and rectangles. That's polymorphism.

---

## 🔹 5. Encapsulation

> *📄 See full code → `05_encapsulation.cpp`*

### The Concept

**Encapsulation** means **hiding the internals** and exposing only a controlled interface. Like a vending machine — you press buttons (public interface), but the internal mechanism is sealed away. You can't reach in and grab items or mess with the coin counter.

### Example

```cpp
class BankAccount {
private:
    std::string owner_;
    double balance_;

    void log(const std::string& action, double amount) const {
        std::cout << "[LOG] " << action << ": $" << amount << '\n';
    }

public:
    BankAccount(const std::string& owner, double initial = 0)
        : owner_(owner), balance_(initial) {}

    // ── Controlled access ──
    double getBalance() const { return balance_; }

    void deposit(double amount) {
        if (amount > 0) {                 // Validation!
            balance_ += amount;
            log("Deposit", amount);
        }
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance_) {    // Validation!
            balance_ -= amount;
            log("Withdraw", amount);
            return true;
        }
        return false;                     // Insufficient funds
    }
};

int main() {
    BankAccount acc("Alice", 1000);
    acc.deposit(500);                     // [LOG] Deposit: $500
    acc.withdraw(200);                    // [LOG] Withdraw: $200
    std::cout << "Balance: $" << acc.getBalance() << '\n';  // $1300

    // acc.balance_ = 999999;            // ❌ ERROR: private!
    // acc.withdraw(-500);               // Does nothing: validation catches it
}
```

### Why This Matters

Without encapsulation, anyone could write `acc.balance_ = -999999` — instant chaos. By making `balance_` private, you **force** all changes to go through `deposit()` and `withdraw()`, which validate the input. This is how real-world banking works — you can't reach into the vault, you go through the teller.

---

## 🔹 6. Abstraction

> *📄 See full code → `06_abstraction.cpp`*

### The Concept

**Abstraction** means showing only *what* something does, not *how* it does it. An **interface** defines a contract: "Any class that implements me must provide these functions."

> When you drive a car, you use the steering wheel, pedals, and gear shift (the interface). You don't need to understand how the engine converts fuel into torque. That complexity is *abstracted away*.

### Example

```cpp
#include <iostream>
#include <string>

// ── Interface: pure contract, no implementation ──
class ILogger {
public:
    virtual void log(const std::string& message) = 0;   // Pure virtual
    virtual ~ILogger() = default;
};

// ── Concrete implementation #1 ──
class ConsoleLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[CONSOLE] " << message << '\n';
    }
};

// ── Concrete implementation #2 ──
class FileLogger : public ILogger {
public:
    void log(const std::string& message) override {
        std::cout << "[FILE] " << message << " (would write to disk)\n";
    }
};

// ── Code that depends on the INTERFACE, not the implementation ──
void processOrder(ILogger& logger) {
    logger.log("Processing order...");
    logger.log("Order complete.");
}

int main() {
    ConsoleLogger console;
    FileLogger file;

    processOrder(console);   // Uses console logging
    processOrder(file);      // Uses file logging — same function!
}
```

### 🖥️ Output

```
[CONSOLE] Processing order...
[CONSOLE] Order complete.
[FILE] Processing order... (would write to disk)
[FILE] Order complete. (would write to disk)
```

`processOrder` doesn't know or care whether it's logging to the console, a file, or a database. It only knows the **interface**: "there's a `.log()` method." This makes code easy to extend — add a `DatabaseLogger` without changing `processOrder` at all.

---

## 🔹 7. Operator Overloading

> *📄 See full code → `07_operator_overloading.cpp`*

### The Concept

**Operator overloading** lets you define what `+`, `-`, `==`, `<<`, etc. mean for **your own classes**. Without it, `Vector2D a + Vector2D b` wouldn't compile. With it, your custom types work as naturally as built-in types.

### Example

```cpp
#include <iostream>

class Vector2D {
    double x_, y_;

public:
    Vector2D(double x = 0, double y = 0) : x_(x), y_(y) {}

    // ── Addition ──
    Vector2D operator+(const Vector2D& rhs) const {
        return Vector2D(x_ + rhs.x_, y_ + rhs.y_);
    }

    // ── Compound assignment ──
    Vector2D& operator+=(const Vector2D& rhs) {
        x_ += rhs.x_;
        y_ += rhs.y_;
        return *this;
    }

    // ── Equality ──
    bool operator==(const Vector2D& rhs) const {
        return x_ == rhs.x_ && y_ == rhs.y_;
    }

    // ── Stream output (must be friend to access private members) ──
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
        os << "(" << v.x_ << ", " << v.y_ << ")";
        return os;
    }
};

int main() {
    Vector2D a(3, 4), b(1, 2);

    Vector2D sum = a + b;              // Calls operator+
    std::cout << "a + b = " << sum << '\n';            // (4, 6)
    std::cout << "a == b? " << (a == b) << '\n';       // 0 (false)

    a += b;                            // Calls operator+=
    std::cout << "a after += b: " << a << '\n';        // (4, 6)
}
```

### 🖥️ Output

```
a + b = (4, 6)
a == b? 0
a after += b: (4, 6)
```

### How This Works

When you write `a + b`, the compiler translates it to `a.operator+(b)`. You're defining what that function does for your class. The `friend` keyword for `operator<<` gives the function access to private members without being a class method — necessary because the left-hand side is `std::ostream`, not your class.

---

## 🔹 8. Special Members & The Rule of Five

> *📄 See full code → `08_special_members.cpp`*

### The Concept

When your class manages a **resource** (dynamic memory, file handle, network connection), the compiler's default copy behavior (shallow copy — just copy the pointer) is **dangerous**. You must write all five special members to handle resources correctly.

```
  The Rule of Five:
  ┌──────────────────────────────────────────────────────────────┐
  │  If you define ANY of these, you should define ALL of them:  │
  │                                                              │
  │  1. Destructor              ~Buffer()                        │
  │  2. Copy Constructor         Buffer(const Buffer&)           │
  │  3. Copy Assignment          operator=(const Buffer&)        │
  │  4. Move Constructor         Buffer(Buffer&&)                │
  │  5. Move Assignment          operator=(Buffer&&)             │
  └──────────────────────────────────────────────────────────────┘
```

### Example

```cpp
#include <iostream>
#include <algorithm>   // std::copy, std::swap

class Buffer {
    int* data_;
    size_t size_;

public:
    // ── 0. Constructor ──
    explicit Buffer(size_t size)
        : data_(new int[size]{}), size_(size) {
        std::cout << "  Constructed (size=" << size << ")\n";
    }

    // ── 1. Destructor ──
    ~Buffer() {
        std::cout << "  Destroyed (size=" << size_ << ")\n";
        delete[] data_;
    }

    // ── 2. Copy Constructor (deep copy) ──
    Buffer(const Buffer& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  Copy constructed\n";
    }

    // ── 3. Copy Assignment (copy-and-swap idiom) ──
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            Buffer temp(other);                // Make a copy
            std::swap(data_, temp.data_);      // Swap with the copy
            std::swap(size_, temp.size_);
        }   // temp's destructor cleans up old data
        std::cout << "  Copy assigned\n";
        return *this;
    }

    // ── 4. Move Constructor (steal resources) ──
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "  Move constructed\n";
    }

    // ── 5. Move Assignment ──
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        std::cout << "  Move assigned\n";
        return *this;
    }

    size_t size() const { return size_; }
};

int main() {
    Buffer a(10);                  // Constructor
    Buffer b = a;                  // Copy constructor
    Buffer c(5);
    c = a;                         // Copy assignment
    Buffer d = std::move(a);       // Move constructor (a is now empty)
}
```

### How This Works

| Operation | What Happens |
|-----------|-------------|
| **Copy construct** | Allocate NEW memory, copy data byte-by-byte. Like photocopying a book — two independent copies exist. |
| **Copy assign** | Uses the **copy-and-swap idiom**: make a temporary copy, swap internals. If the copy fails (throws), the original is untouched. **Exception safe.** |
| **Move construct** | Steal the pointer from `other`, set `other` to null. Like handing someone your book instead of photocopying it. **Fast — no allocation.** |
| **Move assign** | Delete your current data, steal from `other`. |

> 💡 **Rule of Zero:** If your class only uses `std::string`, `std::vector`, `unique_ptr`, etc. (types that manage themselves), you don't need to write **any** special members. The compiler's defaults do the right thing. Only write the Five when you manage raw resources like `new`/`delete`.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <em>Next chapter → <strong>Chapter 3: Memory Management</strong></em><br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
