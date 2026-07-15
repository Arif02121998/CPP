# Chapter 3 — Inheritance (All Types)

> *Level: Intermediate*
> Back to [Table of Contents](README.md)

Inheritance is the most **powerful and most misused** OOP feature. This chapter
explains every form, the mental models, the famous pitfalls, and — crucially —
*when not to use it*.

---

## 3.1 Definition (Conceptual)

**Inheritance lets a new class (the *derived*/*child*/*subclass*) be defined in
terms of an existing class (the *base*/*parent*/*superclass*), automatically
acquiring its attributes and behavior, and optionally extending or modifying
them.**

It models an **"is-a"** relationship:

```text
   "A Dog IS-A Animal"     →  Dog inherits from Animal
   "A SavingsAccount IS-A Account"  →  SavingsAccount inherits from Account
```

> **Analogy:** A child inherits traits from a parent (eye color, surname) but can
> also have unique traits and behave differently. The child *is a* member of the
> family, plus more.

---

## 3.2 Why Inheritance Exists — The Limitation It Addresses

Without inheritance, common behavior must be **duplicated** across related types:

```text
WITHOUT inheritance (copy-paste):
   class Dog  { eat(); sleep(); bark(); }
   class Cat  { eat(); sleep(); meow(); }   // eat()/sleep() duplicated!
   class Bird { eat(); sleep(); fly();  }   // again duplicated!

WITH inheritance (shared base):
   class Animal { eat(); sleep(); }         // common behavior, defined once
   class Dog  : Animal { bark(); }
   class Cat  : Animal { meow(); }
   class Bird : Animal { fly();  }
```

Inheritance provides:
1. **Code reuse** — shared behavior lives in the base, written once.
2. **Polymorphism enablement** — a base reference can refer to any derived object
   (the foundation of [Chapter 4](04_polymorphism_and_dynamic_binding.md)).
3. **Conceptual hierarchy** — expresses taxonomy/classification.

---

## 3.3 The Types of Inheritance

### 1. Single Inheritance
One base, one derived. The simplest and safest form.

```text
   Animal
     ▲
     │
    Dog
```

### 2. Multilevel Inheritance
A chain: derived class becomes a base for another.

```text
   Animal  ──►  Mammal  ──►  Dog
   (each level adds/specializes behavior)
```

### 3. Hierarchical Inheritance
Multiple derived classes share one base.

```text
            Animal
          ┌───┼───┐
        Dog  Cat  Bird
```

### 4. Multiple Inheritance
One class inherits from **more than one** base. Powerful but problematic.

```text
   Camera      Phone
       ▲        ▲
       └───┬────┘
        SmartPhone   (is-a Camera AND is-a Phone)
```

### 5. Hybrid / Diamond Inheritance
A combination that creates a **diamond shape** — the source of a classic problem.

```text
            Device
            ▲    ▲
            │    │
        Camera  Phone
            ▲    ▲
            └─┬──┘
          SmartPhone     ← inherits Device TWICE (the "diamond")
```

---

## 3.4 The Diamond Problem (and How Languages Solve It)

**The problem:** if `SmartPhone` inherits `Device` through both `Camera` and
`Phone`, does it get **two copies** of `Device`'s state? Which `powerOn()` wins?

```text
   Device has: batteryLevel
   SmartPhone via Camera → batteryLevel #1
   SmartPhone via Phone  → batteryLevel #2   ← ambiguous & duplicated!
```

### Language Solutions

| Language | Approach |
|----------|----------|
| **C++** | Allows multiple inheritance; solves diamond with **virtual inheritance** (one shared `Device`) — but it's complex. |
| **Java** | **Bans** multiple inheritance of classes. Allows multiple **interfaces** (no state → no diamond of data). |
| **Python** | Allows it; resolves method order via **MRO** (Method Resolution Order, C3 linearization). |

```cpp
// C++ — virtual inheritance dissolves the diamond
class Device { /* batteryLevel */ };
class Camera : virtual public Device {};   // 'virtual' → share one Device
class Phone  : virtual public Device {};
class SmartPhone : public Camera, public Phone {};  // only ONE Device subobject
```

```python
# Python — MRO determines which method runs
class Device:  pass
class Camera(Device): pass
class Phone(Device):  pass
class SmartPhone(Camera, Phone): pass
print(SmartPhone.__mro__)   # explicit, deterministic resolution order
```

> **Interview favorite:** "What is the diamond problem and how does each language
> solve it?" → ambiguity from inheriting the same base twice; C++ uses virtual
> inheritance, Java forbids class MI (interfaces only), Python uses MRO.

---

## 3.5 What Gets Inherited — and Overriding

A subclass typically inherits:
- **public** and **protected** members (part of the inheritable surface).
- **private** members exist in the object but are **not directly accessible**.

The subclass can:
- **Add** new members (extend).
- **Override** inherited behavior (replace the implementation) → enables runtime
  polymorphism (Ch.4).
