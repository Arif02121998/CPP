# Chapter 9 — Interview Preparation Bank (OOP)

> Back to [Table of Contents](README.md)
> Language-agnostic. Categories: **Beginner → Intermediate → Advanced**, plus
> **scenario-based**, **design problems**, and **tricky edge cases**.

---

## 9.1 Beginner — Conceptual

**Q1. What is the difference between a class and an object?**
A class is a blueprint/template defining state (attributes) and behavior
(methods). An object is a concrete instance of that class with its own data. One
class → many objects.

**Q2. What are the four pillars of OOP?**
Encapsulation (bundle data + behavior, hide internals), Abstraction (expose
essential behavior, hide complexity), Inheritance (derive new types from
existing), Polymorphism (one interface, many implementations).

**Q3. Difference between encapsulation and abstraction?**
Encapsulation is *implementation hiding* (how) — bundling and restricting access
to internal state. Abstraction is *design/essential modeling* (what) — exposing
only relevant behavior. Encapsulation is a mechanism; abstraction is a mindset.

**Q4. What is inheritance and why use it?**
A mechanism where a subclass acquires attributes/behavior of a superclass,
modeling an "is-a" relationship to enable reuse and polymorphism. Use it only for
genuine specialization.

**Q5. What is polymorphism? Name its two forms.**
The ability for one interface to take many forms. **Compile-time** (overloading,
generics/templates — resolved statically) and **run-time** (overriding via
dynamic dispatch — resolved at execution).

**Q6. What is a constructor and a destructor?**
A constructor initializes an object's state at creation. A destructor/finalizer
releases resources at end of life (explicit in C++ via `~T()`, automatic GC in
Java/Python with optional cleanup hooks).

---

## 9.2 Intermediate — Conceptual

**Q7. Overloading vs overriding?**
Overloading: same method name, **different signatures**, resolved at compile time
(static). Overriding: subclass redefines a base method with the **same
signature**, resolved at run time (dynamic dispatch).

**Q8. What is dynamic binding and how does it work conceptually?**
The call target is chosen at run time based on the object's actual type. Typically
implemented with a **vtable** (per-class table of method pointers) and a **vptr**
(per-object pointer to it); a virtual call indexes the vtable to find the override.

**Q9. Composition vs inheritance — which is preferred and why?**
"Favor composition over inheritance." Composition (has-a) is more flexible,
loosely coupled, and changeable at run time; inheritance (is-a) is rigid, exposes
base internals (fragile base class), and is compile-time fixed. Use inheritance
only for true is-a + polymorphism.

**Q10. Explain association, aggregation, and composition.**
All are "has-a" with increasing ownership strength. Association: objects just use
each other (no ownership). Aggregation: whole holds parts but parts outlive it
(shared, e.g., Team↔Player). Composition: whole owns parts; parts die with it
(e.g., House↔Room).

**Q11. What is an abstract class vs an interface?**
An abstract class may contain state + some implementation and can't be
instantiated (partial abstraction, "is-a"). An interface is a pure behavioral
contract (capability, "can-do"), typically no state. A class extends one abstract
class but can implement many interfaces.

**Q12. State the SOLID principles in one line each.**
- **S**RP: one reason to change per class.
- **O**CP: open for extension, closed for modification.
- **L**SP: subtypes must be substitutable for their base.
- **I**SP: prefer many small interfaces over one fat one.
- **D**IP: depend on abstractions, not concretions.

**Q13. What is loose vs tight coupling?**
Coupling is interdependence between modules. Tight coupling = changes ripple
widely, hard to test/reuse. Loose coupling (via interfaces/DIP) = modules interact
through stable abstractions, enabling independent change and testing.

---

## 9.3 Advanced — Conceptual & Tricky

**Q14. (Tricky) Why does the classic `Square extends Rectangle` violate LSP?**
A Square that constrains width == height breaks code that relies on
Rectangle's invariant "set width and height independently." Substituting a Square
silently changes behavior → LSP violation. Fix: don't model it via inheritance;
use a common `Shape` abstraction or composition.

**Q15. What is the diamond problem and how do languages handle it?**
With multiple inheritance, a class inherits two paths to a common ancestor,
creating ambiguity/duplicate state. C++ solves it with **virtual inheritance**
(single shared base) + explicit qualification. Java/C# forbid multiple class
inheritance (interfaces only; default methods need explicit resolution). Python
uses **C3 linearization (MRO)**.

**Q16. What is the fragile base class problem?**
Changes to a base class can unexpectedly break derived classes that depended on
its internal behavior, because inheritance exposes implementation, not just
interface. Mitigate with composition, stable/abstract base interfaces, and
documenting the inheritance contract.

**Q17. How is method dispatch different for static, instance, and virtual methods?**
Static methods: bound at compile time to a type (no dispatch). Non-virtual
instance methods: bound by static type. Virtual methods: dispatched at run time by
the object's dynamic type via the vtable.

**Q18. (Tricky) Can constructors be polymorphic / virtual?**
No — an object's type must be known to construct it, so constructors aren't
virtual. Calling a virtual method *from within* a constructor is dangerous: the
object isn't fully built, so overrides may run against an incomplete subclass
(C++ calls the base version; Java/Python may hit an uninitialized override).

**Q19. What's the difference between abstraction and encapsulation at the memory/dispatch level?**
Abstraction is a *design-time* separation of interface from implementation
(no runtime artifact by itself). Encapsulation is enforced by access rules
(compile-time in C++/Java, convention in Python). Neither directly equals the
vtable — that's the polymorphism mechanism.

