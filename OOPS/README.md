# Object-Oriented Programming — A Language-Agnostic Textbook

> A complete, conceptual guide to OOP that is **not tied to any single language**.
> Examples appear in C++, Java, and Python only to *illustrate* ideas — the
> concepts are the focus.
> Designed for deep understanding, system-design discussions, and interviews.
> Format: Markdown (PDF/book ready).

---

## How to Use This Book

- **Read progressively.** Chapters go *Beginner → Intermediate → Advanced*.
- **Every concept follows the same teaching template:**
  1. **Definition** (conceptual, not syntax)
  2. **Why** it exists (what procedural programming couldn't do)
  3. **Real-world analogy**
  4. **How it works conceptually** (under the hood)
  5. **Benefits & trade-offs**
  6. **When to use / when NOT to use**
- **Multi-language snippets** show how the *same idea* differs across C++, Java,
  and Python.
- **Cheat sheets** end every chapter; **Chapter 9** is a full interview bank.

> Companion C++ books in this workspace: [C++11](../CPP11/README.md) ·
> [C++14](../CPP14/README.md) · [C++17](../CPP17/README.md) ·
> [Advanced C++](../CPP_Advanced/README.md). This OOP book is the
> **language-independent theory** beneath them.

---

## Table of Contents

### Front Matter
- [Preface: What OOP Really Is](#preface-what-oop-really-is)
- [From Procedural to Object-Oriented](#from-procedural-to-object-oriented)
- [The Four Pillars at a Glance](#the-four-pillars-at-a-glance)
- [Language Philosophy Comparison](#language-philosophy-comparison)

### Chapter 1 — Classes & Objects (and the Object Memory Model)
*File:* [`01_classes_and_objects.md`](01_classes_and_objects.md)

### Chapter 2 — Encapsulation & Abstraction
*File:* [`02_encapsulation_and_abstraction.md`](02_encapsulation_and_abstraction.md)

### Chapter 3 — Inheritance (All Types)
*File:* [`03_inheritance.md`](03_inheritance.md)

### Chapter 4 — Polymorphism & Dynamic Binding
*File:* [`04_polymorphism_and_dynamic_binding.md`](04_polymorphism_and_dynamic_binding.md)

### Chapter 5 — Object Relationships & Composition vs Inheritance
*File:* [`05_relationships_and_composition.md`](05_relationships_and_composition.md)

### Chapter 6 — The SOLID Principles
*File:* [`06_solid_principles.md`](06_solid_principles.md)

### Chapter 7 — Design Patterns (Intro) & Design Thinking
*File:* [`07_design_patterns_and_thinking.md`](07_design_patterns_and_thinking.md)

### Chapter 8 — Anti-Patterns & Common Mistakes
*File:* [`08_antipatterns_and_mistakes.md`](08_antipatterns_and_mistakes.md)

### Chapter 9 — Interview Preparation Bank
*File:* [`09_interview_questions.md`](09_interview_questions.md)

---

## Preface: What OOP Really Is

**Object-Oriented Programming is a way of structuring software around "objects"
— self-contained units that bundle together data (state) and the behavior
(operations) that act on that data.**

Instead of thinking *"what steps does the computer perform?"* (procedural),
OOP asks *"what things exist in my problem, what do they know, and what can they
do?"* This shift mirrors how humans naturally model the world: as interacting
entities with properties and responsibilities.

OOP is fundamentally about **managing complexity** in large systems through four
ideas — **encapsulation, abstraction, inheritance, and polymorphism** — supported
by sound **design principles** (SOLID) and reusable **design patterns**.

---

## From Procedural to Object-Oriented

### The Procedural World

In procedural programming, **data and functions are separate**. You have global
or passed-around data structures, and free functions that operate on them.

```text
   DATA (structs / records)            FUNCTIONS (operate on data)
   ┌───────────────────┐               ┌─────────────────────────┐
   │ account_balance   │ ◄──────────── │ deposit(account, amount)│
   │ account_owner     │ ◄──────────── │ withdraw(account, amt)  │
   └───────────────────┘               │ print_account(account)  │
                                        └─────────────────────────┘
```

### Why This Breaks Down at Scale

| Problem in Procedural Code | Consequence |
|----------------------------|-------------|
| Data is **exposed** to all functions | Anyone can corrupt it (no invariants) |
| Logic is **scattered** across functions | Hard to find "everything about an account" |
| **No enforced relationship** between data and its operations | Easy to call the wrong function on the wrong data |
| Changes ripple everywhere | Poor maintainability |
| Hard to **reuse** | Copy-paste leads to duplication |
| Adding a new "type" means editing **every** function | Poor extensibility |

### The OOP Answer

Bundle the data and its operations into one unit (a **class**), expose only a
safe **interface**, and let the object **protect its own invariants**.

```text
   OBJECT: BankAccount
   ┌─────────────────────────────────────┐
   │ STATE (private):                     │
   │   balance, owner                     │
   ├─────────────────────────────────────┤
   │ BEHAVIOR (public interface):         │
   │   deposit(amount)                    │
   │   withdraw(amount)   ← enforces rules│
   │   getBalance()                       │
   └─────────────────────────────────────┘
   Data and the code that guards it live together.
```

We will revisit this exact banking example throughout the book to show how each
OOP concept improves the design.

---

## The Four Pillars at a Glance

```text
                    OBJECT-ORIENTED PROGRAMMING
   ┌──────────────┬──────────────┬──────────────┬──────────────┐
   │ ENCAPSULATION│ ABSTRACTION  │ INHERITANCE  │ POLYMORPHISM │
   ├──────────────┼──────────────┼──────────────┼──────────────┤
   │ Hide internal│ Expose only  │ Reuse &      │ One interface│
   │ state; guard │ essential    │ specialize   │ many forms;  │
   │ invariants   │ behavior;    │ existing     │ behavior     │
   │ behind an    │ hide         │ types        │ varies by    │
   │ interface    │ complexity   │ ("is-a")     │ actual type  │
   └──────────────┴──────────────┴──────────────┴──────────────┘
         Ch.2           Ch.2           Ch.3           Ch.4
```

| Pillar | One-Sentence Essence | Chapter |
|--------|----------------------|---------|
| **Encapsulation** | Bundle data + behavior; hide and protect internal state. | [Ch.2](02_encapsulation_and_abstraction.md) |
| **Abstraction** | Model only the essential; hide irrelevant detail behind an interface. | [Ch.2](02_encapsulation_and_abstraction.md) |
| **Inheritance** | Build new types from existing ones to reuse and specialize. | [Ch.3](03_inheritance.md) |
| **Polymorphism** | The same call does the right thing based on the object's real type. | [Ch.4](04_polymorphism_and_dynamic_binding.md) |

> **Memory hook:** *"A PIE"* — **A**bstraction, **P**olymorphism,
> **I**nheritance, **E**ncapsulation.

---

## Language Philosophy Comparison

Different languages embrace OOP differently. Knowing this prevents you from
mistaking one language's rules for universal truths.

| Aspect | C++ | Java | Python |
|--------|-----|------|--------|
| OOP style | Multi-paradigm; OOP optional | OOP-first ("everything in a class") | Multi-paradigm; OOP optional |
| Everything an object? | No (primitives are not objects) | Almost (primitives + objects) | **Yes** (even functions, classes) |
| Multiple inheritance | **Yes** (of classes) | No (classes); yes via interfaces | **Yes** (with MRO) |
| Access control | `public`/`protected`/`private` (enforced) | enforced keywords | **Convention only** (`_name`, `__name`) |
| Interfaces | Abstract classes / concepts | `interface` keyword | Duck typing / ABCs / Protocols |
| Method dispatch | Static by default; `virtual` for dynamic | Dynamic by default | Always dynamic |
| Memory model | Manual / RAII / smart pointers | Garbage collected | Garbage collected (ref counting) |

> **Key insight:** OOP is a set of **ideas**. Each language picks *which* ideas to
> enforce and *how*. This book teaches the ideas; the snippets show the dialects.

---

## Master Cheat Sheet (OOP)

```text
PILLARS
  Encapsulation → hide state, expose safe interface, protect invariants
  Abstraction   → model essentials, hide complexity ("what" not "how")
  Inheritance   → "is-a"; reuse + specialize (use sparingly)
  Polymorphism  → one interface, many implementations (compile-time + runtime)

RELATIONSHIPS  (strength of coupling, weakest → strongest)
  Association  "uses-a"      →  Aggregation "has-a" (shared) →  Composition "owns-a" (exclusive)

DISPATCH
  Static  (compile-time): overloading, templates/generics, early binding
  Dynamic (runtime):      overriding, virtual methods, late binding (vtable)

SOLID
  S  Single Responsibility   — one reason to change
  O  Open/Closed             — open to extension, closed to modification
  L  Liskov Substitution     — subtypes must be substitutable for base types
  I  Interface Segregation   — many small interfaces > one fat interface
  D  Dependency Inversion    — depend on abstractions, not concretions

DESIGN GOALS
  Loose coupling · High cohesion · Reusability · Maintainability · Scalability

GOLDEN RULE
  "Favor composition over inheritance." · "Program to an interface, not an implementation."
```

---

*Next: [Chapter 1 — Classes & Objects](01_classes_and_objects.md)*
