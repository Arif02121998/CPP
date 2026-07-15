# Chapter 4 — Polymorphism & Dynamic Binding

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

Polymorphism is what makes OOP *flexible and extensible*. This chapter separates
its two kinds, then opens the hood on **how dynamic dispatch actually works**.

---

## 4.1 Definition (Conceptual)

**Polymorphism (Greek: "many forms") is the ability of a single interface or
operation to behave differently depending on the actual type of the object
involved.** One name, many behaviors.

```text
   draw(shape)  →  if shape is a Circle   → draws a circle
                   if shape is a Square   → draws a square
                   if shape is a Triangle → draws a triangle
   ONE call site, MANY behaviors — chosen by the object's real type.
```

> **Analogy:** The verb **"speak"** applied to different animals: a dog *barks*, a
> cat *meows*, a cow *moos*. Same command (`speak()`), different behavior per type.

---

## 4.2 Why Polymorphism Exists — The Limitation It Removes

Without polymorphism, code must **check types explicitly** and branch:

```text
WITHOUT polymorphism (rigid, must edit to extend):
   void render(Shape s) {
       if      (s.type == CIRCLE)   drawCircle(s);
       else if (s.type == SQUARE)   drawSquare(s);
       else if (s.type == TRIANGLE) drawTriangle(s);
       // adding a new shape → must EDIT this function (and every similar one!)
   }

WITH polymorphism (open to extension):
   void render(Shape s) { s.draw(); }   // each shape knows how to draw itself
       // adding a new shape → just create a new class. NO edits here.
```

This directly enables the **Open/Closed Principle** ([Ch.6](06_solid_principles.md)):
*open to extension, closed to modification.* Polymorphism is the mechanism that
makes systems extensible without touching existing code.

---

## 4.3 The Two Kinds of Polymorphism

```text
                       POLYMORPHISM
            ┌────────────────────┴────────────────────┐
   COMPILE-TIME (static)                      RUN-TIME (dynamic)
   "ad-hoc / parametric"                      "subtype"
   resolved by COMPILER                       resolved at RUNTIME
   ┌──────────────────────┐                   ┌──────────────────────┐
   │ • Function overloading│                   │ • Method overriding   │
   │ • Operator overloading│                   │ • Virtual functions   │
   │ • Templates/generics  │                   │ • Late binding        │
   └──────────────────────┘                   └──────────────────────┘
   EARLY binding                              LATE binding
```

| | Compile-Time | Run-Time |
|--|--------------|----------|
| **Bound when** | Compilation | Execution |
| **Mechanism** | Overloading, generics/templates | Overriding via virtual methods |
| **Speed** | Faster (no indirection) | Slight overhead (indirection) |
| **Flexibility** | Fixed at compile time | Decided by actual object type |
| **Also called** | Early/static binding | Late/dynamic binding |

---

## 4.4 Compile-Time Polymorphism

### Function/Method Overloading
Same method name, **different parameter lists**; the compiler picks the match.

```cpp
// C++
int  add(int a, int b)             { return a + b; }
double add(double a, double b)     { return a + b; }
std::string add(std::string a, std::string b) { return a + b; }
add(2, 3);        // picks int version    — chosen at COMPILE time
add(2.0, 3.0);    // picks double version
```

> **Note:** Python does **not** support traditional overloading by signature (the
> last definition wins); it uses default args / `*args` / `singledispatch`
> instead. Another cross-language difference to remember.

### Operator Overloading
Giving operators (`+`, `==`, `<<`) meaning for user types.

```cpp
// C++ — '+' for a Vector type
Vector operator+(const Vector& a, const Vector& b);
Vector c = a + b;     // reads naturally; resolved at compile time
```

```python
# Python — via dunder methods
class Vector:
    def __add__(self, other): ...   # enables  a + b
```

> Java deliberately **omits** user-defined operator overloading (except built-in
> `+` for strings) — a design choice for simplicity.

### Generics / Templates
One algorithm, many types, resolved at compile time. (See C++ depth in
[Advanced C++ — templates](../CPP_Advanced/01_advanced_templates.md).)

```text
   max<T>(a, b)  works for int, double, string... compiler generates each version.
```

---

## 4.5 Run-Time Polymorphism (The Heart of OOP)

This is **subtype polymorphism**: a **base reference** points to a **derived
object**, and a call resolves to the derived class's override **at runtime**.

```cpp
// C++ — 'virtual' enables runtime dispatch
class Animal {
public:
    virtual void speak() const { std::cout << "..."; }   // virtual = overridable
    virtual ~Animal() = default;
};
class Dog : public Animal { public: void speak() const override { std::cout << "Woof"; } };
class Cat : public Animal { public: void speak() const override { std::cout << "Meow"; } };

void makeItSpeak(const Animal& a) { a.speak(); }   // doesn't know the concrete type!
makeItSpeak(Dog{});   // → "Woof"   ┐ decided at RUNTIME by the
makeItSpeak(Cat{});   // → "Meow"   ┘ object's ACTUAL type
```

```java
// Java — methods are virtual BY DEFAULT
class Animal { void speak() { System.out.println("..."); } }
class Dog extends Animal { void speak() { System.out.println("Woof"); } }
Animal a = new Dog();
a.speak();   // → "Woof" (dynamic dispatch)
```

```python
# Python — always dynamic
class Animal:
    def speak(self): print("...")
class Dog(Animal):
    def speak(self): print("Woof")
a: Animal = Dog()
a.speak()   # → "Woof"
```

