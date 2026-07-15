# Chapter 7 — Design Patterns (Intro) & Design Thinking

> *Level: Advanced*
> Back to [Table of Contents](README.md)

Design patterns are **proven, reusable solutions to recurring design problems**.
This chapter introduces the three classic categories, walks through the most
important patterns conceptually, and frames the *design thinking* that decides
when to use them.

> For language-specific C++ implementations, see
> [Advanced C++ — Design Patterns](../CPP_Advanced/04_design_patterns.md). This
> chapter stays **conceptual and language-agnostic**.

---

## 7.1 What Is a Design Pattern?

### Definition
**A design pattern is a named, general, reusable solution to a commonly occurring
problem in software design.** It's not code you copy — it's a *template* for how to
structure classes and objects to solve a problem.

> **Analogy:** In architecture, "build an arch to span an opening" is a pattern —
> a reusable idea, adapted to each building. Patterns in software are the same:
> tried-and-true *shapes* for solving recurring problems.

### Why Patterns Exist
- Capture **expert design knowledge** in a shareable form.
- Provide a **common vocabulary** ("just use a Factory here").
- Avoid reinventing solutions to known problems.
- Encode SOLID principles ([Ch.6](06_solid_principles.md)) into concrete structures.

> **Caution up front:** Patterns are tools, not goals. Forcing patterns where
> they're not needed is an anti-pattern ([Ch.8](08_antipatterns_and_mistakes.md)).
> Reach for a pattern when you *feel the problem it solves*.

---

## 7.2 The Three Categories (GoF)

```text
   CREATIONAL        STRUCTURAL          BEHAVIORAL
   how objects       how objects are     how objects
   are CREATED       COMPOSED            INTERACT/communicate
   ┌──────────────┐  ┌────────────────┐  ┌──────────────────┐
   │ Singleton    │  │ Adapter        │  │ Strategy         │
   │ Factory      │  │ Decorator      │  │ Observer         │
   │ Abstract     │  │ Facade         │  │ Command          │
   │  Factory     │  │ Composite      │  │ Iterator         │
   │ Builder      │  │ Proxy          │  │ State            │
   │ Prototype    │  │ Bridge         │  │ Template Method  │
   └──────────────┘  └────────────────┘  └──────────────────┘
```

| Category | Concern | Question Answered |
|----------|---------|-------------------|
| **Creational** | Object creation | "How do I create objects flexibly?" |
| **Structural** | Object composition | "How do I assemble objects into bigger structures?" |
| **Behavioral** | Object interaction | "How do objects collaborate and share responsibility?" |

---

## 7.3 Creational Patterns (Selected)

### Singleton — *one instance, global access*
**Problem:** exactly one instance should exist (e.g., a configuration or logger).
**Solution:** the class controls its own single instance and provides global
access to it.

```text
   getInstance() → always returns the same single object
```
> **Caution:** Singletons are global state in disguise — they hurt testability and
> coupling. Often a code smell; prefer dependency injection ([Ch.6 DIP](06_solid_principles.md)).

### Factory Method — *defer which class to instantiate*
**Problem:** code shouldn't hard-code concrete classes to create.
**Solution:** a method decides which concrete type to build; callers depend on the
abstraction.

```text
   ShapeFactory.create("circle") → returns a Shape (caller doesn't know/care which)
   Adding a new shape → extend the factory, not every call site (Open/Closed).
```

### Builder — *construct complex objects step by step*
**Problem:** an object needs many optional parts; giant constructors are unwieldy.
**Solution:** assemble the object incrementally through a builder.

```text
   new PizzaBuilder().size("L").cheese().mushrooms().build();
```

---

## 7.4 Structural Patterns (Selected)

### Adapter — *make incompatible interfaces work together*
**Problem:** you have a class with the wrong interface for your code.
**Solution:** wrap it in an adapter that exposes the expected interface.

```text
   YourCode → expects «MediaPlayer»
   Adapter wraps a legacy AudioLib → presents MediaPlayer interface
```
> **Analogy:** A travel power-plug adapter — same electricity, compatible shape.

### Decorator — *add behavior without subclassing*
**Problem:** you want to add responsibilities to objects dynamically.
**Solution:** wrap an object in a decorator that adds behavior and forwards the
rest — composition over inheritance in action.

```text
   coffee = new Milk(new Sugar(new Espresso()));   // stack behaviors at runtime
```

### Facade — *a simple front for a complex subsystem*
**Problem:** a subsystem is complex; clients want a simple entry point.
**Solution:** provide one unified, high-level interface (pure abstraction, Ch.2).

```text
   client → ComputerFacade.start() → [CPU, Memory, Disk... orchestrated inside]
```

---

## 7.5 Behavioral Patterns (Selected)

### Strategy — *swap an algorithm at runtime*
**Problem:** you need interchangeable behaviors (e.g., different sort/pay methods).
**Solution:** encapsulate each algorithm behind a common interface; compose it in.

```text
   class Context { Strategy s; execute() { s.run(); } }
   set s = QuickSort or MergeSort or BubbleSort — swapped freely.
```
> This is the **composition-over-inheritance** fix from
> [Chapter 5](05_relationships_and_composition.md) (the penguin/fly example).

### Observer — *publish/subscribe notifications*
**Problem:** many objects must react when one object changes.
**Solution:** subjects maintain a list of observers and notify them on change.

