# Design Patterns — A Concept-First Guide with Modern C++

> A two-layer study book:
> **Layer 1 (Primary):** Language-agnostic conceptual understanding — *why* patterns exist and *how* to think about them.
> **Layer 2 (Secondary):** Modern C++ (C++14/17) implementation insights — memory, lifetime, smart pointers, RAII, copy/move semantics.

---

## How to Read This Book

This book is organized as a progression: **Beginner → Intermediate → Advanced**.

- If you are new, read the **Foundations** chapter first. Patterns make little sense without OOP and SOLID.
- Each pattern is taught **concept-first**. Read the analogy and structure before the code.
- The C++ layer always comes *after* the concept. Don't skip the "Bad vs Improved" sections — that's where real learning happens.
- Use the **Cheatsheets** and **Comparisons** chapters for revision and interviews.

---

## Table of Contents

### Chapter 1 — [Foundations](01-Foundations.md)
1.1 What Is a Design Pattern?
1.2 A Short History (GoF)
1.3 The Naive OOP Problems Patterns Solve
1.4 OOP Fundamentals Refresher (Abstraction, Encapsulation, Inheritance, Polymorphism)
1.5 The SOLID Principles
1.6 Composition over Inheritance
1.7 Coupling and Cohesion
1.8 Classification of Patterns (Creational / Structural / Behavioral)
1.9 How to Choose a Pattern
1.10 C++ Prerequisites (RAII, smart pointers, move semantics, virtual dispatch)

### Chapter 2 — [Creational Patterns](02-Creational-Patterns.md)
2.1 Singleton *(Beginner)*
2.2 Factory Method *(Beginner → Intermediate)*
2.3 Abstract Factory *(Intermediate)*
2.4 Builder *(Intermediate)*
2.5 Prototype *(Intermediate)*

### Chapter 3 — [Structural Patterns](03-Structural-Patterns.md)
3.1 Adapter *(Beginner)*
3.2 Bridge *(Intermediate)*
3.3 Composite *(Intermediate)*
3.4 Decorator *(Intermediate)*
3.5 Facade *(Beginner)*
3.6 Flyweight *(Advanced)*
3.7 Proxy *(Intermediate)*

### Chapter 4 — [Behavioral Patterns](04-Behavioral-Patterns.md)
4.1 Observer *(Intermediate)*
4.2 Strategy *(Beginner)*
4.3 Command *(Intermediate)*
4.4 State *(Intermediate)*
4.5 Chain of Responsibility *(Intermediate)*
4.6 Mediator *(Intermediate)*
4.7 Memento *(Intermediate)*
4.8 Template Method *(Beginner)*
4.9 Visitor *(Advanced)*
4.10 Iterator *(Beginner → Intermediate)*

### Chapter 5 — [Pattern Comparisons](05-Pattern-Comparisons.md)
5.1 Factory Method vs Abstract Factory
5.2 Strategy vs State
5.3 Adapter vs Facade
5.4 Decorator vs Proxy
5.5 Decorator vs Strategy vs Chain of Responsibility
5.6 Bridge vs Strategy
5.7 Composite vs Decorator

### Chapter 6 — [Design Thinking & System Design](06-Design-Thinking-System-Design.md)
6.1 Patterns and Maintainability
6.2 Patterns and Scalability
6.3 Patterns and Decoupling
6.4 Mapping Patterns to SOLID
6.5 Case Study: Logging Framework
6.6 Case Study: GUI Framework
6.7 Case Study: Event-Driven System
6.8 Case Study: Automotive / Embedded Systems

### Chapter 7 — [Anti-Patterns & Common Mistakes](07-Anti-Patterns.md)
7.1 Overengineering / "Patternitis"
7.2 Singleton Abuse
7.3 Misusing Inheritance
7.4 God Objects and Leaky Abstractions
7.5 When NOT to Use a Pattern

### Chapter 8 — [Interview Preparation](08-Interview-Preparation.md)
8.1 Beginner Questions
8.2 Intermediate Questions
8.3 Advanced Questions
8.4 Pattern Identification Drills
8.5 Scenario-Based Design Problems
8.6 C++-Specific Pattern Questions

### Chapter 9 — [Cheatsheets](09-Cheatsheets.md)
9.1 One-Line Summaries
9.2 Pattern Selection Guide
9.3 Intent Keyword Map
9.4 C++ Idiom Quick Reference
9.5 Pattern → SOLID Matrix

---

## Conventions Used

| Symbol | Meaning |
|--------|---------|
| 🎯 | Intent / definition |
| ❓ | The problem it solves |
| 🌍 | Real-world analogy |
| 🧩 | Conceptual structure |
| ⚙️ | Step-by-step working |
| ⚖️ | Advantages & tradeoffs |
| ✅ | When to use |
| 🚫 | When NOT to use |
| 💻 | C++ implementation |
| 🧠 | C++ nuance (memory/lifetime/move/RAII) |

---

*Next: [Chapter 1 — Foundations →](01-Foundations.md)*
