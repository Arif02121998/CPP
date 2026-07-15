# Chapter 8 — Anti-Patterns & Common Mistakes

> *Level: Advanced*
> Back to [Table of Contents](README.md)

Knowing what **not** to do is as valuable as knowing best practices. An
**anti-pattern** is a common "solution" that looks reasonable but reliably creates
more problems than it solves. This chapter catalogs the classics — their symptoms,
why they're harmful, and the fix.

---

## 8.1 What Is an Anti-Pattern?

**An anti-pattern is a recurring, tempting-but-counterproductive approach to a
problem.** Unlike a simple bug, it's a *design-level* mistake that often passes
review and only reveals its cost as the system grows.

```text
   PATTERN     = proven solution that recurs because it WORKS
   ANTI-PATTERN= tempting "solution" that recurs but causes HARM
```

---

## 8.2 Inheritance Anti-Patterns

### The God Object / Blob
**Symptom:** one giant class that knows and does everything.
**Why harmful:** violates SRP; impossible to test, reuse, or change safely;
becomes a bottleneck for every developer.
**Fix:** decompose by responsibility ([SRP, Ch.6](06_solid_principles.md)); apply
composition.

```text
   class ApplicationManager {
       handleUsers(); processPayments(); renderUI();
       sendEmails(); generateReports(); manageDatabase();  // ← everything!
   }
   → split into focused collaborators.
```

