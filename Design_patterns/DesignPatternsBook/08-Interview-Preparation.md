# Chapter 8 — Interview Preparation

> Questions progress Beginner → Advanced, then pattern-identification drills, scenario design problems, and C++-specific questions. Answers are included — try to answer before reading them.

Sections:
- [8.1 Beginner Questions](#81-beginner-questions)
- [8.2 Intermediate Questions](#82-intermediate-questions)
- [8.3 Advanced Questions](#83-advanced-questions)
- [8.4 Pattern Identification Drills](#84-pattern-identification-drills)
- [8.5 Scenario-Based Design Problems](#85-scenario-based-design-problems)
- [8.6 C++-Specific Pattern Questions](#86-c-specific-pattern-questions)

---

## 8.1 Beginner Questions

**Q1. What is a design pattern? Is it code you reuse?**
A reusable *solution template* for a recurring design problem — a description of how roles collaborate, not copy-paste code. It's a *vocabulary* and a *blueprint*, not a library.

**Q2. Name the three GoF categories and what each addresses.**
- *Creational* — object creation.
- *Structural* — object composition/structure.
- *Behavioral* — object interaction and responsibility.

**Q3. Why prefer composition over inheritance?**
Inheritance is a permanent compile-time "is-a" bond and causes combinatorial subclass explosion; composition is a runtime, swappable "has-a" relationship that's more flexible and avoids tight coupling to a base class.

**Q4. What problem does Strategy solve?**
It replaces large conditionals that select among algorithms with interchangeable algorithm objects, so behavior can be swapped at runtime and new algorithms added without modifying the context (OCP).

**Q5. What is a Singleton and why is it controversial?**
It ensures one instance with global access. Controversial because it's hidden global state: hurts testability, hides dependencies, creates coupling, and complicates concurrency. Often an anti-pattern; prefer dependency injection.

**Q6. What does the Factory Method pattern do?**
It defines a creation method that subclasses override to decide which concrete product to instantiate, removing hardcoded `new ConcreteType()` from client logic.

**Q7. Give a real-world analogy for Observer.**
A magazine subscription: subscribers (observers) register; when a new issue (state change) is published, the publisher (subject) notifies all current subscribers.

**Q8. What is the Decorator pattern in one sentence?**
Wrap an object in another object of the same interface to add behavior dynamically, stackably, without subclassing.

---

## 8.2 Intermediate Questions

**Q9. Strategy and State look identical. How do they differ?**
Structure is the same; *intent* differs. Strategy: the client picks one of several interchangeable algorithms, usually fixed for the task. State: behavior changes as the object moves through internal states, and states often trigger transitions among themselves. (Strategy = "how"; State = "mode over time.")

**Q10. When would you choose Abstract Factory over Factory Method?**
When you must create *families of related products* that have to be used together (e.g., a themed UI's matching button + checkbox), not just one product. Abstract Factory guarantees family consistency; it's typically built from several Factory Methods.

**Q11. How does Observer avoid memory leaks / dangling listeners?**
By managing subscription lifetimes: observers must detach, or the subject holds non-owning references (in C++, `weak_ptr`) so dead observers are detected and pruned. Otherwise you get the *lapsed-listener* leak.

**Q12. What's the difference between Adapter and Facade?**
Adapter converts one interface into another *expected* interface (usually one class). Facade invents a *new, simpler* interface over a *whole subsystem*. Adapter = compatibility; Facade = simplification.

**Q13. How does Command enable undo?**
Each operation is an object with `execute()` and `undo()`. The invoker keeps a history stack; undo pops the last command and calls `undo()`, which reverses the effect (by inverse operation or by restoring a stored Memento).

**Q14. Decorator vs Proxy — same structure, what's the difference?**
Both wrap an object with the same interface. Decorator *adds behavior* (and is often stacked). Proxy *controls access* (lazy creation, security, caching, remoting) and usually manages the real subject's lifecycle.

**Q15. What is the Template Method pattern and the Hollywood Principle?**
A base method defines a fixed algorithm skeleton calling overridable steps. "Don't call us, we'll call you" — the framework's skeleton calls your hook methods, inverting control.

**Q16. How does Chain of Responsibility differ from Decorator (both chains)?**
Decorator's layers *all* execute and each contributes. CoR's handlers *compete*; typically one handles the request and the chain may stop.

**Q17. What is double dispatch and which pattern uses it?**
Selecting behavior based on the runtime types of *two* objects. Visitor simulates it: `element.accept(visitor)` resolves the element type, then `visitor.visit(*this)` resolves the visitor/operation type.

---

## 8.3 Advanced Questions

**Q18. Why does adding a new product type break Abstract Factory, and adding a new operation break the element classes — but Visitor flips this?**
Abstract Factory is easy to extend with new *families* but hard with new *product kinds* (every factory changes). Element hierarchies are easy to extend with new *types* but hard with new *operations* (every type changes). Visitor trades these: new *operations* are easy (new visitor), new *element types* are hard (every visitor changes). This is the **expression problem** — no single OO arrangement makes both axes easy.

**Q19. How do you make a thread-safe Singleton in modern C++?**
Use the Meyers' Singleton: a function-local `static` whose initialization is guaranteed thread-safe since C++11. No manual double-checked locking needed. Return by reference; delete copy/move.

**Q20. When would you replace a runtime pattern with a compile-time one?**
When performance/determinism matters and the variation is known at compile time — e.g., replace virtual Strategy with a *template policy* or **CRTP** to eliminate vtable/heap overhead (common in embedded/HPC). Trade runtime flexibility for zero-overhead static dispatch.

**Q21. How does the Bridge pattern relate to PIMPL in C++?**
PIMPL (pointer-to-implementation) is a degenerate Bridge: the public class (abstraction) holds a `unique_ptr<Impl>` (implementation) hidden in the .cpp. It decouples interface from implementation, reduces compile-time dependencies, and stabilizes ABI.

**Q22. What's the expression-problem-friendly modern C++ alternative to Visitor?**
`std::variant` + `std::visit` with an overloaded callable set. For a *closed* set of types it gives value-based, allocation-free double dispatch without the `accept` boilerplate.

**Q23. How do you prevent reference cycles when patterns create back-references (e.g., Observer, Composite parent links)?**
Make exactly one direction non-owning: use `weak_ptr` (or raw non-owning pointers) for back-references/observers, and `shared_ptr`/`unique_ptr` for the owning direction. This breaks the cycle and avoids leaks.

**Q24. Why return `unique_ptr` from factory methods instead of raw pointers?**
It makes ownership explicit and transfers it to the caller, guaranteeing no leaks via RAII. (Caveat: it disables covariant return types, so all overrides must return the same base `unique_ptr` type.)

**Q25. Explain how RAII underlies several patterns.**
RAII ties resource lifetime to object lifetime. Composite frees subtrees recursively via destructors; Decorator/Proxy chains free inner objects automatically; Command history stacks destroy commands on pop; smart pointers (a Proxy) manage memory deterministically. RAII makes the C++ implementations leak-free without manual cleanup.

**Q26. What is the NVI (Non-Virtual Interface) idiom and how does it relate to Template Method?**
NVI: make the public interface non-virtual and the customization points private/protected virtuals. It's Template Method at the method level — the non-virtual public method enforces invariants (pre/post checks, locking) around the virtual steps.

---

## 8.4 Pattern Identification Drills

For each, name the pattern.

1. *"A class exposes `begin()`/`end()` so clients can traverse it with range-for without knowing it's backed by a ring buffer."* → **Iterator**.
2. *"Each UI button, menu item, and shortcut triggers an object with `execute()`/`undo()` stored in a history stack."* → **Command**.
3. *"A `Renderer` interface (OpenGL/Vulkan) is held by a `Shape` hierarchy (Circle/Square) so both vary independently."* → **Bridge**.
4. *"`shared_ptr` adds reference counting around a raw pointer with the same dereferencing interface."* → **Proxy** (smart reference).
5. *"A `Directory` contains `File`s and other `Directory`s; `size()` recurses uniformly."* → **Composite**.
6. *"Adding milk and sugar by wrapping a `Coffee` in `WithMilk(WithSugar(...))`."* → **Decorator**.
7. *"A document moves Draft → Moderation → Published; each phase behaves differently and triggers the next."* → **State**.
8. *"A `WeatherStation` notifies all registered displays when temperature changes."* → **Observer**.
9. *"One `GUIFactory` makes a matching set of Windows widgets; another makes macOS widgets."* → **Abstract Factory**.
10. *"A request flows through auth → validation → processing, each handler deciding to handle or pass on."* → **Chain of Responsibility**.
11. *"A class clones itself via a virtual `clone()` returning a copy of its dynamic type."* → **Prototype**.
12. *"A `VideoConverter::convert()` hides a dozen subsystem calls behind one method."* → **Facade**.
13. *"Millions of trees share one immutable mesh/texture; only position is per-tree."* → **Flyweight**.
14. *"An undo system stores opaque snapshots the editor can restore but the caretaker can't read."* → **Memento**.
15. *"A new `AreaVisitor`/`DrawVisitor` adds operations over a fixed `Shape` hierarchy via `accept`."* → **Visitor**.
16. *"Dialog widgets don't talk to each other; they all notify a controller that coordinates them."* → **Mediator**.
17. *"`std::sort` takes a comparator deciding the ordering algorithm."* → **Strategy** (compile-time).
18. *"`prepare()` runs boil → brew → pour → addCondiments; subclasses override brew/addCondiments."* → **Template Method**.

---

## 8.5 Scenario-Based Design Problems

**Scenario A — Payment processing.**
*"Design a checkout that supports credit card, PayPal, and crypto, with more methods added later."*
- **Strategy** for the payment algorithm (`PaymentMethod` interface), chosen at runtime; **Abstract Factory** if each method needs a *family* of related objects (gateway + validator + receipt); **Command** if payments must be queued/retried/undone; depend on the `PaymentMethod` abstraction (DIP) so adding crypto = a new class (OCP).

**Scenario B — Text editor with undo/redo.**
- **Command** for each edit (`execute`/`undo`) with two history stacks; **Memento** to snapshot complex state when an inverse op is impractical; **Composite** commands for macros (group edits); possibly **Flyweight** for character formatting.

**Scenario C — Cross-platform notification system (email, SMS, push), with formatting and filtering.**
- **Abstract Factory** or **Strategy** for channel; **Observer** to fan a single event out to multiple channels; **Decorator** to add retry/encryption to a channel; **Chain of Responsibility** for filters (do-not-disturb, rate limit); **Template Method** for the send skeleton (format → validate → dispatch).

**Scenario D — Game with thousands of entities and AI.**
- **Flyweight** for shared sprites/meshes; **State** for entity AI (Patrol → Chase → Attack → Flee); **Strategy** for movement/targeting algorithms; **Observer** for events (entity died → update score/UI); **Object Pool** (creational, beyond GoF) to recycle bullets/particles; **Command** for replay/input buffering.

**Scenario E — HTTP server middleware.**
- **Chain of Responsibility** for the middleware pipeline (logging → auth → rate-limit → router → handler); **Decorator** to wrap handlers with caching/compression; **Strategy** for routing/serialization; **Factory** to build handlers from config.

> **Interview technique:** state the *varying axis* first ("the payment method varies"), name the pattern, then justify in **SOLID** terms ("OCP: new method = new class; DIP: checkout depends on the interface"), then mention the C++ mechanism (`unique_ptr`, `std::function`, or template policy) and lifetime/ownership.

---

## 8.6 C++-Specific Pattern Questions

**Q27. Which smart pointer for the wrapped object in Decorator, and why?**
`unique_ptr<Component>` — exclusive ownership; the chain forms a single owning line freed innermost-last by RAII. No shared ownership needed.

**Q28. Why `weak_ptr` in Observer?**
The subject must not own observers (they have independent lifetimes). `weak_ptr` lets the subject notify if alive and prune if expired, preventing dangling pointers and the lapsed-listener leak.

**Q29. How do you copy a polymorphic object held by base pointer without slicing?**
The **virtual clone** idiom: a virtual `clone()` returning `unique_ptr<Base>` that each derived class implements via `make_unique<Derived>(*this)`. A plain copy would slice to the base.

**Q30. Why must a polymorphic base have a virtual destructor in these patterns?**
Clients hold and delete through base pointers/`unique_ptr<Base>`. Without a virtual destructor, destroying a derived object via a base pointer is undefined behavior and leaks the derived part.

**Q31. How do you avoid heap allocation for Strategy/State in performance-critical code?**
Use templates/CRTP for static polymorphism (compile-time strategy, no vtable/heap), share stateless State objects as singletons, or store strategies as `std::function` only when allocation is acceptable.

**Q32. Where does the C++ standard library itself use these patterns?**
`std::stack`/`std::queue` = Adapter (container adapters); iterators across all containers = Iterator; `shared_ptr` = Proxy (smart reference); `std::sort` comparator = Strategy; `std::variant`+`std::visit` = Visitor-like double dispatch; streams/filters = Decorator-like; function-local statics = Meyers' Singleton mechanism.

**Q33. How do move semantics improve Builder?**
Setters take parameters by value and `std::move` them into members (one copy/move total, efficient for both lvalues/rvalues). Rvalue-ref-qualified builders can move internals straight into the immutable product, avoiding a final copy.

**Q34. Explain `std::variant` + `std::visit` as a Visitor alternative.**
For a closed set of types, model elements as a `variant` and apply an overloaded callable via `std::visit`. It gives value-based, allocation-free, type-safe double dispatch without `accept` boilerplate or virtual dispatch — but only works for a *fixed* type set.

**Q35. What's the danger of `shared_ptr` cycles in Composite/Observer, and the fix?**
Two objects owning each other via `shared_ptr` never reach refcount zero → leak. Fix: make back-references/parent-links `weak_ptr` (or raw non-owning), keeping a single owning direction.

---

*Next: [Chapter 9 — Cheatsheets →](09-Cheatsheets.md)*
