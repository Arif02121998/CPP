# Chapter 5 — Object Relationships & Composition vs Inheritance

> *Level: Intermediate → Advanced*
> Back to [Table of Contents](README.md)

OOP is not just about individual objects — it's about **how objects relate**. This
chapter classifies relationships by coupling strength and settles the famous
debate: **composition vs inheritance**.

---

## 5.1 Why Relationships Matter

Real systems are **networks of collaborating objects**. *How* they're connected
determines coupling, flexibility, and maintainability.

```text
   Weakly coupled relationships → flexible, changeable, testable systems
   Tightly coupled relationships → rigid, fragile, hard-to-change systems
```

Choosing the right relationship is a core **design-thinking** skill (expanded in
[Ch.7](07_design_patterns_and_thinking.md)).

---

## 5.2 The Spectrum of Relationships

From **weakest** to **strongest** coupling:

```text
  WEAKER ◄─────────────────────────────────────────────► STRONGER

  Dependency   Association     Aggregation      Composition     Inheritance
  "uses-a"     "knows-a"       "has-a"          "owns-a"        "is-a"
  temporary    persistent      shared parts     exclusive parts strongest bind
  link         link            independent life  shared life     (Ch.3)
```

| Relationship | Verb | Lifetime coupling | Example |
|--------------|------|-------------------|---------|
| **Dependency** | "uses-a" | None (temporary) | `Order` uses a `PrinterService` to print |
| **Association** | "knows-a" | Independent | `Teacher` ↔ `Student` |
| **Aggregation** | "has-a" (shared) | Independent (whole/part outlive each other) | `Team` has `Player`s |
| **Composition** | "owns-a" (exclusive) | Shared (part dies with whole) | `House` owns `Room`s |
| **Inheritance** | "is-a" | Strongest (compile-time) | `Dog` is-a `Animal` |

---

## 5.3 Dependency ("uses-a")

The weakest link: one object **temporarily uses** another, often as a method
parameter or local — no stored reference.

```text
   class Order {
       void print(PrinterService p) {   // 'uses' PrinterService just here
           p.print(this);
       }
   }
   No long-term ownership; just a transient collaboration.
```

> **Analogy:** You *use* a pen to sign a form, then put it down. You don't *own* or
> *contain* it.

---

## 5.4 Association ("knows-a")

A **persistent structural** link between two independent objects. Neither owns the
other; both have their own lifecycle.

```text
   Teacher ◄──────────► Student      (bidirectional association)
   Both exist independently; they just reference each other.
```

Association can be one-to-one, one-to-many, or many-to-many, and uni- or
bi-directional.

```java
class Teacher { List<Student> students; }   // teacher knows students
class Student { Teacher advisor; }           // student knows advisor
// Deleting a Teacher does NOT delete the Students.
```

---

## 5.5 Aggregation ("has-a", shared/weak ownership)

A **whole/part** relationship where the parts can **exist independently** of the
whole and may be **shared**.

```text
   Team  ◇───────  Player          (◇ = hollow diamond = aggregation)
   A Team HAS players, but players exist before/after the team and can
   move to other teams. Deleting the team does NOT delete the players.
```

> **Analogy:** A university *department* has *professors*. If the department
> closes, the professors still exist (they transfer elsewhere). Shared, independent
> lifetime.

```java
class Team {
    private List<Player> players;       // aggregation
    Team(List<Player> players) {        // players passed IN from outside
        this.players = players;         // team does not "create"/own them
    }
}
```

---

## 5.6 Composition ("owns-a", exclusive/strong ownership)

A **whole/part** relationship where the part's **lifetime is bound to the whole**.
The whole *creates and destroys* its parts; parts are **not shared**.

```text
   House  ◆───────  Room           (◆ = filled diamond = composition)
   A House OWNS its Rooms. Destroy the house → the rooms cease to exist.
   Rooms are not shared with other houses.
```

> **Analogy:** A *human body* is composed of a *heart*. The heart doesn't have an
> independent life outside that body; its lifetime is the body's lifetime.

```java
class House {
    private final List<Room> rooms = new ArrayList<>();
    House() {
        rooms.add(new Room("Kitchen"));   // House CREATES its own rooms
        rooms.add(new Room("Bedroom"));   // they live & die with the House
    }
}
```

### Aggregation vs Composition — The Key Test

```text
   "If the whole is destroyed, do the parts survive?"
        YES → AGGREGATION (shared/independent lifetime)  ◇
        NO  → COMPOSITION  (exclusive/bound lifetime)    ◆
```

| | Aggregation | Composition |
|--|-------------|-------------|
| Ownership | Weak / shared | Strong / exclusive |
| Lifetime | Independent | Part dies with whole |
| Parts shared? | Yes | No |
| UML symbol | Hollow diamond ◇ | Filled diamond ◆ |
| Example | Team–Player | House–Room |

---

## 5.7 The Big Debate: Composition vs Inheritance

This is one of the most important design lessons in OOP.

### The Guidance
> **"Favor composition over inheritance."** — *Design Patterns* (Gang of Four)

