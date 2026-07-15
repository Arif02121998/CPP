# Chapter 1 — Classes & Objects (and the Object Memory Model)

> *Level: Beginner*
> Back to [Table of Contents](README.md)

Everything in OOP starts here. Before pillars, principles, or patterns, you must
deeply understand what a **class** and an **object** actually are.

---

## 1.1 The Core Idea

### Definition (Conceptual)

- A **class** is a **blueprint** or **template** — it defines what data
  (attributes) and behavior (methods) a kind of thing has. A class is a
  *description*; it occupies no "real" runtime existence on its own.
- An **object** (or *instance*) is a **concrete thing built from that blueprint**
  — it has actual state stored in memory and can perform the behaviors the class
  defines.

> **Analogy:** A class is the **architectural plan** for a house. An object is an
> **actual house** built from that plan. One plan → many houses, each with its own
> address, paint color, and occupants, but all sharing the same structure.

### Another Analogy — The Cookie Cutter

```text
   CLASS (cookie cutter)             OBJECTS (cookies)
        ┌─┐                       🍪      🍪      🍪
        │★│        ──────►       (each one separate,
        └─┘                       same shape, different
                                  sprinkles/state)
```

The cutter defines the *shape*; each cookie is a separate instance with its own
existence.

---

## 1.2 Why Classes Exist — The Procedural Limitation

In procedural code, the "concept" of an account is split apart:

```text
PROCEDURAL: data here, behavior there, nothing links them.
   struct Account { double balance; };        // just data
   void deposit(Account* a, double amt);       // separate function
   void withdraw(Account* a, double amt);      // separate function
```

Problems:
- Nothing **forces** `deposit`/`withdraw` to be the *only* way to change balance.
- Anyone can write `account.balance = -9999;` directly, breaking invariants.
- The "account concept" isn't a single unit you can reason about.

A **class unifies state and behavior** into one named concept that owns and
protects its data.

---

## 1.3 Attributes (State) and Methods (Behavior)

Every object has:
- **Attributes / fields / properties** — the data it holds (its *state*).
- **Methods / operations** — the functions it can perform (its *behavior*).

```text
OBJECT = STATE + BEHAVIOR + IDENTITY
   State    : the current values of its attributes
   Behavior : the methods it exposes
   Identity : what makes it distinct from other objects (even with equal state)
```

> Two `$100` accounts owned by "Alice" are still **two different objects** — they
> have separate **identity**.

---

## 1.4 The Same Concept in Three Languages

```cpp
// C++
class BankAccount {
    double balance;                       // attribute (state)
public:
    BankAccount(double initial) : balance(initial) {}   // constructor
    void deposit(double amt) { balance += amt; }        // behavior
    double getBalance() const { return balance; }
};
BankAccount acc(100.0);                   // an OBJECT
acc.deposit(50.0);
```

```java
// Java
class BankAccount {
    private double balance;               // attribute
    BankAccount(double initial) { balance = initial; }   // constructor
    void deposit(double amt) { balance += amt; }         // behavior
    double getBalance() { return balance; }
}
BankAccount acc = new BankAccount(100.0); // an OBJECT
acc.deposit(50.0);
```

```python
# Python
class BankAccount:
    def __init__(self, initial):          # constructor
        self._balance = initial           # attribute (convention: _ = "private")
    def deposit(self, amt):               # behavior
        self._balance += amt
    def get_balance(self):
        return self._balance

acc = BankAccount(100.0)                  # an OBJECT
acc.deposit(50.0)
```

**Same concept, three dialects.** Note the differences already visible:
- C++ has `const` methods and explicit access sections.
- Java requires `new` and wraps everything in a class.
- Python uses explicit `self` and access is by *convention*, not enforcement.

---

## 1.5 Constructors & Destructors (Lifecycle)

### Construction

A **constructor** runs when an object is created. Its job is to put the object in
a **valid initial state** (establish its invariants).

> **Analogy:** Building the house from the plan — you don't hand over a house with
> no floor. The constructor guarantees the object is "move-in ready."

### Destruction / Cleanup

An object eventually ceases to exist. Some languages run a **destructor**
(C++) deterministically; others rely on **garbage collection** (Java, Python) to
reclaim memory later.

| Language | Cleanup Model |
|----------|---------------|
| C++ | **Deterministic** destructors (`~Class()`), often via RAII |
| Java | **Garbage collected**; `finalize` deprecated; use `try-with-resources` |
| Python | **Reference counting** + GC; `__del__` exists but is non-deterministic |

> This is a major conceptual divide: in C++ *you* (or RAII) control lifetime; in
> GC languages the runtime decides *when* memory is freed.

---

## 1.6 The Object Memory Model (Conceptual)

You don't need a specific language's exact layout, but a **mental model** of how
objects live in memory clarifies many later topics (polymorphism, copying,
references).