```text
   Subject (e.g., a Stock) ──notify──► [Observer1, Observer2, Observer3]
   Observers subscribe/unsubscribe freely; loose coupling.
```
> **Analogy:** A newsletter — subscribers get updates without the publisher
> knowing who they individually are.

### Template Method — *fixed skeleton, customizable steps*
**Problem:** several algorithms share a structure but differ in some steps.
**Solution:** base defines the skeleton; subclasses override specific steps.

```text
   prepareRecipe(): boilWater() → brew() → pour() → addCondiments()
   Tea and Coffee override brew()/addCondiments(); skeleton stays fixed.
```

---

## 7.6 Patterns Encode SOLID

Patterns aren't arbitrary — they're crystallized SOLID:

| Pattern | Principle It Embodies |
|---------|------------------------|
| Strategy | OCP + DIP (swap behavior via abstraction) |
| Factory | OCP + DIP (create via abstraction) |
| Decorator | OCP (extend without modifying) + composition |
| Observer | OCP + loose coupling |
| Adapter | DIP (depend on expected interface) |

---

## 7.7 Design Thinking — The Skill Behind the Patterns

Patterns are *answers*; **design thinking** is knowing *which question you're
solving*. Three guiding goals:

### 1. Loose Coupling
Minimize how much one module must know about another. Depend on **interfaces**,
inject dependencies, prefer composition.

```text
   TIGHT:  ReportService → new MySQLDatabase()      (knows the concretion)
   LOOSE:  ReportService → «Database» (injected)    (knows only the contract)
```

### 2. High Cohesion / Reusability
Keep each unit focused on one job (SRP). Focused, well-named components get reused;
tangled ones get rewritten.

### 3. Scalability & Maintainability
Design so the system can **grow by addition, not modification** (OCP). New
features become new classes, not edits to fragile core code.

```text
   GOOD ARCHITECTURE SCORES HIGH ON:
   ├─ Low coupling        → change one part without breaking others
   ├─ High cohesion       → each part has a clear, single purpose
   ├─ Extensibility       → add features without rewriting
   ├─ Testability         → swap real parts for mocks (via DI)
   └─ Readability         → intent is obvious from structure
```

### The Design Decision Flow

```text
   1. What varies?            → encapsulate it behind an interface
   2. Is-a or has-a?          → prefer has-a (composition)
   3. One reason to change?   → split if not (SRP)
   4. Extend without editing? → introduce a polymorphic seam (OCP)
   5. Depending on concretes? → invert to abstractions (DIP)
   6. Recognized problem?     → consider a known pattern
```

---

## 7.8 Real-World System Example — A Payment Module

Applying the whole toolkit to one feature:

```text
   PaymentProcessor (high-level policy)
        │ depends on
        ▼
     «PaymentMethod»  ← abstraction (DIP, Strategy)
        ▲     ▲     ▲
        │     │     │
   CreditCard PayPal Crypto      ← add new methods without editing processor (OCP)

   Notifications via Observer: on success → notify Email, SMS, Ledger observers.
   Object creation via Factory: PaymentFactory.create("paypal").
```

- **Strategy/DIP:** `PaymentProcessor` depends on the `PaymentMethod` interface.
- **OCP:** adding "ApplePay" = a new class, zero edits to the processor.
- **Observer:** receipts/notifications decoupled from payment logic.
- **Factory:** creation logic centralized and swappable.

This is OOP design maturity: pillars + relationships + SOLID + patterns working
together for a **loosely coupled, extensible** system.

---

## Chapter 7 Cheat Sheet

```text
PATTERN = named reusable solution to a recurring design problem

CATEGORIES
   Creational → how objects are CREATED   (Singleton, Factory, Builder)
   Structural → how objects are COMPOSED  (Adapter, Decorator, Facade)
   Behavioral → how objects INTERACT      (Strategy, Observer, Template Method)

KEY PATTERNS
   Strategy  : swap algorithm at runtime (composition)
   Factory   : create via abstraction (no hard-coded concretes)
   Observer  : publish/subscribe notifications
   Decorator : add behavior by wrapping (not subclassing)
   Adapter   : bridge incompatible interfaces
   Facade    : simple front over a complex subsystem

DESIGN THINKING GOALS
   Loose coupling · High cohesion · Reusability · Extensibility · Testability

DESIGN FLOW
   What varies? → encapsulate · has-a over is-a · SRP split ·
   polymorphic seam (OCP) · invert to abstractions (DIP) · then pick a pattern

WARNING: don't force patterns. Use them when you feel the problem they solve.
```

**Key Takeaways**
- Patterns are reusable, named solutions grouped as creational/structural/behavioral.
- They crystallize SOLID principles into concrete structures.
- Design thinking = pursuing loose coupling, high cohesion, and safe extensibility.
- Apply patterns to *real* problems — forcing them is an anti-pattern.

---

## Common Mistakes
1. **Pattern obsession** — forcing patterns where simple code suffices.
2. Overusing Singleton (hidden global state, hard to test).
3. Choosing a pattern before understanding the problem it solves.
4. Confusing the categories (creational vs structural vs behavioral).

---

*Next: [Chapter 8 — Anti-Patterns & Common Mistakes](08_antipatterns_and_mistakes.md)*
