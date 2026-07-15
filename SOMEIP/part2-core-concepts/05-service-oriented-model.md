# Chapter 5: The Service-Oriented Communication Model

[← Previous: Chapter 4](../part1-foundations/04-terminology.md) | [Back to TOC](../README.md) | [Next: Chapter 6 →](06-client-server.md)

---

## 5.1 Definition

The **service-oriented communication model** is a way of structuring system
communication around **services** — self-contained units that expose a
well-defined **interface** (methods, events, fields) — rather than around raw
signals or messages. Consumers interact with the *interface contract*, never
with the provider's internals or its physical location.

```
        ┌──────────────── Interface Contract ────────────────┐
        │  Service: SpeedService (ID 0x0100, v1)             │
Consumer│   • getSpeed() -> uint16                            │ Provider
   App ─┼──▶• Method  setLimit(uint16) -> ack                │─── App
        │   • Event   onSpeedChanged -> uint16               │
        │   • Field   currentSpeed (get/notify)             │
        └────────────────────────────────────────────────────┘
            (consumer never sees provider's code or wiring)
```

## 5.2 Why it is needed

- **Decoupling:** Teams agree on an interface, then build provider and consumer
  independently and in parallel.
- **Relocatability:** A service can move to another ECU; consumers find it again
  via Service Discovery without code changes.
- **Evolvability:** Add a new method or event without breaking existing
  consumers (with proper versioning).
- **Reuse:** The same service definition is reused across vehicle programs.

## 5.3 Problem it solves

In signal-based systems, *meaning*, *location*, and *transport* are welded
together: "RPM is bits 0–7 of CAN ID 0x120 on the powertrain bus." Move it, and
everything breaks. The service model **separates the contract (what) from the
deployment (where/how)**, eliminating the brittle, system-wide coupling.

## 5.4 The three interaction primitives

| Primitive | Direction | Returns? | Typical use |
|-----------|-----------|----------|-------------|
| **Method** (R/R) | Consumer → Provider → Consumer | Yes | Query/command with result |
| **Method** (fire&forget) | Consumer → Provider | No | One-way command |
| **Event** | Provider → Consumer(s) | n/a | State change / streaming |
| **Field** | Both (get/set + notify) | Yes (get/set) | Stateful value with last-known |

A **field** is the important "stateful" primitive: it always has a current
value, supports `Getter`/`Setter` methods, and an event-style `Notifier`. This
gives late-joining subscribers an immediate initial value.

## 5.5 Tradeoffs

- **Pros:** loose coupling, runtime flexibility, parallel development, reuse.
- **Cons:** interface/versioning governance, serialization cost, discovery
  overhead, harder worst-case timing analysis than static signals.

## 5.6 Real-world analogy

A **restaurant menu**. The menu (interface) lists dishes (methods), daily
specials announced to subscribers (events), and the soup-of-the-day board that
always shows a current value (field). You order from the menu without knowing
the kitchen layout (implementation) or which chef (instance) cooks it.

---

[← Previous: Chapter 4](../part1-foundations/04-terminology.md) | [Back to TOC](../README.md) | [Next: Chapter 6 →](06-client-server.md)