**Q20. When does composition NOT beat inheritance?**
When you genuinely need run-time substitutability through a base type and a true
is-a relationship (e.g., `Shape` → `Circle`/`Square` rendered polymorphically), or
to leverage framework template-method hooks. Inheritance is the right tool for
polymorphic hierarchies; composition is right for reuse/flexibility.

---

## 9.4 Scenario-Based Questions

**S1. You have a `Bird` base class with `fly()`. Now you must add a penguin.
What's wrong and how do you fix it?**
Putting `fly()` on every Bird violates LSP (penguins can't fly). Fix: separate
capabilities into interfaces (`Flyable`, `Swimmable`) and compose them, so
`Penguin` implements `Swimmable` but not `Flyable`.

**S2. A `Report` class formats, prints, and emails reports. What principle is
violated and how do you refactor?**
SRP — three responsibilities/reasons to change. Split into `ReportFormatter`,
`ReportPrinter`, `ReportMailer`, each with one job, composed by a coordinator.

**S3. Adding a new payment method forces you to edit a giant `switch` in
`PaymentProcessor`. Redesign it.**
OCP/DIP violation. Define a `PaymentMethod` interface with `pay()`; implement
`CreditCard`, `PayPal`, etc.; have the processor depend on the interface. Adding a
method = a new class, no edits to the processor.

**S4. Your `OrderService` directly creates a `MySQLDatabase`. How do you make it
testable?**
Apply DIP: depend on a `Database`/`Repository` interface injected via the
constructor. In tests, inject an in-memory/mock implementation — no real DB
needed.

---

## 9.5 Design Problems (System Modeling)

**D1. Design a banking system (core classes).**
- `Account` (abstract): balance, `deposit()`, `withdraw()` (encapsulated balance).
- `SavingsAccount`, `CheckingAccount` : override interest/overdraft (polymorphism).
- `Customer` *aggregates* `Account`s (accounts can outlive a view of the customer
  per rules); `Transaction` records movements.
- `Bank` composes branches. Use DIP for persistence (`AccountRepository`).
- Apply SRP: separate `InterestCalculator`, `StatementGenerator`.

**D2. Design an inventory system.**
- `Product` (id, name, price), `InventoryItem` (product + quantity).
- `Warehouse` *composes* `InventoryItem`s (items belong to it).
- `StockPolicy` interface (reorder strategies) — Strategy pattern (OCP).
- `InventoryService` depends on `ProductRepository` abstraction (DIP).
- Observer pattern: notify `Dashboard`/`Alerts` on low stock.

**D3. Design a notification system supporting email, SMS, push.**
- `Notifier` interface with `send(message)`.
- `EmailNotifier`, `SmsNotifier`, `PushNotifier` implementations (polymorphism +
  OCP).
- `NotificationService` holds a list of `Notifier`s (composition); add channels
  without modifying the service. Optionally Decorator for retry/logging.

> **Interview tip:** narrate your reasoning — identify nouns→classes,
> verbs→methods, relationships→association/aggregation/composition, points of
> change→interfaces (OCP/DIP), and responsibilities→SRP splits.

---

## 9.6 Tricky Edge Cases (Rapid)

| Question | Sharp answer |
|----------|--------------|
| Is overloading polymorphism? | Yes — *ad-hoc/compile-time* polymorphism. |
| Can you override a private method? | No — it's not visible to subclasses (may be hidden, not overridden). |
| Does Python enforce `private`? | No — `_`/`__` are convention/name-mangling, not enforcement. |
| Default method dispatch in C++? | **Non-virtual** (static) unless declared `virtual`. |
| Default in Java/Python? | Virtual — all instance methods dispatch dynamically. |
| Calling virtual from a constructor? | Dangerous — object not fully constructed. |
| Can an interface have state? | Pure interfaces shouldn't; some languages allow constants/defaults. |
| Multiple inheritance of state in Java? | No — only interfaces (no shared state). |
| Composition or inheritance for code reuse? | Composition — reuse without is-a coupling. |
| Abstract class with no abstract methods? | Legal — still non-instantiable; used as a base. |

---

## 9.7 Rapid-Fire One-Liners

| Concept | One-liner |
|---------|-----------|
| Class vs object | Blueprint vs instance. |
| Encapsulation | Hide *how*; expose controlled access. |
| Abstraction | Show *what*; hide complexity. |
| Inheritance | "is-a" reuse + polymorphism. |
| Polymorphism | One interface, many forms. |
| Dynamic binding | Run-time method selection (vtable). |
| Aggregation vs composition | Shared parts vs owned parts (lifetime). |
| SRP | One reason to change. |
| OCP | Extend without modifying. |
| LSP | Subtypes substitutable. |
| ISP | No fat interfaces. |
| DIP | Depend on abstractions. |
| Coupling | Minimize inter-module dependence. |
| Cohesion | Maximize within-module focus. |
| Favor composition | Flexibility over rigid hierarchies. |

---

## Final OOP Recap

| Pillar / Topic | Master This |
|----------------|-------------|
| Classes & Objects | blueprint vs instance, memory model, identity vs equality |
| Encapsulation | implementation hiding, invariants, access control |
| Abstraction | interface vs implementation, abstract class vs interface |
| Inheritance | is-a, types, diamond problem, fragile base class |
| Polymorphism | compile-time vs run-time, vtable dispatch |
| Relationships | association/aggregation/composition, coupling/cohesion |
| SOLID | five principles + how they reinforce each other |
| Patterns | creational/structural/behavioral intent |
| Anti-patterns | inheritance overuse, god class, SOLID violations |

---

*End of the OOP book. Companion language-specific books:
[C++11 »](../CPP11/README.md) · [C++14 »](../CPP14/README.md) ·
[C++17 »](../CPP17/README.md) · [Advanced C++ »](../CPP_Advanced/README.md)*