### Why Composition Often Wins

| Concern | Inheritance ("is-a") | Composition ("has-a") |
|---------|----------------------|------------------------|
| Coupling | **Tight** (subclass depends on base internals) | **Loose** (depends on interface) |
| Flexibility | Fixed at compile time | Can swap parts at runtime |
| Reuse | Reuse by extending (rigid) | Reuse by delegating (flexible) |
| Fragile base class | Yes — base changes break subclasses | No |
| Multiple behaviors | Limited (diamond issues) | Easy — compose many parts |
| Testability | Harder (must instantiate hierarchy) | Easy (inject mock parts) |

### The Classic Example

```text
PROBLEM: model a "Penguin" that is a Bird but cannot fly.

INHERITANCE TRAP:
   class Bird { fly(); }           // assumes all birds fly
   class Penguin extends Bird {}   // ← inherits fly()... but penguins can't!
       → forced to override fly() to throw → broken "is-a" (LSP violation, Ch.6)

COMPOSITION FIX:
   interface FlyBehavior { fly(); }
   class CanFly  implements FlyBehavior { ... }
   class NoFly   implements FlyBehavior { /* do nothing / refuse */ }

   class Bird { FlyBehavior flight; }      // bird HAS-A fly behavior
   Penguin → Bird with NoFly behavior      // no broken contract; swappable!
```

This is exactly the **Strategy pattern** ([Ch.7](07_design_patterns_and_thinking.md)):
compose behavior instead of inheriting it.

### Delegation — How Composition Reuses

Composition reuses code by **delegating** to the contained object:

```java
class Stack<T> {
    private List<T> items = new ArrayList<>();   // HAS-A list (composition)
    void push(T x) { items.add(x); }             // delegate to the list
    T pop()        { return items.remove(items.size()-1); }
    // Stack exposes ONLY push/pop — not the whole List API.
}
```

Compare to the inheritance mistake (`Stack extends ArrayList`) from
[Chapter 3](03_inheritance.md): composition exposes a **clean, correct contract**.

---

## 5.8 When Inheritance Is Still Right

Composition is the default, but inheritance is correct when:
- There's a **genuine is-a** relationship **and** substitutability holds (LSP).
- You need **runtime polymorphism** over a stable base interface.
- The base is **stable** and designed for extension.

```text
   Shape ← Circle, Square     ✔ true is-a, polymorphic area() → inheritance is right
   Stack reusing List         ✘ not is-a → composition
```

---

## 5.9 Coupling & Cohesion (Design Quality Metrics)

These two terms summarize *why* relationship choice matters.

```text
   COUPLING = how dependent modules are on each other   → want it LOW
   COHESION = how focused a single module is            → want it HIGH

   GOAL: Loose coupling + High cohesion
         = modules that are independent (easy to change)
           and internally focused (easy to understand)
```

| | Tight Coupling (bad) | Loose Coupling (good) |
|--|----------------------|------------------------|
| Change impact | Ripples everywhere | Localized |
| Reuse | Hard | Easy |
| Testing | Hard (can't isolate) | Easy (inject substitutes) |
| Achieved via | Inheritance, concrete deps | Interfaces, composition, DI |

> Choosing weaker relationships (composition over inheritance, depending on
> interfaces over concretes) is *how* you achieve loose coupling — the foundation
> of the **Dependency Inversion Principle** ([Ch.6](06_solid_principles.md)).

---

## Chapter 5 Cheat Sheet

```text
RELATIONSHIP SPECTRUM (weak → strong coupling)
   Dependency  "uses-a"   transient (a parameter)
   Association "knows-a"  persistent link, independent lifetimes
   Aggregation "has-a"    shared parts, independent lifetime   ◇ hollow diamond
   Composition "owns-a"   exclusive parts, bound lifetime      ◆ filled diamond
   Inheritance "is-a"     strongest (compile-time) bind

AGGREGATION vs COMPOSITION
   "Destroy the whole — do parts survive?"  YES→aggregation  NO→composition

COMPOSITION vs INHERITANCE
   Favor COMPOSITION (has-a, loose, swappable, testable)
   Use INHERITANCE only for true "is-a" + substitutable + stable base
   Composition reuses via DELEGATION; enables Strategy pattern

DESIGN QUALITY
   Low COUPLING + High COHESION  → maintainable, reusable, testable
```

**Key Takeaways**
- Relationships range from weak (uses-a) to strong (is-a) coupling.
- Aggregation = shared/independent parts; Composition = exclusive/bound parts.
- "Favor composition over inheritance" — looser, swappable, testable.
- Aim for loose coupling + high cohesion via interfaces and composition.

---

## Common Mistakes
1. Modeling "has-a" as inheritance (the `Stack extends ArrayList` trap).
2. Confusing aggregation and composition (the lifetime test settles it).
3. Deep inheritance trees where composition would be flexible.
4. Tightly coupling to concrete classes instead of interfaces.

---

*Next: [Chapter 6 — The SOLID Principles](06_solid_principles.md)*