- **Reuse** the base implementation by calling it (`super`, `Base::method`).

```java
class Account {
    void describe() { System.out.println("Generic account"); }
}
class SavingsAccount extends Account {
    @Override
    void describe() {
        super.describe();                 // reuse base behavior
        System.out.println("...with interest");   // then extend
    }
}
```

---

## 3.6 Constructor Chaining in Hierarchies

When a derived object is created, the **base part is constructed first**, then the
derived part. Destruction runs in reverse.

```text
   Construction order:   Device → Camera → SmartPhone   (base → derived)
   Destruction order:    SmartPhone → Camera → Device   (derived → base)
```

> **Why base-first?** The derived class may depend on the base being valid. You
> can't specialize an account before the account itself exists.

---

## 3.7 The Big Warning: Inheritance Is Overused

Inheritance creates the **tightest coupling** in OOP: the subclass depends on the
base's *implementation details*, not just its interface. A change in the base can
silently break every subclass — the **fragile base class problem**.

### The Classic Mistake: Inheriting for Reuse Instead of "Is-A"

```text
   WRONG: "Stack needs a list's methods, so Stack extends ArrayList"
          → Stack now exposes add(index), remove(index)... breaking its contract!
          → A Stack is NOT-A general list. This violates "is-a".

   RIGHT: Stack HAS-A list internally (composition) and exposes only push/pop.
```

> **Litmus test:** Only use inheritance when the subclass is *genuinely a
> substitutable kind of* the base (see **Liskov Substitution**, [Ch.6](06_solid_principles.md)).
> If you're inheriting just to **reuse code**, prefer **composition**
> ([Ch.5](05_relationships_and_composition.md)).

---

## 3.8 "Is-A" vs "Has-A" — Choosing Correctly

| Relationship | Mechanism | Example |
|--------------|-----------|---------|
| **is-a** | Inheritance | A `Car` *is-a* `Vehicle` |
| **has-a** | Composition | A `Car` *has-a* `Engine` |

```text
   Ask: "Is X truly a kind of Y, substitutable everywhere Y is expected?"
        YES → inheritance may be appropriate
        NO  → use composition ("X has-a Y" or "X uses-a Y")
```

The industry guidance — **"Favor composition over inheritance"** — is explored in
depth in [Chapter 5](05_relationships_and_composition.md).

---

## 3.9 Benefits & Trade-offs

### Benefits
- Eliminates duplication of shared behavior.
- Enables runtime polymorphism (base references → derived objects).
- Expresses clear taxonomy when one genuinely exists.

### Trade-offs / Dangers
- **Tightest coupling** in OOP (fragile base class problem).
- Deep hierarchies become rigid and hard to change.
- Multiple inheritance adds ambiguity (diamond problem).
- Easy to violate "is-a" and break substitutability (LSP).

---

## 3.10 When to Use / When NOT To

**Use inheritance when:**
- There's a true **is-a** relationship.
- Subtypes are genuinely **substitutable** for the base.
- You want polymorphic behavior over a stable base interface.

**Avoid inheritance when:**
- You only want to **reuse code** → use composition.
- The relationship is "has-a" or "uses-a".
- The hierarchy would be deep or the base unstable.
- You'd need to override most of the base's behavior (a sign it's the wrong base).

---

## Chapter 3 Cheat Sheet

```text
INHERITANCE = define new class from existing one; models "IS-A"
   Benefits: reuse + polymorphism + taxonomy
   Cost:     TIGHTEST coupling (fragile base class)

TYPES
   Single        A → B
   Multilevel    A → B → C
   Hierarchical  A → {B, C, D}
   Multiple      {A, B} → C
   Hybrid/Diamond  →  diamond problem

DIAMOND PROBLEM (inherit same base twice → ambiguity)
   C++   : virtual inheritance (one shared base)
   Java  : no class MI; multiple INTERFACES only
   Python: MRO (C3 linearization)

CONSTRUCTION: base → derived ; DESTRUCTION: derived → base

DECISION
   true "is-a" + substitutable  → inheritance
   just want code reuse / "has-a" → COMPOSITION
   "Favor composition over inheritance."
```

**Key Takeaways**
- Inheritance = "is-a" + reuse + polymorphism, but it's the tightest coupling.
- Know all five forms and the diamond problem's per-language solution.
- Base constructs before derived; destructs in reverse.
- Inheriting for *reuse* (not is-a) is the #1 OOP design mistake.

---

## Common Mistakes
1. Using inheritance to reuse code rather than express "is-a".
2. Deep hierarchies that become impossible to change (fragile base class).
3. Violating substitutability (subclass that breaks base's contract → LSP violation).
4. Reaching for multiple inheritance when interfaces + composition would be cleaner.

---

*Next: [Chapter 4 — Polymorphism & Dynamic Binding](04_polymorphism_and_dynamic_binding.md)*