> **Crucial cross-language rule:**
> - **C++:** static dispatch by default; you must say `virtual`.
> - **Java:** dynamic by default; `final` to prevent overriding.
> - **Python:** always dynamic.
> Forgetting `virtual` in C++ is a top source of bugs.

---

## 4.6 Under the Hood — How Dynamic Dispatch Works (The vtable)

This is the conceptual mechanism behind runtime polymorphism. (C++-flavored, but
the idea generalizes.)

```text
Each polymorphic class has a hidden VIRTUAL TABLE (vtable):
an array of pointers to the correct method implementations.

   Dog vtable                 Cat vtable
   ┌───────────────┐          ┌───────────────┐
   │ speak → Dog::speak │      │ speak → Cat::speak │
   │ ~Dog → Dog::~Dog   │      │ ~Cat → Cat::~Cat   │
   └───────────────┘          └───────────────┘

Each OBJECT stores a hidden pointer (vptr) to its class's vtable:

   Dog object:  [ vptr ●──────► Dog vtable ]
   Cat object:  [ vptr ●──────► Cat vtable ]
```

### A virtual call is a two-step indirection:

```text
   a.speak()  ──►  1. follow object's vptr → find its vtable
                   2. look up 'speak' slot → call that function
   The concrete function is discovered at RUNTIME, not compile time.
```

| Term | Meaning |
|------|---------|
| **vtable** | Per-class table of function pointers for virtual methods |
| **vptr** | Per-object hidden pointer to its class's vtable |
| **late binding** | Choosing the function at runtime via the vtable |

**Cost of dynamic dispatch:** one extra pointer per object (vptr) + one indirect
lookup per virtual call + it usually prevents inlining. Usually negligible, but it
explains why C++ makes it opt-in (`virtual`).

---

## 4.7 Static vs Dynamic Binding — Summary

```text
STATIC (early) binding:
   compiler knows EXACTLY which function to call.
   overloading, non-virtual calls, templates → faster, inlinable.

DYNAMIC (late) binding:
   actual function chosen at runtime by object's real type.
   virtual/overridden methods → flexible, extensible, slight overhead.
```

> **Interview phrasing:** *"Overloading is resolved at compile time (static
> binding); overriding is resolved at runtime (dynamic binding via the vtable)."*

---

## 4.8 Overloading vs Overriding (Don't Confuse Them!)

| | **Overloading** | **Overriding** |
|--|-----------------|----------------|
| Polymorphism kind | Compile-time | Run-time |
| Signature | **Different** params | **Same** signature |
| Relationship | Same class (or visible scope) | Base ↔ derived |
| Binding | Static / early | Dynamic / late |
| Purpose | Convenience: same name, varied inputs | Specialize base behavior |

```text
   OverLOADING  → same name, different parameters  (compile-time)
   OverRIDING   → same name, same parameters, base→derived (run-time)
```

---

## 4.9 Abstract Methods & Pure Interfaces

A base can declare a method with **no implementation**, forcing subclasses to
provide one. This guarantees a polymorphic interface.

```cpp
class Shape { public: virtual double area() const = 0; };  // pure virtual → abstract
```

Such a class is **abstract** — it cannot be instantiated; it exists to define a
**contract** for polymorphic use. This connects back to abstraction
([Ch.2](02_encapsulation_and_abstraction.md)) and forward to design patterns
([Ch.7](07_design_patterns_and_thinking.md)).

---

## 4.10 Benefits, Trade-offs, and When NOT To

### Benefits
- **Extensibility** — add new types without touching existing code (Open/Closed).
- **Decoupling** — callers depend on the base interface, not concrete types.
- **Cleaner code** — eliminates sprawling `if/switch` type checks.

### Trade-offs
- Runtime dispatch has slight performance cost (vtable indirection).
- Indirection can make control flow harder to trace.
- Requires careful base design (and `virtual` destructors in C++!).

### When NOT To
- Hot, performance-critical loops where the type is fixed → prefer static dispatch
  or templates.
- Only one implementation will ever exist → polymorphism is premature.

---

## Chapter 4 Cheat Sheet

```text
POLYMORPHISM = one interface, many behaviors (chosen by actual type)

COMPILE-TIME (static, early binding)
   overloading · operator overloading · generics/templates

RUN-TIME (dynamic, late binding)
   overriding · virtual methods · resolved via VTABLE

UNDER THE HOOD
   vtable = per-class table of method pointers
   vptr   = per-object pointer to its vtable
   virtual call = follow vptr → vtable → correct method (at runtime)

LANGUAGE DEFAULTS
   C++   : static by default → must mark 'virtual'
   Java  : dynamic by default → 'final' to lock
   Python: always dynamic

OVERLOADING (compile-time, diff params)  ≠  OVERRIDING (runtime, same signature)

Enables OPEN/CLOSED PRINCIPLE → extend without modifying.
```

**Key Takeaways**
- Two kinds: compile-time (overloading/generics) and run-time (overriding).
- Run-time dispatch works via vptr → vtable → method (late binding).
- Language defaults differ — `virtual` matters most in C++.
- Polymorphism replaces type-checking `if/switch` and powers Open/Closed.

---

## Common Mistakes
1. Confusing overloading with overriding.
2. Forgetting `virtual` in C++ (call binds statically → wrong method runs).
3. Missing a **virtual destructor** in C++ base classes → undefined behavior on delete.
4. Using runtime polymorphism in tight loops where static dispatch would do.

---

*Next: [Chapter 5 — Object Relationships & Composition vs Inheritance](05_relationships_and_composition.md)*
