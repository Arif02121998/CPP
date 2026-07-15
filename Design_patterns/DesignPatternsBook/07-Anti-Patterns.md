# Chapter 7 — Anti-Patterns & Common Mistakes

> Knowing patterns is half the skill. Knowing **when not to use them** is the other half. Misapplied patterns add complexity, hurt performance, and confuse readers. This chapter is the cautionary counterweight.

Sections:
- [7.1 Overengineering / "Patternitis"](#71-overengineering--patternitis)
- [7.2 Singleton Abuse](#72-singleton-abuse)
- [7.3 Misusing Inheritance](#73-misusing-inheritance)
- [7.4 God Objects and Leaky Abstractions](#74-god-objects-and-leaky-abstractions)
- [7.5 When NOT to Use a Pattern](#75-when-not-to-use-a-pattern)

---

## 7.1 Overengineering / "Patternitis"

**Symptom:** applying patterns *because you can*, not because a problem demands them. A 3-line task hidden behind a Factory, an Abstract Factory, and a Strategy with one implementation.

### Why it's harmful
- **Indirection tax:** every abstraction layer is something the reader must trace through. Five layers to find where work actually happens kills comprehension.
- **YAGNI violation:** "You Aren't Gonna Need It." Flexibility you build for imagined future requirements usually goes unused — and guesses the wrong axis of change.
- **Harder debugging:** more polymorphism = more "where does this actually go?" at runtime.

### The tells
```cpp
// Overengineered: a Strategy with exactly one strategy, forever.
struct GreetingStrategy { virtual std::string greet() = 0; };
struct EnglishGreeting : GreetingStrategy { std::string greet() override { return "Hi"; } };
class Greeter { std::unique_ptr<GreetingStrategy> s_; /* ... */ };

// Just write:
std::string greet() { return "Hi"; }
```

### The cure
- **Rule of Three:** don't abstract until you have *three* concrete cases of variation (or a near-certain, imminent second).
- **Start concrete, refactor to a pattern when the pain is real.** Patterns are a *destination* you refactor toward, not a starting template.
- Ask: *"What specific change does this abstraction make cheap, and is that change actually coming?"*

---

## 7.2 Singleton Abuse

The most over-used and most criticized pattern. Often labeled an anti-pattern outright.

### Why it's harmful
- **Hidden global state:** a `Singleton::instance()` call buried in a method is an *invisible dependency*. The function's signature lies about what it needs.
- **Untestable:** you can't substitute a mock; tests share mutable global state and interfere with each other; test order matters.
- **Hidden coupling:** any code can reach in and mutate shared state, creating action-at-a-distance bugs.
- **Concurrency hazards:** global mutable state is a magnet for data races.
- **Lifetime/ordering issues:** destruction order across singletons is hard to control.

### Better alternatives
- **Dependency Injection:** pass the dependency in (constructor parameter). Explicit, testable, mockable.
```cpp
// Instead of Logger::instance().log(...) buried inside:
class OrderService {
    ILogger& logger_;                       // injected dependency — explicit & mockable
public:
    explicit OrderService(ILogger& l) : logger_(l) {}
};
```
- **A single instance ≠ Singleton pattern:** you can create *one* instance at the composition root and pass it around. You get "one instance" without the global-access anti-pattern.
- If you *must* use a singleton, **hide it behind an interface** and inject it, so business logic depends on `ILogger`, not on `Logger::instance()`.

> **Rule:** "There is one of these" is a *lifetime* decision (make one). "Anyone can grab it globally" is the *anti-pattern*. Separate the two.

---

## 7.3 Misusing Inheritance

### Inheritance for code reuse (instead of "is-a")
Deriving `Stack` from `std::vector` to "reuse" its methods is a classic mistake — a `Stack` is *not* a `vector` (it shouldn't expose `insert`/`operator[]`). Use **composition**: hold a vector, expose only `push/pop`.

### Deep inheritance hierarchies
Five levels of inheritance make behavior hard to trace (which override runs?) and changes to a base ripple unpredictably (the **fragile base class** problem). Prefer shallow hierarchies + composition.

### LSP violations
The **Circle/Ellipse** and **Square/Rectangle** problem: if `Square` inherits `Rectangle` but overrides `setWidth` to also change height, code written against `Rectangle` (assuming independent dimensions) breaks. The subtype isn't substitutable — it violates LSP. Fix by *not* forcing the inheritance (they're different abstractions).

### The cure
- Prefer **composition over inheritance** ([§1.6](01-Foundations.md#16-composition-over-inheritance)).
- Use inheritance only for genuine **is-a** + **substitutability**.
- Patterns like **Strategy, Decorator, Bridge** exist precisely to *replace* inheritance-for-reuse.

---

## 7.4 God Objects and Leaky Abstractions

### God object
A class that knows/does too much (a 3000-line `Manager`/`Engine`/`Utils`). Often the *result of misusing* Facade or Mediator until they absorb all logic.
- **Symptom:** every feature edits the same class; merge conflicts everywhere; impossible to test in isolation.
- **Cure:** split by responsibility (SRP). A Facade should *delegate*, not *implement*. A Mediator should *coordinate*, not *contain all business logic*.

### Leaky abstraction
An abstraction that forces clients to understand its underlying implementation.
- E.g., a "generic repository" interface that throws SQL-specific exceptions, or an iterator that breaks if you don't know the container reallocates.
- **Cure:** make the abstraction *complete and honest* — or don't abstract at all. A half-abstraction is worse than none.

### Poltergeist / unnecessary indirection
Classes that exist only to pass a call along ("manager → controller → handler → doer") with no added value. Collapse them.

---

## 7.5 When NOT to Use a Pattern

A consolidated checklist. Reach for the *absence* of a pattern when:

| Don't use… | …when |
|---|---|
| **Singleton** | You just want convenient global access — inject instead |
| **Factory/Abstract Factory** | There's one product type with no foreseeable variation |
| **Builder** | The object has few fields — a constructor is clearer |
| **Prototype** | Construction is cheap and types are known |
| **Adapter** | You control both interfaces — just align them |
| **Decorator** | The feature set is fixed and small — a flag/subclass works |
| **Proxy** | No cross-cutting concern (lazy/secure/cache) justifies indirection |
| **Strategy/State** | There's a single behavior that won't vary |
| **Observer** | A direct one-to-one call is simpler and clearer |
| **Mediator** | Interactions are few — direct calls are fine |
| **Visitor** | The element hierarchy changes frequently (you'll edit every visitor) |
| **Any pattern** | You can't name the *specific* change it makes cheap |

### The three questions before adopting a pattern
1. **What varies, and is it *actually* going to vary?** (Not "might it theoretically.")
2. **Is this the simplest solution** that handles that variation? (Could a function, parameter, or `std::function` do it?)
3. **Does the indirection cost less than the change it enables?** (Readability is a real cost.)

> **Golden rule:** *Make it work, make it right, make it fast — in that order.* Reach for a pattern when duplicated/rigid code is causing **real, recurring pain**, not in anticipation of imagined futures. The best code is often the simplest code that solves today's problem cleanly.

---

*Next: [Chapter 8 — Interview Preparation →](08-Interview-Preparation.md)*
