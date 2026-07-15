# Chapter 3: The Shift to Service-Oriented Architecture (SOA)

[← Previous: Chapter 2](02-why-someip.md) | [Back to TOC](../README.md) | [Next: Chapter 4 →](04-terminology.md)

---

## 3.1 Signal-oriented vs Service-oriented

| Aspect | Signal-Oriented (CAN) | Service-Oriented (SOME/IP) |
|--------|----------------------|----------------------------|
| Unit of design | Signal in a frame | **Service** with an interface |
| Knowledge | Static, design-time | Dynamic, runtime discovery |
| Direction | Mostly broadcast | Directed + pub/sub |
| Coupling | Tight (byte layout) | Loose (interface contract) |
| Data flow | Always-on, periodic | On-demand + on-change |
| Change cost | Re-flash many ECUs | Add/replace a service |

## 3.2 What is a "service"?

**Definition.** A *service* is a named, versioned collection of capabilities a
provider offers:

- **Methods** — callable functions (request/response or fire-and-forget).
- **Events** — notifications the provider pushes to subscribers.
- **Fields** — stateful values with a getter, setter, and change-notification.

A service has a **Service ID**, an **Interface Version**, and an **Instance ID**
so the same service can run as multiple instances.

```
Service: DoorControl  (Service ID 0x1234, v1)
 ├─ Method  LockDoor(doorId) -> status
 ├─ Method  UnlockDoor(doorId) -> status
 ├─ Event   DoorStateChanged  -> {doorId, open/closed}
 └─ Field   ChildLock  (get / set / on-change)
```

## 3.3 Why SOA in cars now?

- **Software-defined vehicles:** features ship as software, updated over the air.
- **Domain/zonal architectures:** a few powerful controllers replace dozens of
  small ECUs; they need flexible internal comms.
- **Reuse & portability:** a service can be developed once and reused across
  platforms; its location on the network can change.
- **Decoupled teams:** define the **interface** once, then provider and consumer
  teams work independently.

## 3.4 Tradeoffs of SOA

- **Pros:** loose coupling, reuse, runtime flexibility, independent evolution.
- **Cons:** discovery/versioning overhead, harder end-to-end timing analysis,
  more sophisticated tooling and governance required.

## 3.5 Real-world analogy

Signal-oriented design is a **hard-wired circuit board** — change one trace, you
respin the board. Service-oriented design is an **app store**: apps (services)
announce themselves, others discover and use them, and you can update an app
without rebuilding the phone.

---

[← Previous: Chapter 2](02-why-someip.md) | [Back to TOC](../README.md) | [Next: Chapter 4 →](04-terminology.md)
