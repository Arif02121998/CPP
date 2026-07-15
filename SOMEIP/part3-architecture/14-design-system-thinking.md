# Chapter 14: Design & System Thinking

[← Previous: Chapter 13](13-middleware.md) | [Back to TOC](../README.md) | [Next: Chapter 15 →](15-message-flows.md)

---

## 14.1 Loose coupling vs tight coupling

| | Tight coupling | Loose coupling |
|---|----------------|----------------|
| Depends on | concrete location, byte layout, timing | interface contract only |
| Change impact | ripples across ECUs | contained behind interface |
| Example | CAN signal at fixed ID | SOME/IP service discovered at runtime |
| Risk | brittle, hard to evolve | more moving parts |

**Goal:** depend on the **interface**, not on **who/where/how**. SOME/IP enables
loose coupling via discovery, serialization, and versioning — but you can still
*design* tight coupling by leaking implementation details into interfaces.

### How to keep services loosely coupled
- Design interfaces around **capabilities**, not internal data structures.
- Avoid exposing **implementation-specific** fields or assumptions.
- Use **versioning** and optional (TLV) fields to evolve without breaking.
- Don't assume a fixed **provider location** or **startup order**.

## 14.2 Scalability in large distributed systems

Strategies SOME/IP supports:

- **Multicast eventgroups** — fan-out to many consumers at constant provider
  cost.
- **Multiple service instances** — load distribution / redundancy.
- **Right-sizing transport** — UDP for small/frequent, TCP/TP for large.
- **Subscription TTLs** — automatic cleanup keeps state bounded.
- **Domain/zonal partitioning** — localize traffic; gateways bridge domains.

```
        Zonal architecture
   ┌──────────┐    Ethernet backbone    ┌──────────┐
   │ Zone ECU │════════════════════════│ Zone ECU │
   │  (front) │         │              │  (rear)  │
   └────┬─────┘     ┌────▼─────┐        └────┬─────┘
   CAN/LIN sensors  │ Central  │        CAN/LIN actuators
                    │ Compute  │
                    └──────────┘
```

## 14.3 Worked example: infotainment ↔ vehicle data

**Requirement:** the head unit shows live speed, range, and door status, and can
send "lock all doors."

```
 SpeedService (provider: vehicle domain controller)
   • Field speed (UDP eventgroup, multicast, on-change + cyclic 100 ms)
 EnergyService (provider: BMS gateway)
   • Field range (UDP eventgroup, on-change)
 DoorService (provider: body controller)
   • Event  DoorStateChanged (UDP)
   • Method LockAll() -> status (UDP, request/response)

 Head Unit (consumer)
   • Discovers all three via SD
   • Subscribes to speed/range/door eventgroups
   • Calls DoorService.LockAll() on user tap
```

**Design choices & why:**
- Speed/range/door updates are **events/fields** (push) → no polling.
- `LockAll` is a **request/response method** → needs confirmation.
- All on **UDP** (small payloads, low latency); multicast for speed (many
  consumers: cluster, HUD, head unit).

## 14.4 Reliability & failure thinking

- **Provider down:** consumers detect via **TTL expiry** of the offer → show
  "data unavailable," retry discovery.
- **Lost UDP event:** acceptable for cyclic data (next update corrects it); use
  **E2E protection** or TCP where loss is unacceptable.
- **Startup ordering:** never assume order — rely on SD; buffer/queue until the
  service is found.

## 14.5 Tradeoffs to weigh in design

| Decision | Pulls toward | At the cost of |
|----------|--------------|----------------|
| UDP + multicast | scalability, low latency | reliability, debuggability |
| TCP | reliability | latency, jitter |
| Fine-grained services | reuse, flexibility | more SD traffic, overhead |
| Coarse services | fewer messages | weaker cohesion, bigger payloads |
| TLV serialization | evolvability | bandwidth, CPU |

## 14.6 Real-world analogy

Designing services is **city planning**. Loose coupling = standardized utility
hookups so any building can connect; scalability = zoning and shared infra so the
city grows without rewiring everything; failure thinking = redundant water/power
so one outage doesn't darken the whole city.

---

[← Previous: Chapter 13](13-middleware.md) | [Back to TOC](../README.md) | [Next: Chapter 15 →](15-message-flows.md)