### Inheritance for Code Reuse (not "is-a")
**Symptom:** extending a class just to grab its methods (`Stack extends ArrayList`).
**Why harmful:** tight coupling, fragile base class, broken contracts (exposes
methods that don't belong), LSP violations.
**Fix:** **composition + delegation** ([Ch.5](05_relationships_and_composition.md)).

### Yo-Yo Problem (deep inheritance)
**Symptom:** to understand one method you must scroll up and down a tall
inheritance tree across many files.
**Why harmful:** cognitive overload; behavior is scattered; fragile.
**Fix:** flatten hierarchies; favor composition; keep trees shallow (often ≤ 2–3).

### Refused Bequest (LSP violation)
**Symptom:** a subclass overrides inherited methods to throw
"UnsupportedOperation" or do nothing.
**Why harmful:** the subclass isn't truly an "is-a"; substitutability breaks.
**Fix:** restructure the hierarchy or use composition (the Penguin/fly fix, Ch.5).

```text
   class Penguin extends Bird {
       void fly() { throw new UnsupportedOperationException(); }  // ← refused bequest
   }
   → compose a FlyBehavior instead.
```

---

## 8.3 Encapsulation & Abstraction Anti-Patterns

### Anemic Domain Model
**Symptom:** classes are just bags of getters/setters with **no behavior**; all
logic lives in separate "service" classes.
**Why harmful:** it's procedural code wearing an OOP costume; invariants aren't
protected; data and behavior are separated again.
**Fix:** put behavior **with** the data it governs (rich domain objects that guard
their own invariants — [Ch.2](02_encapsulation_and_abstraction.md)).

```text
   class Order { getItems(); setStatus();  /* no rules, no behavior */ }
   class OrderService { /* ALL the logic lives here */ }
   → move order rules INTO Order; expose behavior, not raw setters.
```

### Getter/Setter Overload (Encapsulation Theater)
**Symptom:** a public getter and setter for every private field.
**Why harmful:** the fields are effectively public; no invariants enforced.
**Fix:** expose **behavior**, not data; make fields immutable where possible
([Ch.2](02_encapsulation_and_abstraction.md)).

### Leaky Abstraction
**Symptom:** an interface forces callers to know internal details (e.g., "call
`connect()` before `query()`, and don't forget `flush()`").
**Why harmful:** the abstraction fails its one job — hiding complexity.
**Fix:** design the interface around client *intent*; hide ordering/lifecycle inside.

---

## 8.4 Over-Engineering Anti-Patterns

### Over-Abstraction / "Astronaut Architecture"
**Symptom:** layers of interfaces and factories for things with a single
implementation (`AbstractFactoryFactoryBean`).
**Why harmful:** indirection without benefit; hard to follow; slows everyone down.
**Fix:** apply **YAGNI** ("You Aren't Gonna Need It") — abstract only when a second
implementation or real change is imminent.

```text
   IFooStrategyFactoryProvider → FooFactory → IFoo → FooImpl
   ...for a thing that has exactly ONE implementation and never changes.
   → just write the class.
```

### Premature Optimization / Premature Generalization
**Symptom:** building flexible, generic frameworks before any concrete need.
**Why harmful:** complexity now for benefits that may never materialize.
**Fix:** solve today's problem simply; refactor toward abstraction when the second
case actually appears (the "Rule of Three").

### Pattern Obsession (Hammer & Nail)
**Symptom:** forcing design patterns everywhere because you just learned them.
**Why harmful:** patterns add structure *and* cost; misapplied, they only add cost.
**Fix:** reach for a pattern when you feel the problem it solves
([Ch.7](07_design_patterns_and_thinking.md)).

> *"When all you have is a hammer, everything looks like a nail."*

---

## 8.5 Coupling Anti-Patterns

### Tight Coupling to Concretes
**Symptom:** `new ConcreteClass()` scattered through high-level code.
**Why harmful:** can't swap implementations, can't test in isolation.
**Fix:** depend on abstractions; inject dependencies ([DIP, Ch.6](06_solid_principles.md)).

### Circular Dependencies
**Symptom:** class A depends on B, which depends on A.
**Why harmful:** can't understand, test, or compile/modularize either in isolation.
**Fix:** introduce an interface to break the cycle; rethink responsibilities.

```text
   Order ──► Customer ──► Order   (cycle!)
   → extract an interface or mediator; invert one dependency.
```

### Singleton Abuse (Global State)
**Symptom:** Singletons used as convenient global variables.
**Why harmful:** hidden dependencies, non-deterministic tests, concurrency hazards.
**Fix:** inject the dependency explicitly; reserve Singleton for genuinely single
resources, and even then access via an interface.

---

## 8.6 The Mistake → Principle Map

Each mistake maps to the principle it violates:

| Anti-Pattern | Violates | Fix Direction |
|--------------|----------|---------------|
| God Object | SRP | Decompose by responsibility |
| Inheritance for reuse | "Favor composition" / LSP | Composition + delegation |
| Refused Bequest | LSP | Correct the hierarchy |
| Anemic Model | Encapsulation | Behavior with data |
| Getter/Setter overload | Encapsulation | Expose behavior |
| Leaky Abstraction | Abstraction | Intent-based interface |
| Tight coupling to concretes | DIP | Depend on abstractions, inject |
| `if/switch` on type | OCP | Polymorphism |
| Fat interface | ISP | Segregate interfaces |
| Over-abstraction | YAGNI / KISS | Simplify; abstract on demand |
| Pattern obsession | KISS | Use patterns purposefully |

---

## 8.7 Guiding Counter-Principles

When in doubt, these keep you balanced between under- and over-engineering:

```text
   KISS  — Keep It Simple, Stupid       → simplest design that works
   YAGNI — You Aren't Gonna Need It      → don't build for imagined futures
   DRY   — Don't Repeat Yourself         → eliminate duplication (but don't over-abstract)
   Rule of Three — abstract after the 3rd duplication, not the 1st
   Principle of Least Astonishment — code should behave as readers expect
```

> **The balance:** SOLID and patterns fight *complexity*; KISS/YAGNI fight
> *over-engineering*. Good design lives in the tension between them — apply
> structure proportional to the problem's real complexity and expected change.

---

## Chapter 8 Cheat Sheet

```text
INHERITANCE SMELLS
   God Object          → SRP violation → decompose
   Inheritance-for-reuse → use composition + delegation
   Yo-Yo (deep trees)  → flatten / compose
   Refused Bequest     → LSP violation → fix hierarchy

ENCAPSULATION SMELLS
   Anemic Model        → put behavior with data
   Getter/Setter overload → expose behavior, not data
   Leaky Abstraction   → design around client intent

OVER-ENGINEERING SMELLS
   Over-abstraction    → YAGNI; abstract on demand
   Premature generalization → solve today's problem
   Pattern obsession   → use patterns purposefully

COUPLING SMELLS
   Tight coupling to concretes → DIP + injection
   Circular dependencies → break with an interface
   Singleton abuse     → inject instead of global state

COUNTER-PRINCIPLES: KISS · YAGNI · DRY · Rule of Three · Least Astonishment
```

**Key Takeaways**
- Anti-patterns are design-level mistakes that look reasonable but scale badly.
- The biggest OOP traps: God objects, inheritance-for-reuse, anemic models, and
  over-abstraction.
- Every anti-pattern maps to a violated principle and a clear fix.
- Balance SOLID/patterns (anti-complexity) with KISS/YAGNI (anti-over-engineering).

---

*Next: [Chapter 9 — Interview Preparation Bank](09_interview_questions.md)*
