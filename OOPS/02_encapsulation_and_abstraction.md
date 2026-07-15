# Chapter 2 — Encapsulation & Abstraction

> *Level: Beginner → Intermediate*
> Back to [Table of Contents](README.md)

These two pillars are often confused because they're closely related. This
chapter draws a sharp line between them and shows how they work together.

> **One-line distinction:**
> **Encapsulation** = *bundling + hiding* the data (an **implementation**
> concern: "keep the wires inside the box").
> **Abstraction** = *exposing only the essential* (a **design** concern: "show me
> only the buttons I need").

---

# Part A — Encapsulation

## 2.1 Definition (Conceptual)

**Encapsulation is the bundling of data and the methods that operate on it into a
single unit, combined with restricting direct access to that data from outside.**

Two halves:
1. **Bundling** — state and behavior live together in one class.
2. **Information hiding** — internal state is private; the outside world touches
   it only through a controlled interface.

> **Analogy — A medicine capsule:** the powder (data) is sealed inside a shell.
> You consume it through the intended interface (swallow it whole); you don't pry
> it open and rearrange the contents. The shell *protects* what's inside.

> **Analogy — A car:** you steer, accelerate, and brake (the interface). You don't
> reach into the engine while driving. The internals are **hidden and protected**.

---

## 2.2 Why Encapsulation Exists — The Limitation It Fixes

Without encapsulation, **invariants cannot be guaranteed**:

```text
NO ENCAPSULATION (public data):
   account.balance = -5000;     // ← anyone can corrupt state directly
   account.balance = "oops";    // ← wrong type, broken object

WITH ENCAPSULATION (private data + guarded interface):
   account.withdraw(5000);      // ← method rejects if funds insufficient
```

An **invariant** is a rule that must always hold (e.g., *balance ≥ 0*).
Encapsulation lets the object **enforce its own invariants**, because the only way
to change state is through methods that check the rules.

---

## 2.3 Access Control Mechanisms

| Visibility | Meaning | Who can access |
|------------|---------|----------------|
| **public** | Part of the interface | Everyone |
| **protected** | Internal but inheritable | The class + subclasses |
| **private** | Pure implementation detail | Only the class itself |

### Across Languages

```cpp
// C++ — access is ENFORCED by the compiler
class Account {
private:   double balance;            // truly inaccessible outside
public:    void deposit(double a);    // the interface
protected: void log();                // visible to subclasses
};
```

```java
// Java — enforced keywords per member
class Account {
    private double balance;
    public void deposit(double a) { /* ... */ }
    protected void log() { /* ... */ }
}
```

```python
# Python — access is by CONVENTION, not enforcement
class Account:
    def __init__(self):
        self.balance = 0      # public
        self._internal = 0    # "protected" by convention (single _)
        self.__secret = 0     # name-mangled (double __), discourages access
```

> **Critical cross-language insight:** C++ and Java **enforce** access; Python
> **trusts** the programmer ("we're all adults here"). The *concept* of hiding is
> identical; the *enforcement* differs.

---

## 2.4 Getters, Setters, and the Trap of "Fake" Encapsulation

A common misunderstanding: *"encapsulation means add a getter and setter for
every field."* This is often **encapsulation theater** — it exposes the same data
with extra ceremony.

```java
// ANTI-PATTERN: getter/setter that adds no protection
private double balance;
public double getBalance()        { return balance; }
public void   setBalance(double b){ this.balance = b; }   // ← no rules! = public field
```

```java
// REAL encapsulation: the interface enforces invariants
private double balance;
public double getBalance() { return balance; }            // read-only outside
public void withdraw(double amt) {
    if (amt <= 0)            throw new IllegalArgumentException("amount must be positive");
    if (amt > balance)      throw new IllegalStateException("insufficient funds");
    balance -= amt;                                         // invariant preserved
}
```

> **Rule of thumb:** Expose **behavior** (`withdraw`), not raw **data** (`setBalance`).
> Ask: *"Does this setter let an outsider break an invariant?"* If yes, it's not
> encapsulation.

---

## 2.5 Benefits & Trade-offs of Encapsulation

### Benefits
- **Protects invariants** — invalid state becomes impossible.
- **Localizes change** — you can rewrite internals without breaking callers.
- **Reduces coupling** — callers depend on the interface, not the data layout.
- **Easier debugging** — state changes flow through few, controlled paths.

### Trade-offs
- Slightly more code (interfaces, validation).
- Over-hiding can make legitimate access awkward.

---

# Part B — Abstraction

## 2.6 Definition (Conceptual)

**Abstraction is the practice of exposing only the essential features of a
concept while hiding the complex underlying details.** It focuses on **what** an
object does, not **how** it does it.

> **Analogy — A TV remote:** the *power button* abstracts away the electronics,
> infrared signaling, and circuitry. You think "turn on the TV," not "emit a 38kHz
> modulated IR pulse." The remote presents a **simplified model** of a complex
> machine.

> **Analogy — Driving:** "press the brake to stop" is an abstraction over hydraulic
> pressure, brake pads, and friction. You operate at the level of *intent*.

---

## 2.7 Why Abstraction Exists — Managing Complexity

Software systems are too complex to understand all at once. Abstraction lets you
**reason about a component by its contract**, ignoring its internals.

```text
WITHOUT abstraction: caller must understand every internal detail.
WITH abstraction:    caller knows only the INTERFACE (the contract).

   Caller ──► "save(document)"   ←── interface (the WHAT)
                    │
                    ▼
        [ encryption, disk I/O, retries, buffering ]  ←── hidden HOW
```

This enables **layered systems**, where each layer trusts the abstraction below it
(e.g., app → library → OS → hardware).

---

## 2.8 Mechanisms of Abstraction

| Mechanism | How it abstracts |
|-----------|------------------|
| **Abstract classes** | Define *what* operations exist; defer *how* to subclasses |
| **Interfaces / protocols** | A pure contract with no implementation |
| **Public methods** | Hide internal algorithm behind a named operation |

```cpp
// C++ — abstract base class = a contract
class Shape {
public:
    virtual double area() const = 0;   // pure virtual: "every shape HAS an area"
    virtual ~Shape() = default;        // HOW is left to each subclass
};
```

```java
// Java — interface = pure contract
interface Shape {
    double area();        // the WHAT; implementers provide the HOW
}
```

```python
# Python — duck typing / ABC
from abc import ABC, abstractmethod
class Shape(ABC):
    @abstractmethod
    def area(self): ...   # subclasses must implement
```

> **Interface vs implementation separation** is the heart of abstraction. The
> interface is a **promise**; the implementation is **how the promise is kept**.

---

## 2.9 Interface vs Implementation (The Conceptual Core)

```text
   ┌──────────────────────────────┐   ← INTERFACE (stable, public)
   │  area()   perimeter()         │      what clients depend on
   └──────────────────────────────┘
   ┌──────────────────────────────┐   ← IMPLEMENTATION (volatile, private)
   │  radius, formula, caching...  │      free to change anytime
   └──────────────────────────────┘
```

The golden design guidance: **"Program to an interface, not an implementation."**
Depend on the *stable* promise, not the *changeable* details. This single idea
underpins polymorphism (Ch.4), Dependency Inversion (Ch.6), and most design
patterns (Ch.7).

---

## 2.10 Encapsulation vs Abstraction — Side by Side

| Aspect | Encapsulation | Abstraction |
|--------|---------------|-------------|
| **Concern** | Implementation (how to hide) | Design (what to expose) |
| **Question** | "How do I protect my data?" | "What is the essential interface?" |
| **Achieved by** | Access modifiers, private state | Abstract classes, interfaces |
| **Goal** | Data safety & integrity | Complexity reduction |
| **Level** | Mostly within one class | Across the system / between modules |
| **Relationship** | Hides the *data* | Hides the *complexity/details* |

> **They reinforce each other:** abstraction decides *what* the interface should
> be; encapsulation *enforces* that nothing else leaks through. You design with
> abstraction and protect with encapsulation.

```text
   ABSTRACTION says:  "Clients should only see deposit/withdraw/getBalance."
   ENCAPSULATION says:"...and I'll make 'balance' private so nothing else can touch it."
```

---

## 2.11 Real-World System Example — Inventory Module

```text
PUBLIC INTERFACE (abstraction):
   addStock(item, qty)
   removeStock(item, qty)
   getAvailable(item)

HIDDEN INTERNALS (encapsulation):
   storage layout (hash map? database?), locking, audit logging,
   reorder thresholds, caching strategy
```

Because the *interface* is stable, you can later swap the in-memory store for a
database **without changing a single caller** — that's abstraction + encapsulation
delivering **maintainability**.

---

## Chapter 2 Cheat Sheet

```text
ENCAPSULATION  (implementation concern)
   bundle data + behavior; make state private; expose guarded interface
   → protects INVARIANTS; localizes change; reduces coupling
   access: public / protected / private  (enforced in C++/Java, convention in Python)

ABSTRACTION    (design concern)
   expose the essential WHAT; hide the complex HOW
   tools: abstract classes, interfaces/protocols
   → manages complexity; enables layered systems

DISTINCTION
   Abstraction = "show only what matters"  (interface design)
   Encapsulation = "protect the internals" (data hiding)

GOLDEN RULES
   "Program to an interface, not an implementation."
   "Expose behavior, not data." (avoid getter/setter theater)
```

**Key Takeaways**
- Encapsulation hides **data**; abstraction hides **complexity**.
- Real encapsulation enforces invariants — not just getters/setters.
- Abstraction = the stable interface; implementation = the changeable internals.
- Together they deliver safety + maintainability + low coupling.

---

## Common Mistakes
1. **Getter/setter theater** — exposing every field defeats encapsulation.
2. **Leaky abstraction** — interface forces callers to know internal details.
3. Equating the two pillars — they solve different problems.
4. Over-abstracting trivial concepts → needless indirection (see [Ch.8](08_antipatterns_and_mistakes.md)).

## When NOT to Over-Apply
- Don't add an interface for a class that will only ever have one implementation
  and no testing/extension need (premature abstraction).
- Don't make everything private if it genuinely is part of the contract.

---

*Next: [Chapter 3 — Inheritance](03_inheritance.md)*