### Where Objects Live

```text
        STACK (automatic, fast)            HEAP (dynamic, flexible)
   ┌───────────────────────────┐    ┌────────────────────────────────┐
   │ local variables           │    │ objects created "dynamically"   │
   │ a reference/pointer ───────┼───►│  ┌──────────────────────────┐   │
   │ (e.g. 'acc' handle)       │    │  │ BankAccount object       │   │
   └───────────────────────────┘    │  │   balance = 150.0        │   │
                                     │  │   owner   = "Alice"      │   │
                                     │  └──────────────────────────┘   │
                                     └────────────────────────────────┘
```

- A **variable** often holds a **reference/handle** (or pointer) to the actual
  object data that lives elsewhere (commonly the heap).
- In Java and Python, object variables are **always references** to heap objects.
- In C++, an object can live **directly on the stack** *or* on the heap (via
  `new`/smart pointers) — a key C++ distinction.

### State Is Per-Object; Methods Are Shared

A crucial conceptual point:

```text
   Each object has its OWN copy of the attributes (state).
   But there is ONE shared copy of the method code for the whole class.

   obj1.balance = 100   ┐
   obj2.balance = 250   ├─ separate state per object
   obj3.balance = 0     ┘

   deposit() code  ── one implementation, used by all objects
```

When you call `obj1.deposit(50)`, the method receives a hidden reference to
`obj1` (called `this` / `self`) so it knows *which* object's state to modify.

> **`this` / `self`** is simply "the object the method was called on." It's how
> shared method code operates on per-object state.

### Static (Class-Level) vs Instance Members

```text
   Instance member  → one per OBJECT  (each account's own balance)
   Static member    → one per CLASS   (e.g., total number of accounts ever created)
```

```cpp
class BankAccount {
    static int accountCount;     // shared by ALL objects (class-level)
    double balance;              // unique per object (instance-level)
};
```

---

## 1.7 Object Identity, Equality, and Copying

Three different questions you must not confuse:

| Question | Meaning |
|----------|---------|
| **Identity** | Are these two references the *same* object? (same memory) |
| **Equality** | Do these two objects have *equivalent state*? (same values) |
| **Copy** | Create a new object with the same state as an existing one |

```python
a = BankAccount(100)
b = a                 # same object (identity): a is b  → True
c = BankAccount(100)  # different object, equal-ish state: a is c → False
```

> **Interview trap:** "`a == b`" may test *equality* (values) while "`a is b`"
> (Python) or pointer comparison (C++/Java `==` on references) tests *identity*.
> Languages differ — Java `==` compares references; `.equals()` compares values.

---

## 1.8 Benefits & Trade-offs of the Class/Object Model

### Benefits
- **Modularity** — each class is a self-contained unit.
- **Reusability** — instantiate the same class many times.
- **Maintainability** — everything about a concept lives in one place.
- **Models reality** — code mirrors the problem domain.

### Trade-offs
- **Overhead** — objects can carry memory/indirection cost vs raw data.
- **Over-modeling** — wrapping trivial data in classes adds ceremony.
- **Learning curve** — lifecycle, references, and identity require care.

### When to Use / When NOT To
- **Use** classes when a concept has both **state and behavior**, or when you need
  many similar instances with protected invariants.
- **Don't** create a class for a pure, stateless one-off calculation — a plain
  function is clearer (not everything needs to be an object).

---

## Chapter 1 Cheat Sheet

```text
CLASS   = blueprint (description; no runtime state of its own)
OBJECT  = instance built from a class (state + behavior + identity)

OBJECT = STATE (per-object attributes)
       + BEHAVIOR (shared method code)
       + IDENTITY (distinct existence)

this / self   = "the object this method was called on"
static member = one per CLASS; instance member = one per OBJECT

MEMORY: variables often hold REFERENCES to objects (heap).
        Java/Python: always references. C++: stack OR heap.

IDENTITY ≠ EQUALITY ≠ COPY
LIFECYCLE: constructor (make valid) → use → destructor/GC (clean up)
```

**Key Takeaways**
- A class describes; an object exists.
- Each object owns its state; method code is shared and uses `this`/`self`.
- Know where objects live (stack vs heap; reference vs value).
- Distinguish identity, equality, and copying — languages differ.

---

## Common Mistakes
1. Confusing the class (blueprint) with the object (instance).
2. Assuming `==` always compares *values* (in Java it compares references).
3. Treating a copied reference as a new object (it's the same object).
4. Forgetting that constructors exist to **establish invariants**, not just set fields.

## When NOT to Use a Class
- Stateless utility logic → a free function/module is simpler.
- A throwaway data bag with no behavior and no invariants → a plain record/struct.

---

*Next: [Chapter 2 — Encapsulation & Abstraction](02_encapsulation_and_abstraction.md)*
