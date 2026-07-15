# Chapter 6 — The SOLID Principles

> *Level: Advanced*
> Back to [Table of Contents](README.md)

The four pillars tell you *what* OOP is. **SOLID** tells you how to use it
*well*. These five principles (coined by Robert C. Martin) are the bridge from
"working code" to "maintainable, extensible design."

```text
   S  Single Responsibility Principle  — one reason to change
   O  Open/Closed Principle            — open to extension, closed to modification
   L  Liskov Substitution Principle    — subtypes must be substitutable
   I  Interface Segregation Principle  — many small interfaces > one fat one
   D  Dependency Inversion Principle   — depend on abstractions, not concretions
```

> **Common goal:** All five reduce **coupling** and increase **flexibility** so
> systems can grow and change without breaking.

---

## 6.1 S — Single Responsibility Principle (SRP)

### Definition
**A class should have only one reason to change** — i.e., it should do one job,
owned by one concern/actor.

### The Problem It Solves
A class that does many things has many reasons to change; a change for one reason
risks breaking the others. "God classes" become unmaintainable.

```text
VIOLATION — one class, many responsibilities:
   class Employee {
       calculatePay();      ← changes when payroll rules change
       saveToDatabase();    ← changes when persistence changes
       generateReport();    ← changes when reporting format changes
   }
   Three unrelated reasons to change → fragile, tangled.

FIX — split by responsibility:
   class Employee        { /* core data + business rules */ }
   class PayCalculator   { calculatePay(Employee e); }
   class EmployeeRepo    { save(Employee e); }
   class ReportGenerator { generate(Employee e); }
```

> **Analogy:** A Swiss Army knife seems convenient, but a kitchen full of
> single-purpose tools is easier to maintain, replace, and reason about.

### Benefit / Trade-off
- ✔ Easier to understand, test, and change in isolation.
- ✘ More classes; over-splitting can fragment cohesive logic.

---

## 6.2 O — Open/Closed Principle (OCP)

### Definition
**Software entities should be open for extension but closed for modification.**
You should add new behavior by **adding new code**, not editing existing,
tested code.

### The Problem It Solves
Every time you edit working code to add a case, you risk breaking it. OCP makes
extension safe.

```text
VIOLATION — must EDIT to extend (the if/switch smell):
   double area(Shape s) {
       if (s.type == CIRCLE) ...
       else if (s.type == SQUARE) ...
       // adding Triangle → must modify this function (and retest everything)
   }

FIX — extend via polymorphism (Ch.4):
   abstract class Shape { abstract double area(); }
   class Circle   extends Shape { double area() {...} }
   class Square   extends Shape { double area() {...} }
   class Triangle extends Shape { double area() {...} }   // ← NEW class only
   // existing code is untouched ("closed"); system is "open" to new shapes.
```

> **Mechanism:** OCP is achieved through **abstraction + polymorphism**. This is
> the practical payoff of [Chapter 4](04_polymorphism_and_dynamic_binding.md).

### Benefit / Trade-off
- ✔ Add features without destabilizing existing code.
- ✘ Requires upfront abstraction; over-applying adds needless indirection.

---

## 6.3 L — Liskov Substitution Principle (LSP)

### Definition
**Objects of a subtype must be substitutable for objects of their base type
without breaking correctness.** If `S` is a subtype of `T`, anywhere a `T` is
expected, an `S` must work correctly.

### The Problem It Solves
Inheritance that *looks* like "is-a" but breaks the base's contract creates
landmines. LSP is the litmus test for *correct* inheritance.

```text
VIOLATION — the classic Rectangle/Square:
   class Rectangle { setWidth(w); setHeight(h); area(); }
   class Square extends Rectangle { 
       setWidth(w)  → also sets height (to stay square)
       setHeight(h) → also sets width
   }
   // Code expecting a Rectangle:
   r.setWidth(5); r.setHeight(4); assert r.area() == 20;   // FAILS for Square (→16)!
   // A Square is NOT substitutable for a Rectangle → LSP violated.
```

Another example: the **Penguin extends Bird** problem from
[Chapter 5](05_relationships_and_composition.md) — overriding `fly()` to throw
breaks substitutability.

### Signs of LSP Violation
- Overriding a method to **throw "not supported."**
- Subclass **strengthens preconditions** or **weakens postconditions**.
- Callers must check `if (obj instanceof Square)` → the abstraction leaks.

### The Fix
Model the relationship correctly: don't force a false "is-a." Use composition, or
a better abstraction (e.g., immutable `Shape` with `area()`), or separate types.

> **LSP is the principle that keeps inheritance honest.** If substitutability
> fails, you don't have a true "is-a" relationship.

---

## 6.4 I — Interface Segregation Principle (ISP)

### Definition
**Clients should not be forced to depend on methods they do not use.** Prefer many
small, focused interfaces over one large, "fat" interface.

### The Problem It Solves
A fat interface forces implementers to provide methods they don't need (often by
throwing or leaving empty) and couples clients to irrelevant changes.

```text
VIOLATION — fat interface:
   interface Worker { work(); eat(); sleep(); }
   class Robot implements Worker {
       work();  ✔
       eat();   ✘ robots don't eat  → forced to throw / no-op
       sleep(); ✘ robots don't sleep
   }

FIX — segregate into focused interfaces:
   interface Workable { work(); }
   interface Feedable { eat(); }
   interface Sleepable{ sleep(); }
   class Robot implements Workable { work(); }            // only what it needs
   class Human implements Workable, Feedable, Sleepable { ... }
```

> **Analogy:** A multifunction printer that forces *every* user to learn faxing,
> scanning, and stapling — even those who only print — is poorly segregated.

### Cross-Language Note
Java/C# have explicit `interface`s; C++ uses abstract classes/concepts; Python
uses Protocols/duck typing. The principle is identical: **keep contracts small and
role-specific.**

### Benefit / Trade-off
- ✔ Implementers depend only on what they use; less coupling.
- ✘ More interfaces to manage.

---

## 6.5 D — Dependency Inversion Principle (DIP)

### Definition
Two parts:
1. **High-level modules should not depend on low-level modules. Both should depend
   on abstractions.**
2. **Abstractions should not depend on details. Details should depend on
   abstractions.**

### The Problem It Solves
When high-level policy code depends directly on low-level concrete classes, you
can't swap implementations (e.g., change database, mock for tests) without editing
the policy.

```text
VIOLATION — high-level depends on concrete low-level:
   class ReportService {
       MySQLDatabase db = new MySQLDatabase();   // ← hard-wired concretion
       // can't switch to Postgres or a mock without editing this class.
   }

FIX — both depend on an abstraction (interface):
   interface Database { save(data); }                 // abstraction
   class MySQLDatabase    implements Database { ... }  // detail depends on abstraction
   class PostgresDatabase implements Database { ... }
   class ReportService {
       private final Database db;                      // depends on abstraction
       ReportService(Database db) { this.db = db; }    // injected from outside
   }
```

```text
   BEFORE:  ReportService ───► MySQLDatabase     (high-level → low-level: bad)

   AFTER:   ReportService ───► «Database» ◄─── MySQLDatabase
                              (both point at the abstraction: inverted!)
```

### Dependency Injection (DI) — The Common Technique
DIP is usually *implemented* via **Dependency Injection**: pass dependencies in
(constructor/setter/parameter) rather than creating them inside.

> This is the backbone of testable architecture: inject a real DB in production, a
> mock in tests — **without changing the high-level code.** It builds directly on
> [Chapter 5](05_relationships_and_composition.md)'s loose-coupling goal.

---

## 6.6 How SOLID Principles Reinforce Each Other

```text
   SRP  →  small, focused classes (one reason to change)
   ISP  →  small, focused interfaces (the interface-level analog of SRP)
   OCP  →  extend via new types (needs polymorphism)
   LSP  →  keeps that polymorphism/inheritance CORRECT (substitutable)
   DIP  →  depend on the abstractions OCP/LSP rely on (loose coupling)

   Together: high cohesion (SRP/ISP) + loose coupling (DIP) +
             safe extension (OCP) + honest inheritance (LSP)
```

| Principle | Primary Goal | Achieved Via |
|-----------|--------------|--------------|
| SRP | Cohesion | Split responsibilities |
| OCP | Extensibility | Abstraction + polymorphism |
| LSP | Correct inheritance | Honest "is-a" / substitutability |
| ISP | Decoupling clients | Small role interfaces |
| DIP | Decoupling layers | Abstractions + dependency injection |

---

## 6.7 Benefits, Trade-offs, and When NOT To Overdo It

### Benefits
- Maintainable, testable, extensible systems.
- Localized changes; safe growth.
- A shared vocabulary for design reviews and interviews.

### Trade-offs / Cautions
- **Over-application** breeds excessive abstraction, tiny classes, and indirection
  ("AbstractFactoryFactory" syndrome) — see [Ch.8](08_antipatterns_and_mistakes.md).
- SOLID is a set of **guidelines**, not laws. Apply where complexity/ change
  justifies them; a small script doesn't need five interfaces.

> **Balance:** Use SOLID to manage *real* complexity and *anticipated* change — not
> to gold-plate trivial code.

---

## Chapter 6 Cheat Sheet

```text
S  Single Responsibility  — one class, one reason to change
O  Open/Closed            — extend via new code, don't modify existing
L  Liskov Substitution    — subtypes must be substitutable for base types
I  Interface Segregation  — many small interfaces > one fat interface
D  Dependency Inversion   — depend on abstractions; inject details

QUICK TESTS
   SRP: "How many unrelated reasons would force me to edit this class?"  (want 1)
   OCP: "Can I add a feature without editing existing code?"             (want yes)
   LSP: "Can I replace base with subclass and nothing breaks?"           (want yes)
   ISP: "Are implementers forced to stub methods they don't use?"        (want no)
   DIP: "Does my high-level code name a concrete low-level class?"       (want no)

PAYOFF: high cohesion + loose coupling + safe extensibility
CAUTION: don't over-engineer; SOLID are guidelines, not laws.
```

**Key Takeaways**
- SOLID turns the four pillars into *maintainable* design.
- OCP needs polymorphism; LSP keeps inheritance honest; DIP enables testable layers.
- They reinforce each other toward low coupling + high cohesion.
- Apply judiciously — over-application is itself an anti-pattern.

---

## Common Mistakes
1. God classes (SRP violation) — one class doing everything.
2. `if/switch` on type instead of polymorphism (OCP violation).
3. Subclasses that throw "unsupported" (LSP violation).
4. Fat interfaces forcing empty implementations (ISP violation).
5. `new ConcreteClass()` inside high-level code instead of injecting (DIP violation).

---

*Next: [Chapter 7 — Design Patterns & Design Thinking](07_design_patterns_and_thinking.md)*
